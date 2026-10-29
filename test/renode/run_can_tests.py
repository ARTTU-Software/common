"""Build portable CAN scenarios for ARM and run Renode's native Robot runner."""

import argparse
import difflib
from datetime import datetime, timezone
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET


def execute(command, log, cwd, expect_failure=False):
    with log.open("w", encoding="utf-8") as output:
        result = subprocess.run(command, cwd=cwd, stdout=output, stderr=subprocess.STDOUT,
                                env={**os.environ, "PYTHONIOENCODING": "utf-8"}, check=False)
    if expect_failure:
        if result.returncode == 0:
            raise RuntimeError(f"Intentional assertion failure returned success: {log}")
    elif result.returncode:
        print(log.read_text(encoding="utf-8", errors="replace")[-8000:])
        raise RuntimeError(f"Command failed ({result.returncode}); see {log}")


def validate_report(report, expected_count, expected="PASS"):
    document = ET.parse(report)
    tests = document.findall(".//test")
    statuses = [test.find("status").attrib["status"] for test in tests]
    errors = document.findall("./errors/msg")
    if len(tests) != expected_count or any(status != expected for status in statuses) or errors:
        raise RuntimeError(f"Incomplete/failed Robot run: {report}")
    return len(tests)


def prepare_runner(runner, output):
    # Renode 1.17's Windows drive-letter path plus ':argument' is ambiguous to
    # Robot 6.1. Repair only that import in an owned copy, preserving assertions.
    provider = runner.parent / "robot_tests_provider.py"
    before = provider.read_text(encoding="utf-8")
    old = "os.path.join(this_path, f'retry_and_timeout_listener.py:{options.retry_count}')"
    if os.name != "nt" or old not in before:
        return runner
    if before.count(old) != 1:
        raise RuntimeError("Ambiguous Windows listener repair; select a compatible --renode-tests")
    staged = output / "native-runner"
    staged.mkdir()
    for entry in runner.parent.iterdir():
        if entry.is_file():
            shutil.copy2(entry, staged / entry.name)
    after = before.replace(old, "f'retry_and_timeout_listener:{options.retry_count}'")
    (staged / provider.name).write_text(after, encoding="utf-8")
    patch = difflib.unified_diff(before.splitlines(True), after.splitlines(True),
                                 fromfile=str(provider), tofile=str(staged / provider.name))
    (output / "windows-listener.patch").write_text("".join(patch), encoding="utf-8")
    return staged / runner.name


def main():
    root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default=shutil.which("arm-none-eabi-gcc"))
    parser.add_argument("--renode", default=shutil.which("renode"))
    parser.add_argument("--renode-tests", type=Path)
    parser.add_argument("--cpu", action="append", choices=("cortex-m4", "cortex-m7", "cortex-m33"))
    parser.add_argument("--results-dir", type=Path)
    parser.add_argument("--check-failure-path", action="store_true",
                        help="First prove that intentionally wrong assertions return nonzero")
    args = parser.parse_args()
    if not args.compiler or not args.renode:
        parser.error("Provide --compiler and --renode, or put both executables on PATH")
    compiler = Path(args.compiler).resolve()
    renode = Path(args.renode).resolve()
    runner = (args.renode_tests or renode.parent / "tests") / "run_tests.py"
    if not compiler.is_file() or not renode.is_file() or not runner.is_file():
        parser.error("Compiler, Renode executable and installed tests/run_tests.py must exist")
    cpus = args.cpu or ["cortex-m4", "cortex-m7", "cortex-m33"]
    if len(cpus) != len(set(cpus)):
        parser.error("Duplicate CPU selection")
    stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S%fZ")
    output = (args.results_dir or root / "build/renode-can" / stamp).resolve()
    output.mkdir(parents=True, exist_ok=False)
    installed_runner = runner
    runner = prepare_runner(runner, output)
    fixture = root / "test/renode"
    count_header = (root / "test/fixtures/can_driver_cases.h").read_text()
    expected_count = int(re.search(r"^#define CAN_DRIVER_CASE_COUNT (\d+)U$",
                                   count_header, re.MULTILINE).group(1))
    sources = [fixture / "can_driver_probe.c", root / "test/fixtures/can_driver_cases.c",
               root / "drivers/can/can_driver_core.c"]
    summary = {"compiler": str(compiler), "renode": str(renode), "cpus": {},
               "case_count": expected_count,
               "installed_runner_sha256": hashlib.sha256(installed_runner.read_bytes()).hexdigest(),
               "used_runner": str(runner)}
    for cpu in cpus:
        directory = output / cpu
        directory.mkdir()
        elf = directory / "can_driver.elf"
        command = [str(compiler), f"-mcpu={cpu}", "-mthumb", "-mfloat-abi=soft", "-std=c11",
                   "-O2", "-g3", "-Wall", "-Wextra", "-Werror", "-pedantic",
                   "-ffunction-sections", "-fdata-sections", "-nostartfiles",
                   "--specs=nosys.specs", "-Wl,-e,main", "-Wl,-Ttext=0x08000000",
                   "-Wl,-Tdata=0x20000000", "-Wl,-Tbss=0x20001000", "-Wl,--gc-sections",
                   "-I", str(root / "drivers/can"), "-I", str(root / "test/fixtures"),
                   *(str(source) for source in sources), "-o", str(elf)]
        execute(command, directory / "build.log", root)
        platform = directory / "can_driver.repl"
        platform.write_text((fixture / "can_driver.repl").read_text().replace("cortex-m4", cpu))
        command = [sys.executable, str(runner), str(fixture / "can_driver.robot"),
                   "--robot-framework-remote-server-full-directory", str(renode.parent),
                   "--robot-framework-remote-server-name", renode.name,
                   "--variable", f"ELF:{elf.as_posix()}",
                   "--variable", f"PLATFORM:{platform.as_posix()}",
                   "--test-timeout", "1 minute", "--save-logs", "always",
                   "--results-dir", str(directory / "results")]
        css = runner.parent / "robot.css"
        if css.is_file():
            command.extend(["--css-file", str(css)])
        if args.check_failure_path and cpu == cpus[0]:
            negative = command.copy()
            negative[negative.index("--results-dir") + 1] = str(directory / "intentional-failure")
            negative.extend(["--variable", "EXPECTED_RESULT:1"])
            execute(negative, directory / "intentional-failure.log", root, expect_failure=True)
            validate_report(directory / "intentional-failure/robot_output.xml", expected_count, expected="FAIL")
            summary["intentional_failure_check"] = f"nonzero, {expected_count} assertion failures, no listener errors"
        execute(command, directory / "runner.log", root)
        report = directory / "results/robot_output.xml"
        # Empty, partial, skipped or infrastructure-broken runs must fail closed.
        count = validate_report(report, expected_count)
        summary["cpus"][cpu] = {"passed": count, "elf_sha256": hashlib.sha256(elf.read_bytes()).hexdigest()}
        print(f"{cpu}: {count} passed", flush=True)
    (output / "summary.json").write_text(json.dumps(summary, indent=2), encoding="utf-8")
    print(f"Results: {output}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (RuntimeError, OSError, ET.ParseError) as error:
        print(f"CAN Renode test failure: {error}", file=sys.stderr)
        sys.exit(1)
