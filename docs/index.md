---
title: Common Library
description: Shared drivers, sensors, and tooling used across ARTTU firmware.
---

# Common Library

The common library is a shared C codebase used by multiple firmware boards. It provides small, testable driver building blocks (CAN, FSM, sensors, misc helpers) and repo-level tooling such as CAN DBC code generation. Board projects wire these modules to hardware-specific HAL handles and configuration.

## What lives here
- Drivers: CAN, FSM, sensors, and misc helpers.
- Tooling: DBC to C header generator and the CAN database.
- Tests: unit tests driven by Ceedling.

> [!NOTE]
> KFCore in common/math/KFCore is an external library and is out of scope.

## Generated CAN headers
- can_signal_defs.h and can_board_defs.h are generated from MAIN_DBC.dbc by dbc_to_c_builder.py.
- Do not edit the generated headers by hand.

## Related Pages
- [Integration and Tooling](/shared/common/integration)
- [Module Reference](/shared/common/modules/)
