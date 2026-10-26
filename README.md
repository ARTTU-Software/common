# common/

Shared libraries and helpers used across boards and software stacks.

Edit CAN definitions in SharePoint, then use **Actions → Sync CAN database** for
an immediate DBC download and automatic patch release. See the
[CAN database sync guide](docs/can-database-sync.md) for setup and validation.

### Subfolders
- [common/drivers/README.md](drivers/README.md)
- [common/math/README.md](math/README.md)
- [common/utils/README.md](utils/README.md)

### Guidelines
- Put platform-agnostic algorithms (filters, control math) in common.
- Drivers should ONLY expose small, testable interfaces used by board code. Keep code uncluttered.
