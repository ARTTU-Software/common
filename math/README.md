# common/math

Math utilities and control primitives used across the stack.

Contents
- Filters (low-pass, complementary)
- Units and conversions
- Controllers (PID helpers, feedforward)

Notes
- Keep functions pure to simplify unit testing ([tests/](tests/)).
- Expose minimal interface. Keep things simple, especially if performance is in consideration.