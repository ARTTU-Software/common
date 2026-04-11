"""Generate CAN definition headers from a DBC file.

This script reads a DBC, extracts message and signal metadata, and writes two
headers:

- can_signal_defs.h: per-message signal structs plus metadata macros
- can_board_defs.h: per-board structs that aggregate the transmit messages

The generated headers are intentionally simple so they can be used from C code
without adding a runtime dependency on a CAN database parser.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
import argparse
import math
import re


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_DBC_PATH = SCRIPT_DIR / "MAIN_DBC.dbc"
DEFAULT_SIGNAL_HEADER = SCRIPT_DIR / "drivers" / "can" / "can_signal_defs.h"
DEFAULT_BOARD_HEADER = SCRIPT_DIR / "drivers" / "can" / "can_board_defs.h"


_RE_NODE_LINE = re.compile(r"^BU_:\s*(?P<nodes>.*)$")
_RE_MESSAGE_LINE = re.compile(
	r"^BO_\s+(?P<frame_id>\d+)\s+(?P<name>[^:]+):\s+(?P<dlc>\d+)\s+(?P<sender>\S+)\s*$"
)
_RE_SIGNAL_LINE = re.compile(
	r'^SG_\s+(?P<raw_name>.+?)\s*:\s*'
	r'(?P<start_bit>\d+)\|(?P<length>\d+)@(?P<byte_order>[01])(?P<sign>[+-])\s*'
	r'\((?P<factor>[-+0-9.eE]+)\s*,\s*(?P<offset>[-+0-9.eE]+)\)\s*'
	r'\[(?P<minimum>[-+0-9.eE]+)\|(?P<maximum>[-+0-9.eE]+)\]\s*'
	r'"(?P<unit>[^"]*)"\s*(?P<receivers>.*)$'
)
_RE_CYCLE_TIME = re.compile(r'^BA_\s+"GenMsgCycleTime"\s+BO_\s+(?P<frame_id>\d+)\s+(?P<value>\d+);\s*$')

_C_KEYWORDS = {
	"auto",
	"break",
	"case",
	"char",
	"const",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"enum",
	"extern",
	"float",
	"for",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"register",
	"restrict",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"struct",
	"switch",
	"typedef",
	"union",
	"unsigned",
	"void",
	"volatile",
	"while",
}


@dataclass
class SignalDef:
	name: str
	start_bit: int
	length: int
	byte_order: int
	sign: str
	factor: float
	offset: float
	minimum: float
	maximum: float
	unit: str
	receivers: list[str] = field(default_factory=list)
	mux: str | None = None
	field_name: str = ""


@dataclass
class MessageDef:
	frame_id: int
	name: str
	dlc: int
	sender: str
	signals: list[SignalDef] = field(default_factory=list)
	cycle_time_ms: int = 0


@dataclass
class DbcModel:
	nodes: list[str]
	messages: list[MessageDef]


def sanitize_identifier(value: str, *, upper: bool = False) -> str:
	candidate = re.sub(r"[^0-9A-Za-z_]", "_", value.strip())
	candidate = re.sub(r"_+", "_", candidate).strip("_")
	if not candidate:
		candidate = "unnamed"
	if candidate[0].isdigit():
		candidate = f"_{candidate}"
	if candidate.lower() in _C_KEYWORDS:
		candidate += "_"
	return candidate.upper() if upper else candidate


def split_signal_name(raw_name: str) -> tuple[str, str | None]:
	parts = raw_name.strip().rsplit(" ", 1)
	if len(parts) == 2 and (parts[1] == "M" or re.fullmatch(r"m\d+", parts[1])):
		return parts[0], parts[1]
	return raw_name.strip(), None


def parse_dbc(text: str) -> DbcModel:
	nodes: list[str] = []
	messages: list[MessageDef] = []
	cycle_times: dict[int, int] = {}

	current_message: MessageDef | None = None

	for raw_line in text.splitlines():
		line = raw_line.strip()
		if not line:
			continue

		node_match = _RE_NODE_LINE.match(line)
		if node_match:
			nodes = node_match.group("nodes").split()
			continue

		cycle_match = _RE_CYCLE_TIME.match(line)
		if cycle_match:
			cycle_times[int(cycle_match.group("frame_id"))] = int(cycle_match.group("value"))
			continue

		message_match = _RE_MESSAGE_LINE.match(line)
		if message_match:
			current_message = MessageDef(
				frame_id=int(message_match.group("frame_id")),
				name=message_match.group("name").strip(),
				dlc=int(message_match.group("dlc")),
				sender=message_match.group("sender"),
				cycle_time_ms=cycle_times.get(int(message_match.group("frame_id")), 0),
			)
			messages.append(current_message)
			continue

		signal_match = _RE_SIGNAL_LINE.match(line)
		if signal_match and current_message is not None:
			signal_name, mux = split_signal_name(signal_match.group("raw_name"))
			receivers = [receiver for receiver in signal_match.group("receivers").split() if receiver and receiver != "Vector__XXX"]
			current_message.signals.append(
				SignalDef(
					name=signal_name,
					start_bit=int(signal_match.group("start_bit")),
					length=int(signal_match.group("length")),
					byte_order=int(signal_match.group("byte_order")),
					sign=signal_match.group("sign"),
					factor=float(signal_match.group("factor")),
					offset=float(signal_match.group("offset")),
					minimum=float(signal_match.group("minimum")),
					maximum=float(signal_match.group("maximum")),
					unit=signal_match.group("unit"),
					receivers=receivers,
					mux=mux,
				)
			)
			continue

	if not nodes:
		nodes = sorted({message.sender for message in messages})

	return DbcModel(nodes=nodes, messages=messages)


def classify_integer_type(signal: SignalDef) -> str | None:
	if not math.isclose(signal.factor, 1.0) or not math.isclose(signal.offset, 0.0):
		return None

	if signal.length == 1:
		return "int8_t" if signal.sign == "-" else "uint8_t"

	if signal.length <= 8:
		return "int8_t" if signal.sign == "-" else "uint8_t"
	if signal.length <= 16:
		return "int16_t" if signal.sign == "-" else "uint16_t"
	if signal.length <= 32:
		return "int32_t" if signal.sign == "-" else "uint32_t"
	return None


def signal_value_type(signal: SignalDef) -> str:
	integer_type = classify_integer_type(signal)
	if integer_type is not None:
		return integer_type
	return "float"


def unique_field_name(base_name: str, existing: set[str]) -> str:
	candidate = sanitize_identifier(base_name)
	if candidate not in existing:
		existing.add(candidate)
		return candidate

	suffix = 2
	while f"{candidate}_{suffix}" in existing:
		suffix += 1
	unique_name = f"{candidate}_{suffix}"
	existing.add(unique_name)
	return unique_name


def generate_signal_header(model: DbcModel, source_path: Path) -> str:
	lines: list[str] = []
	lines.append("#ifndef INC_CAN_SIGNAL_DEFS_H")
	lines.append("#define INC_CAN_SIGNAL_DEFS_H")
	lines.append("")
	lines.append("#include <stdint.h>")
	lines.append("")
	lines.append(f"/* Generated from {source_path.name}. Do not edit by hand. */")
	lines.append("")

	for message in model.messages:
		message_tag = sanitize_identifier(message.name, upper=True)
		lines.append(f"#define CAN_MSG_{message_tag}_ID {message.frame_id}U")
		lines.append(f"#define CAN_MSG_{message_tag}_DLC {message.dlc}U")
		lines.append(f"#define CAN_MSG_{message_tag}_CYCLE_TIME_MS {message.cycle_time_ms}U")
		lines.append("")

		used_field_names: set[str] = set()
		for signal in message.signals:
			signal_tag = sanitize_identifier(signal.name, upper=True)
			prefix = f"CAN_MSG_{message_tag}_SIG_{signal_tag}"
			lines.append(f"#define {prefix}_FACTOR {repr(signal.factor)}f")
			lines.append("")

			signal.field_name = unique_field_name(signal.name, used_field_names)

		struct_name = f"can_msg_{sanitize_identifier(message.name)}_t"
		lines.append(f"typedef struct {{")
		for signal in message.signals:
			value_type = signal_value_type(signal)
			comment_bits = [
				f"start_bit={signal.start_bit}",
				f"length={signal.length}",
				f"factor={signal.factor}",
				f"offset={signal.offset}",
			]
			if signal.mux is not None:
				comment_bits.append(f"mux={signal.mux}")
			if signal.unit:
				comment_bits.append(f'unit="{signal.unit}"')
			comment = "; ".join(comment_bits)
			lines.append(f"    {value_type} {signal.field_name}; /* {comment} */")
		lines.append(f"}} {struct_name};")
		lines.append("")

	lines.append("#endif /* INC_CAN_SIGNAL_DEFS_H */")
	lines.append("")
	return "\n".join(lines)


def generate_board_header(model: DbcModel, source_path: Path) -> str:
	lines: list[str] = []
	lines.append("#ifndef INC_CAN_BOARD_DEFS_H")
	lines.append("#define INC_CAN_BOARD_DEFS_H")
	lines.append("")
	lines.append('#include "can_signal_defs.h"')
	lines.append("")
	lines.append(f"/* Generated from {source_path.name}. Do not edit by hand. */")
	lines.append("")

	messages_by_sender: dict[str, list[MessageDef]] = {}
	for message in model.messages:
		messages_by_sender.setdefault(message.sender, []).append(message)

	for node in model.nodes:
		tx_messages = messages_by_sender.get(node, [])
		board_type = f"can_board_{sanitize_identifier(node)}_t"
		lines.append(f"typedef struct {{")
		if tx_messages:
			for message in tx_messages:
				message_type = f"can_msg_{sanitize_identifier(message.name)}_t"
				field_name = sanitize_identifier(message.name)
				lines.append(f"    {message_type} {field_name};")
		else:
			lines.append("    uint8_t reserved;")
		lines.append(f"}} {board_type};")
		lines.append("")

	lines.append("#endif /* INC_CAN_BOARD_DEFS_H */")
	lines.append("")
	return "\n".join(lines)


def write_if_changed(path: Path, content: str) -> bool:
	path.parent.mkdir(parents=True, exist_ok=True)
	if path.exists() and path.read_text(encoding="utf-8") == content:
		return False
	path.write_text(content, encoding="utf-8", newline="\n")
	return True


def parse_args() -> argparse.Namespace:
	parser = argparse.ArgumentParser(description="Generate CAN header files from a DBC file.")
	parser.add_argument("--dbc", type=Path, default=DEFAULT_DBC_PATH, help="Path to the input DBC file")
	parser.add_argument(
		"--signal-header",
		type=Path,
		default=DEFAULT_SIGNAL_HEADER,
		help="Path to the generated signal header",
	)
	parser.add_argument(
		"--board-header",
		type=Path,
		default=DEFAULT_BOARD_HEADER,
		help="Path to the generated board header",
	)
	return parser.parse_args()


def main() -> int:
	args = parse_args()

	if not args.dbc.exists():
		raise FileNotFoundError(f"DBC file not found: {args.dbc}")

	model = parse_dbc(args.dbc.read_text(encoding="utf-8", errors="ignore"))

	signal_header = generate_signal_header(model, args.dbc)
	board_header = generate_board_header(model, args.dbc)

	signal_written = write_if_changed(args.signal_header, signal_header)
	board_written = write_if_changed(args.board_header, board_header)

	print(f"Parsed {len(model.messages)} messages from {args.dbc}")
	print(f"Wrote {args.signal_header} ({'updated' if signal_written else 'unchanged'})")
	print(f"Wrote {args.board_header} ({'updated' if board_written else 'unchanged'})")

	return 0


if __name__ == "__main__":
	raise SystemExit(main())
