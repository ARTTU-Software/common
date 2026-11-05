"""
End-to-end test: real host client (ota_flash.py) against the real bootloader
core (ota/ota_boot.c) and app hook (ota_app.c), compiled with the host
gcc into a simulated node. No CAN hardware or python-can needed.

    python -m unittest discover -s ota/tools/test -v      (from the common repo root)
"""

import ctypes
import os
import random
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
OTA = os.path.abspath(os.path.join(HERE, "..", ".."))  # common/ota
sys.path.insert(0, os.path.dirname(HERE))

import ota_flash as of  # noqa: E402


def build_sim() -> ctypes.CDLL:
    gcc = shutil.which("gcc")
    if gcc is None:
        raise unittest.SkipTest("host gcc not found")
    out_dir = tempfile.mkdtemp(prefix="ota_sim_")
    lib = os.path.join(out_dir, "ota_sim.dll" if os.name == "nt" else "ota_sim.so")
    cmd = [gcc, "-shared", "-O1", "-g", "-Wall", "-Wextra", "-Werror", "-std=c11",
           "-I", OTA, os.path.join(HERE, "sim_node.c"),
           os.path.join(OTA, "ota_boot.c"), os.path.join(OTA, "ota_crc32.c"), os.path.join(OTA, "ota_app.c"),
           "-o", lib]
    if os.name == "nt":
        cmd += ["-static-libgcc", "-static"]
    else:
        cmd += ["-fPIC"]
    # MSYS2 gcc needs its own bin dir first on PATH, or cc1 fails silently.
    env = dict(os.environ, PATH=os.path.dirname(gcc) + os.pathsep + os.environ.get("PATH", ""))
    subprocess.run(cmd, check=True, env=env)
    dll = ctypes.CDLL(lib)
    dll.sim_host_recv.argtypes = [ctypes.POINTER(ctypes.c_uint32), ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint8)]
    dll.sim_host_send.argtypes = [ctypes.c_uint32, ctypes.c_char_p, ctypes.c_uint8]
    dll.sim_flags.restype = ctypes.c_uint32
    dll.sim_app_start.restype = ctypes.c_uint32
    dll.sim_app_size.restype = ctypes.c_uint32
    dll.sim_read.argtypes = [ctypes.c_uint32, ctypes.c_char_p, ctypes.c_uint32]
    return dll


SIM = None


class SimTransport:
    """Transport for OtaClient. Optional hook can drop/alter host->node frames."""

    def __init__(self, dll, tx_hook=None):
        self.dll = dll
        self.tx_hook = tx_hook
        self.sent = 0

    def send(self, can_id, data):
        self.sent += 1
        if self.tx_hook is not None:
            data = self.tx_hook(self.sent, can_id, bytes(data))
            if data is None:
                return  # frame lost
        self.dll.sim_host_send(can_id, bytes(data), len(data))

    def recv(self, timeout):
        cid = ctypes.c_uint32()
        ln = ctypes.c_uint8()
        buf = ctypes.create_string_buffer(8)
        if self.dll.sim_host_recv(ctypes.byref(cid), buf, ctypes.byref(ln)):
            return cid.value, buf.raw[:ln.value]
        self.dll.sim_advance(int(timeout * 1000))
        if self.dll.sim_host_recv(ctypes.byref(cid), buf, ctypes.byref(ln)):
            return cid.value, buf.raw[:ln.value]
        return None

    def sleep(self, seconds):
        self.dll.sim_advance(int(seconds * 1000))


def make_image(size, seed=1):
    rnd = random.Random(seed)
    body = bytearray(rnd.getrandbits(8) for _ in range(size))
    app_start = SIM.sim_app_start()
    struct.pack_into("<II", body, 0, 0x200A0000, app_start + 0x1C1)  # SP, Reset_Handler|1
    return bytes(body)


def quiet(*_a, **_k):
    pass


class OtaSimTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        global SIM
        SIM = build_sim()

    def setUp(self):
        SIM.sim_power_on_blank()

    def client(self, hook=None):
        return of.OtaClient(SimTransport(SIM, hook), of.NODES["ECU"], log=quiet)

    def flash_ok(self, image, hook=None):
        c = self.client(hook)
        c.enter_bootloader(timeout=0.5)
        c.flash(image)
        return c

    def read_app(self, n):
        buf = ctypes.create_string_buffer(n)
        SIM.sim_read(SIM.sim_app_start(), buf, n)
        return buf.raw

    # ── tests ────────────────────────────────────────────────────

    def test_blank_node_stays_in_bootloader(self):
        self.assertEqual(SIM.sim_mode(), of.MODE_BOOTLOADER)
        self.assertEqual(self.client().ping(), of.MODE_BOOTLOADER)

    def test_full_flash_then_app_runs(self):
        image = make_image(100_003)  # not a multiple of 7, 16 or the block size
        c = self.flash_ok(image)
        self.assertEqual(self.read_app(len(image)), image)
        self.assertTrue(c.reset(wait_app=0.5))
        self.assertEqual(SIM.sim_mode(), of.MODE_APP)

    def test_enter_from_app_and_reflash(self):
        c = self.flash_ok(make_image(5000, seed=2))
        self.assertTrue(c.reset(wait_app=0.5))
        self.assertEqual(c.ping(), of.MODE_APP)

        image = make_image(20_000, seed=3)
        c.enter_bootloader(timeout=0.5)  # 0x4F0 -> app resets into bootloader
        self.assertEqual(SIM.sim_mode(), of.MODE_BOOTLOADER)
        c.flash(image)
        self.assertEqual(c.info(of.INFO_APP_SIZE), len(image))
        self.assertEqual(c.info(of.INFO_APP_CRC), zlib.crc32(image) & 0xFFFFFFFF)

    def test_enter_refused_when_not_safe(self):
        c = self.flash_ok(make_image(4000))
        c.reset(wait_app=0.5)
        SIM.sim_set_app_safe(0)
        with self.assertRaisesRegex(of.OtaError, "ERR_NOT_SAFE"):
            c.enter_bootloader(timeout=0.5)
        self.assertEqual(SIM.sim_mode(), of.MODE_APP)

    def test_wrong_key_ignored(self):
        c = self.flash_ok(make_image(4000))
        c.reset(wait_app=0.5)
        SIM.sim_host_send(of.ID_ENTER_REQUEST, bytes([1]) + b"XYZ" + bytes(4), 8)
        SIM.sim_host_send(of.ID_ENTER_REQUEST, bytes([2]) + b"OTA" + bytes(4), 8)  # other node
        self.assertEqual(SIM.sim_mode(), of.MODE_APP)

    def test_lost_data_frames_recover(self):
        image = make_image(30_000, seed=4)
        lost = {150, 151, 400, 777}

        def drop(n, can_id, data):
            return None if (can_id == of.id_data(1) and n in lost) else data

        self.flash_ok(image, drop)
        self.assertEqual(self.read_app(len(image)), image)

    def test_last_frame_of_block_lost_recovers_via_status(self):
        image = make_image(3 * of.BLOCK_BYTES, seed=5)
        state = {"data": 0}

        def drop(n, can_id, data):
            if can_id == of.id_data(1):
                state["data"] += 1
                if state["data"] == of.BLOCK_FRAMES * 2:  # last frame of block 2: node never ACKs
                    return None
            return data

        self.flash_ok(image, drop)
        self.assertEqual(self.read_app(len(image)), image)

    def test_corrupted_payload_fails_crc(self):
        image = make_image(10_000, seed=6)

        def corrupt(n, can_id, data):
            if can_id == of.id_data(1) and n == 300:
                return data[:3] + bytes([data[3] ^ 0x55]) + data[4:]
            return data

        with self.assertRaisesRegex(of.OtaError, "ERR_CRC"):
            self.flash_ok(image, corrupt)
        self.assertFalse(SIM.sim_app_startable())

    def test_interrupted_update_keeps_bootloader(self):
        old = make_image(8000, seed=7)
        c = self.flash_ok(old)
        c.reset(wait_app=0.5)
        self.assertEqual(SIM.sim_mode(), of.MODE_APP)

        # New update dies halfway (host stops, then power cycle).
        c.enter_bootloader(timeout=0.5)
        cut = {"n": 0}

        def die(n, can_id, data):
            if can_id == of.id_data(1):
                cut["n"] += 1
                if cut["n"] > 200:
                    raise ConnectionError("power lost")
            return data

        with self.assertRaises(ConnectionError):
            self.client(die).flash(make_image(50_000, seed=8))
        SIM.sim_reset()  # power cycle
        self.assertEqual(SIM.sim_mode(), of.MODE_BOOTLOADER)
        self.assertTrue(SIM.sim_flags() & of.FLAG_UPDATE_INTERRUPTED)

        image = make_image(50_000, seed=8)
        self.flash_ok(image)
        self.assertTrue(self.client().reset(wait_app=0.5))

    def test_oversize_rejected(self):
        with self.assertRaisesRegex(of.OtaError, "app area"):
            self.flash_ok(make_image(SIM.sim_app_size() + 16))

    def test_bad_vector_rejected_by_host(self):
        image = bytearray(make_image(2000))
        struct.pack_into("<I", image, 4, 0x08000101)  # reset handler in bootloader area
        with self.assertRaisesRegex(of.OtaError, "reset vector"):
            self.flash_ok(bytes(image))

    def test_flash_write_failure_reported(self):
        SIM.sim_fail_program_after(5)
        with self.assertRaisesRegex(of.OtaError, "ERR_WRITE"):
            self.flash_ok(make_image(20_000))

    def test_listen_window_catches_host(self):
        c = self.flash_ok(make_image(4000))
        self.assertTrue(c.reset(wait_app=0.5))
        SIM.sim_reset()  # power cycle with a valid app
        self.assertTrue(SIM.sim_listening())
        self.assertEqual(c.ping(timeout=0.01), of.MODE_BOOTLOADER)  # PING inside 50 ms window
        SIM.sim_advance(1000)
        self.assertEqual(SIM.sim_mode(), of.MODE_BOOTLOADER)  # host seen: stays

    def test_silent_listen_window_starts_app(self):
        c = self.flash_ok(make_image(4000))
        self.assertTrue(c.reset(wait_app=0.5))
        SIM.sim_reset()
        SIM.sim_advance(60)  # nobody talks during the window
        self.assertEqual(SIM.sim_mode(), of.MODE_APP)

    def test_idle_bootloader_jumps_to_valid_app(self):
        c = self.flash_ok(make_image(4000))
        self.assertEqual(SIM.sim_mode(), of.MODE_BOOTLOADER)
        SIM.sim_advance(31_000)  # host vanished after END, no RESET
        self.assertEqual(SIM.sim_mode(), of.MODE_APP)
        self.assertEqual(c.ping(), of.MODE_APP)


if __name__ == "__main__":
    unittest.main(verbosity=2)
