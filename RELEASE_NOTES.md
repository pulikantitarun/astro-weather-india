# AstroWeather India v1.0.0

First public, buildable release of the low-cost astrophotography weather station.

Included:

- 12 labelled, support-minimised STL parts plus editable OpenSCAD source
- ESP32 firmware for SHT31, MLX90614, TSL2591, and the resistive rain board
- wiring, Indian bill of materials, assembly, calibration, and print instructions
- K1C/K2-oriented print settings and assembly visuals
- local web dashboard and JSON data endpoint

The STL files passed automated mesh and build-volume checks. The firmware compiles
for the ESP32 Dev Module target. Physical fit, sensor calibration, weather sealing,
and safe behaviour at the actual observing site still require staged testing.

Licensed under GNU AGPL v3.0 or later.

## Added after v1.0.0

- reference-calibrated estimated SQM in mag/arcsec²
- explicitly heuristic estimated Bortle class
- dashboard and JSON status for calibration, underflow, and saturation
- calibration worksheet and offset calculator
