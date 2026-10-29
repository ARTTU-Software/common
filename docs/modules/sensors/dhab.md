---
title: Common - LEM DHAB
description: Model-aware conversion and selection of paired LEM current channels.
---

# LEM DHAB current conversion

This driver turns two raw ADC readings into a current measurement. It keeps
both channel voltages and current estimates available for diagnosis, prefers
the sensitive range at low currents and uses the wide range when necessary.
It supports the S/118 (30/350 A) and S/134 (50/200 A) transfer characteristics.
Select the model from the fitted sensor's label.

Initialize a caller-owned `dhab_t` with `dhab_init()`. Configuration supplies the
ADC's maximum code after oversampling shifts and a separate divider gain, series
diode drop and optional zero correction for each channel. Call `dhab_update()`
from one task with paired raw codes, actual sensor supply, ADC reference and
monotonic milliseconds. The instance is not internally synchronized.

The driver reconstructs the sensor output, normalizes it to a 5 V supply and
applies the model's datasheet sensitivity. The S/118 sensitivities are exactly
the published 0.0667 and 0.0057 V/A, rather than values inferred from full range.
Supply scaling applies to both sensitivity and zero voltage. Zero calibration
is explicit and never runs automatically.

There is no generic sensor dependency, Kalman filter, low-pass filter, HAL call,
DMA ownership or CAN encoding. Keep raw ADC acquisition in the board interface.
The ADC scale is explicit: 12-bit codes use 4095; 16-bit codes averaged with an
equal oversampling right shift use 65535. Unshifted sums require their actual
maximum code.

Every update replaces `sensor.sample`. Its selected current uses int32_t mA
and is usable only when `valid` is true. Per-channel `current_a` remains a raw
diagnostic even for invalid samples, so always inspect `flags`. Invalid selection
has range `DHAB_NONE` and numeric current zero.

Low range exits above 90% of its range and re-enters below 80%. A valid high
channel is required. If two otherwise usable channels disagree by over 5 A,
neither is selected; this is a software plausibility threshold, not a datasheet
accuracy claim. ADC rails, out-of-range codes/currents, clamp bands, invalid
configuration/reference/supply, startup and overload recovery are distinguished.
The 0.3/4.7 V clamp boundaries track supply. Startup waits 1 ms; each channel
waits 10 ms after an observed overload or invalid input. Timestamp subtraction
handles normal uint32_t rollover. Callers must separately enforce freshness,
observe overloads often enough and protect shared result publication.

The older [`dhabs134` API](dhabs134.md) remains for existing board callers.
Its int16_t centiamp format cannot represent the S/118's 350 A range.

References: [LEM S/118 datasheet, version 5](https://www.lem.com/sites/default/files/products_datasheets/dhab_s_118.pdf)
and [LEM S/134 datasheet](https://www.lem.com/sites/default/files/products_datasheets/dhab_s_134.pdf).
