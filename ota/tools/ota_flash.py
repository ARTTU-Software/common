#!/usr/bin/env python3
"""
OTA-over-CAN host tool for ART TU boards.

Mirrors common/ota/ota_protocol.h. Talks to any node through a python-can
interface (PCAN, candleLight/gs_usb, slcan, socketcan, kvaser, ...).

Examples:
    python ota_flash.py -i pcan -c PCAN_USBBUS1 scan
    python ota_flash.py -i pcan -c PCAN_USBBUS1 info  --node ECU
    python ota_flash.py -i pcan -c PCAN_USBBUS1 flash --node ECU --bin build/Debug/ART_26_ECU_V2.bin
    python ota_flash.py -i slcan -c COM7 flash --node ECU --bin app.bin --catch
"""

from __future__ import annotations

import argparse
import struct
import sys
import time
import zlib
from dataclasses import dataclass
from typing import Optional, Protocol

# ── protocol constants (keep in sync with ota_protocol.h) ─────────────────────

PROTOCOL_VERSION = 2

NODES = {
    "ECU": 0x01, "GF": 0x02, "GR": 0x03, "GIL": 0x04,
    "GIR": 0x05, "DASH": 0x06, "TELEM": 0x07, "LVSOC": 0x08,
}
NODE_NAMES = {v: k for k, v in NODES.items()}

ID_ENTER_REQUEST = 0x4F0
ID_BASE = 0x600


def id_cmd(n: int) -> int: return ID_BASE | (n << 4) | 0x0
def id_rsp(n: int) -> int: return ID_BASE | (n << 4) | 0x1
def id_data(n: int) -> int: return ID_BASE | (n << 4) | 0x2


OP_PING, OP_INFO, OP_START, OP_DATA, OP_END, OP_ABORT, OP_RESET, OP_STATUS = range(1, 9)
OP_ENTER = 0x10

INFO_BL_VERSION, INFO_APP_START, INFO_APP_MAX_SIZE, INFO_APP_SIZE, INFO_APP_CRC, INFO_FLAGS = range(6)
FLAG_APP_VECTOR_OK, FLAG_COMMITTED, FLAG_UPDATE_INTERRUPTED = 1, 2, 4

MODE_APP, MODE_BOOTLOADER = 0, 1

ST_OK = 0x00
STATUS_NAMES = {
    0x00: "OK", 0x01: "ERR_STATE", 0x02: "ERR_SIZE", 0x03: "ERR_ERASE", 0x04: "ERR_WRITE",
    0x05: "ERR_VERIFY", 0x06: "ERR_CRC", 0x07: "ERR_VECTOR", 0x08: "ERR_SEQ",
    0x09: "ERR_NOT_SAFE", 0x0A: "ERR_UNKNOWN_OP", 0x0B: "ERR_ARG", 0x0C: "ERR_INCOMPLETE",
}
ST_ERR_SEQ = 0x08

DATA_BYTES_PER_FRAME = 7
BLOCK_FRAMES = 64
BLOCK_BYTES = BLOCK_FRAMES * DATA_BYTES_PER_FRAME

BOOT_LISTEN_S = 0.050   # OTA_BOOT_LISTEN_MS in ota/port/ota_stm32.h
BOOT_SETTLE_S = 0.150   # bootloader clock/CAN init before the window + app start-up after it


class OtaError(Exception):
    pass


# ── transport ─────────────────────────────────────────────────────────────────

class Transport(Protocol):
    def send(self, can_id: int, data: bytes) -> None: ...
    def recv(self, timeout: float) -> Optional[tuple[int, bytes]]: ...


# USB serial adapters that speak slcan, for "-c auto".
SLCAN_USB_IDS = {
    (0x16D0, 0x117E): "CANable 2.0 (slcan firmware)",
}


def find_slcan_port() -> str:
    """COM port of the first known slcan adapter (CANable)."""
    try:
        from serial.tools import list_ports  # type: ignore
    except ImportError as exc:
        raise SystemExit("pyserial is required for slcan: pip install pyserial") from exc
    for port in list_ports.comports():
        if (port.vid, port.pid) in SLCAN_USB_IDS:
            return port.device
    raise OtaError("no CANable found (USB 16D0:117E): plug it in or pass -c COMx")


class PythonCanTransport:
    """python-can backed transport. Only RSP frames are delivered to the client."""

    def __init__(self, interface: str, channel: str, bitrate: int):
        try:
            import can  # type: ignore
        except ImportError as exc:
            raise SystemExit("python-can is required: pip install python-can") from exc
        self._can = can
        self.bus = can.Bus(interface=interface, channel=channel, bitrate=bitrate)
        # Hardware/driver filter: only OTA responses (0x6N1).
        self.bus.set_filters([{"can_id": 0x601, "can_mask": 0x70F, "extended": False}])

    def send(self, can_id: int, data: bytes) -> None:
        msg = self._can.Message(arbitration_id=can_id, data=data, is_extended_id=False)
        for _ in range(200):
            try:
                self.bus.send(msg, timeout=0.05)
                return
            except self._can.CanError:
                time.sleep(0.001)  # adapter TX queue full, back off
        raise OtaError("CAN adapter refused to transmit (bus off / no ACK?)")

    def recv(self, timeout: float) -> Optional[tuple[int, bytes]]:
        msg = self.bus.recv(timeout)
        if msg is None:
            return None
        return msg.arbitration_id, bytes(msg.data)

    def close(self) -> None:
        self.bus.shutdown()


# ── client ────────────────────────────────────────────────────────────────────

@dataclass
class Response:
    op: int
    status: int
    b2: int
    b3: int
    value: int


def status_name(st: int) -> str:
    return STATUS_NAMES.get(st, f"0x{st:02X}")


class OtaClient:
    def __init__(self, transport: Transport, node: int, log=print):
        self.t = transport
        self.node = node
        self.log = log

    # low level

    def _sleep(self, seconds: float) -> None:
        """Wait without talking on the bus. Transports with their own clock (simulator) provide sleep()."""
        getattr(self.t, "sleep", time.sleep)(seconds)

    def _drain(self) -> None:
        while self.t.recv(0.0) is not None:
            pass

    def _wait(self, op: int, timeout: float) -> Optional[Response]:
        deadline = time.monotonic() + timeout
        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                return None
            frame = self.t.recv(remaining)
            if frame is None:
                return None
            can_id, data = frame
            if can_id != id_rsp(self.node) or len(data) < 2:
                continue
            data = data.ljust(8, b"\x00")
            if data[0] != op:
                continue
            return Response(data[0], data[1], data[2], data[3], struct.unpack_from("<I", data, 4)[0])

    def _cmd(self, op: int, payload: bytes = b"", timeout: float = 0.5) -> Optional[Response]:
        # Drop anything already queued (late replies, or bytes left in the serial
        # buffer by a previous run: python-can's slcan does not flush on open), so
        # the reply we accept is the one to this command.
        self._drain()
        self.t.send(id_cmd(self.node), bytes([op]) + payload.ljust(7, b"\x00"))
        return self._wait(op, timeout)

    def _expect_ok(self, rsp: Optional[Response], what: str) -> Response:
        if rsp is None:
            raise OtaError(f"{what}: no response from node {NODE_NAMES.get(self.node, self.node)}")
        if rsp.status != ST_OK:
            raise OtaError(f"{what}: {status_name(rsp.status)}")
        return rsp

    # commands

    def ping(self, timeout: float = 0.2) -> Optional[int]:
        """Returns OTA_MODE_* or None when the node does not answer."""
        rsp = self._cmd(OP_PING, timeout=timeout)
        return None if rsp is None else rsp.b2

    def info(self, idx: int) -> int:
        return self._expect_ok(self._cmd(OP_INFO, bytes([idx])), f"INFO[{idx}]").value

    def enter_bootloader(self, timeout: float = 3.0, catch: bool = False) -> None:
        mode = self.ping()
        if mode == MODE_BOOTLOADER:
            return
        if mode == MODE_APP:
            self.t.send(ID_ENTER_REQUEST, bytes([self.node]) + b"OTA" + bytes(4))
            self._expect_ok(self._wait(OP_ENTER, 0.5), "ENTER")
        elif not catch:
            raise OtaError("node does not answer PING (wrong node/bitrate, or app not running OTA hook; try --catch)")
        else:
            self.log("No answer. Power-cycle / reset the node now, waiting to catch the bootloader...")
            timeout = max(timeout, 30.0)

        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            if self.ping(timeout=0.02 if catch else 0.1) == MODE_BOOTLOADER:
                return
        raise OtaError("node did not come up in bootloader mode")

    def flash(self, image: bytes, retries: int = 5) -> None:
        size = len(image)
        crc = zlib.crc32(image) & 0xFFFFFFFF

        app_start = self.info(INFO_APP_START)
        max_size = self.info(INFO_APP_MAX_SIZE)
        check_vector_table(image, app_start, max_size)
        if size > max_size:
            raise OtaError(f"image is {size} B, node app area is {max_size} B")

        self.log(f"Image {size} B, CRC32 0x{crc:08X}, target 0x{app_start:08X}")
        self._expect_ok(self._cmd(OP_START, bytes(3) + struct.pack("<I", size), timeout=2.0), "START")

        written = 0
        attempts = 0
        t0 = time.monotonic()
        while written < size:
            block = image[written:written + BLOCK_BYTES]
            self._drain()
            for seq in range(0, len(block), DATA_BYTES_PER_FRAME):
                chunk = block[seq:seq + DATA_BYTES_PER_FRAME]
                self.t.send(id_data(self.node), bytes([seq // DATA_BYTES_PER_FRAME]) + chunk.ljust(7, b"\xFF"))

            rsp = self._wait(OP_DATA, 2.0)
            if rsp is not None and rsp.status == ST_OK and rsp.value == written + len(block):
                written = rsp.value
                attempts = 0
                rate = written / max(time.monotonic() - t0, 1e-6) / 1024
                self.log(f"\r  {written:>8}/{size} B  {100 * written // size:3d}%  {rate:5.1f} KB/s", end="")
                continue

            if rsp is not None and rsp.status not in (ST_OK, ST_ERR_SEQ):
                raise OtaError(f"block at {written}: {status_name(rsp.status)}")

            attempts += 1
            if attempts > retries:
                raise OtaError(f"block at {written}: giving up after {retries} retries")
            # Resync: STATUS tells us where the node is and drops its partial block.
            st = self._expect_ok(self._cmd(OP_STATUS, timeout=1.0), "STATUS")
            if st.value % BLOCK_BYTES != 0 and st.value != size:
                raise OtaError(f"node reports misaligned progress {st.value}")
            written = st.value
        self.log("")

        self._expect_ok(self._cmd(OP_END, bytes(3) + struct.pack("<I", crc), timeout=5.0), "END")
        dt = time.monotonic() - t0
        self.log(f"Verified and committed in {dt:.1f} s ({size / dt / 1024:.1f} KB/s)")

    def reset(self, wait_app: float = 3.0) -> bool:
        self._expect_ok(self._cmd(OP_RESET), "RESET")
        # Stay quiet through the bootloader's post-reset listen window: a PING
        # inside it means "host wants me" and would keep the node in the bootloader.
        self._sleep(BOOT_LISTEN_S + BOOT_SETTLE_S)
        deadline = time.monotonic() + wait_app
        while time.monotonic() < deadline:
            if self.ping(timeout=0.1) == MODE_APP:
                return True
        return False


def check_vector_table(image: bytes, app_start: int, max_size: int) -> None:
    if len(image) < 8:
        raise OtaError("image too small")
    sp, reset = struct.unpack_from("<II", image, 0)
    if not (0x20000000 <= sp <= 0x20100000):
        raise OtaError(f"initial SP 0x{sp:08X} is not in RAM: is this a raw .bin linked for the app slot?")
    if not (app_start <= reset < app_start + max_size) or not (reset & 1):
        raise OtaError(f"reset vector 0x{reset:08X} is outside the app area at 0x{app_start:08X}: "
                       "image linked for the wrong address (bootloader-less build?)")


# ── CLI ───────────────────────────────────────────────────────────────────────

def parse_node(text: str) -> int:
    key = text.upper()
    if key in NODES:
        return NODES[key]
    try:
        return int(text, 0)
    except ValueError:
        raise argparse.ArgumentTypeError(f"unknown node '{text}', use one of {', '.join(NODES)}")


def cmd_scan(transport: Transport, _args) -> int:
    for name, node in NODES.items():
        mode = OtaClient(transport, node).ping(timeout=0.1)
        if mode is not None:
            print(f"  {name:6s} (0x{node:02X})  {'bootloader' if mode == MODE_BOOTLOADER else 'application'}")
    return 0


def cmd_info(transport: Transport, args) -> int:
    client = OtaClient(transport, args.node)
    mode = client.ping()
    if mode is None:
        raise OtaError("no answer")
    if mode == MODE_APP:
        print("Node is running its application (OTA hook present). Use 'flash' or 'enter' for bootloader info.")
        return 0
    ver = client.info(INFO_BL_VERSION)
    flags = client.info(INFO_FLAGS)
    print(f"  bootloader   v{ver >> 16}.{ver & 0xFFFF}")
    print(f"  app area     0x{client.info(INFO_APP_START):08X}, {client.info(INFO_APP_MAX_SIZE)} B")
    print(f"  committed    {client.info(INFO_APP_SIZE)} B, CRC 0x{client.info(INFO_APP_CRC):08X}")
    print(f"  flags        vector_ok={bool(flags & FLAG_APP_VECTOR_OK)} committed={bool(flags & FLAG_COMMITTED)} "
          f"interrupted={bool(flags & FLAG_UPDATE_INTERRUPTED)}")
    return 0


def cmd_enter(transport: Transport, args) -> int:
    OtaClient(transport, args.node).enter_bootloader(catch=args.catch)
    print("Node is in bootloader mode.")
    return 0


def cmd_flash(transport: Transport, args) -> int:
    image = open(args.bin, "rb").read()
    client = OtaClient(transport, args.node)
    client.enter_bootloader(catch=args.catch)
    client.flash(image)
    if args.no_reset:
        return 0
    print("Starting application...")
    print("Application is up." if client.reset() else "Reset sent, but the app did not answer PING yet.")
    return 0


def cmd_reset(transport: Transport, args) -> int:
    client = OtaClient(transport, args.node)
    mode = client.ping()
    if mode is None:
        raise OtaError("no answer")
    if mode == MODE_APP:
        # The bootloader starts a valid app by itself after 30 s without traffic.
        print("Already running the application, nothing to reset.")
        return 0
    ok = client.reset()
    print("Application is up." if ok else "Reset sent, app did not answer PING.")
    return 0


def main(argv=None) -> int:
    p = argparse.ArgumentParser(description="ART TU OTA-over-CAN flasher")
    p.add_argument("-i", "--interface", default="pcan", help="python-can interface (pcan, gs_usb, slcan, socketcan, ...)")
    p.add_argument("-c", "--channel", default="PCAN_USBBUS1",
                   help="python-can channel; 'auto' with -i slcan finds a CANable's COM port")
    p.add_argument("-b", "--bitrate", type=int, default=500000)
    sub = p.add_subparsers(dest="cmd", required=True)

    sub.add_parser("scan", help="list nodes answering OTA PING")
    for name in ("info", "enter", "reset"):
        sp = sub.add_parser(name)
        sp.add_argument("--node", type=parse_node, required=True)
        if name == "enter":
            sp.add_argument("--catch", action="store_true", help="wait for a power-cycle to catch the bootloader")
    sp = sub.add_parser("flash")
    sp.add_argument("--node", type=parse_node, required=True)
    sp.add_argument("--bin", required=True, help="raw binary linked for the app slot")
    sp.add_argument("--catch", action="store_true", help="wait for a power-cycle to catch the bootloader")
    sp.add_argument("--no-reset", action="store_true", help="stay in the bootloader after flashing")

    args = p.parse_args(argv)
    try:
        if args.channel.lower() == "auto":
            if args.interface != "slcan":
                raise OtaError("-c auto only works with -i slcan")
            args.channel = find_slcan_port()
            print(f"Using {args.channel} (auto-detected CANable)")
        transport = PythonCanTransport(args.interface, args.channel, args.bitrate)
    except OtaError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1
    except Exception as exc:  # adapter missing, port busy, driver errors
        print(f"ERROR: cannot open {args.interface} on {args.channel}: {exc}", file=sys.stderr)
        return 1
    handlers = {"scan": cmd_scan, "info": cmd_info, "enter": cmd_enter, "flash": cmd_flash, "reset": cmd_reset}
    try:
        return handlers[args.cmd](transport, args)
    except OtaError as exc:
        print(f"\nERROR: {exc}", file=sys.stderr)
        return 1
    finally:
        transport.close()


if __name__ == "__main__":
    sys.exit(main())
