# common/drivers

Hardware abstraction layer and peripheral drivers (CAN, SPI, UART, ADC, PWM). For example, SPI drivers used for communication on ADCs that are on multiple boards.

### Testing
- Driver unit tests should run using mocks in [tests/](tests/).

### Guidelines
- Drivers expose deterministic, minimal APIs.
- Do not place policy/control logic here — only I/O and conversion.