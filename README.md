# common/

Shared libraries and helpers used across boards and software stacks.

### Subfolders
- [common/drivers/README.md](drivers/README.md)
- [common/math/README.md](math/README.md)
- [common/ota/README.md](ota/README.md): firmware update over CAN (bootloader + app hook, STM32 G4/H5/L5)
- [common/utils/README.md](utils/README.md)

### Guidelines
- Put platform-agnostic algorithms (filters, control math) in common.
- Drivers should ONLY expose small, testable interfaces used by board code. Keep code uncluttered.