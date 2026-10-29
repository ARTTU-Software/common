# CAN core regression in Renode

This fixture runs the actual common CAN C code as ARM firmware. It exercises
periodic demands, immediate status frames, hardware backpressure, STOP replacing
an unsent demand, independent buses, tick wrap, RX overflow and input validation.
The scenarios are shared with the host unit tests, with assertions owned
by the native Renode Robot runner.

Use an installed Renode distribution and a Python environment containing that
distribution's test requirements. Put `arm-none-eabi-gcc` and `renode` on PATH, or
provide their executable paths:

```text
python test/renode/run_can_tests.py --compiler /path/to/arm-none-eabi-gcc --renode /path/to/renode
```

The default runs Cortex-M4, Cortex-M7 and Cortex-M33, the CPU families relevant to
G4, H7 and H5 projects. Select one with `--cpu cortex-m4`. If the executable is a
launcher outside the installation directory, pass `--renode-tests /path/to/tests`.
On Windows, select `renode.exe` and run the script with the Python environment
that satisfies the installed Renode test requirements.

Each run creates a fresh ignored directory under `build/renode-can`. It retains
compiler output, ELF files, platform descriptions, Renode logs, Robot XML/HTML,
and a summary with ELF hashes. Empty, incomplete, skipped, failed or
infrastructure-broken results return nonzero.

Pass `--check-failure-path` to first run deliberately wrong assertions and verify
that the runner returns nonzero without listener errors. The normal run then
starts fresh and must pass. On affected Windows Renode 1.17 installations, the
script copies the native runner into the results directory and repairs only its
argument-bearing listener import. It retains the exact patch, uses the same
listener implementation, and leaves the installation unchanged.

The fixture uses mapped flash/RAM and a CPU with an interrupt controller. It
enters the test main function directly with an explicit stack pointer. It does
not modify or depend on vendor startup code or board linker scripts. GCC's
default linker is given fixed test section addresses by command-line options.
Virtual execution is bounded to 10 ms per case, and Robot verifies both firmware
completion/progress and the scenario result.

The submit callback is a deterministic hardware-capacity fixture. These tests
prove portable logic on the selected ARM cores, not physical FDCAN behavior,
CAN arbitration, IRQ timing, transceivers or DMA. The separate host adapter tests
exercise header/DLC conversion and bounded FIFO handling. Real board peripheral
configuration remains an integration responsibility.
