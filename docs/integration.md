---
title: Common Library - Integration
description: Build, test, and tooling notes for the common library.
---

# Integration and Tooling

## Build and include paths
The common repo exposes headers from all subfolders and can be used as either a header-only include set or a static library, depending on CMake options.

- BUILD_COMMON_LIBRARY: builds common_static from all .c sources (tests excluded).
- BUILD_COMMON_TESTS: enables CTest hooks (test logic is driven by Ceedling).
- Header discovery uses CMake globbing; rerun CMake when adding new files.

## Ceedling tests
Ceedling is configured in project.yml and targets drivers and math sources.

- Default task: test:all.
- Tests live under common/test.

Example command:
```sh
ceedling test:all
```

## CAN DBC code generation
dbc_to_c_builder.py reads MAIN_DBC.dbc and generates the CAN header files used by firmware:

- drivers/can/can_signal_defs.h
- drivers/can/can_board_defs.h

Example command (defaults to MAIN_DBC.dbc):
```sh
python dbc_to_c_builder.py
```

To point at a custom DBC file:
```sh
python dbc_to_c_builder.py --dbc path/to/your.dbc
```

## Interrupt-context notes
- CAN_driver_rx_callback is designed for ISR context. Keep the callback short and defer processing to the main loop.
- flow_sensor_EXTI_handler is designed for EXTI ISR usage and only increments the pulse counter.
- CAN_process_tx_queue and CAN_send_frames should run in the main loop or a scheduler, not inside an ISR.

## Out of scope
KFCore under common/math/KFCore is a separate library and is not documented here.
