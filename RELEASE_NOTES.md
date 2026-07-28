# AstroWeather India v1.2.0

This release combines the complete printable station, calibrated SQM/Bortle
estimates and astronomy-software integration.

## New in v1.2.0

- direct ESP32 ASCOM Alpaca server on HTTP port 80
- standard Alpaca UDP discovery on port 32227
- ASCOM ObservingConditions device for temperature, humidity, dew point,
  cloud cover, sky temperature, sky brightness and calibrated estimated SQM
- ASCOM SafetyMonitor device for the combined advisory result
- buildable native INDI Weather driver for KStars/Ekos-compatible systems
- INDI weather, rain, safety, estimated SQM and estimated Bortle values
- integration contract tests and continuous INDI driver compilation
- complete ASCOM and INDI setup/troubleshooting documentation

Rain remains a truthful binary dry/wet safety input; the software does not
misrepresent it as a measured millimetres-per-hour rain rate. ASIAIR support is
not claimed because its standard firmware does not permit arbitrary third-party
INDI drivers.

## Base release contents

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

## Added in the SQM/Bortle update

- reference-calibrated estimated SQM in mag/arcsec²
- explicitly heuristic estimated Bortle class
- dashboard and JSON status for calibration, underflow, and saturation
- calibration worksheet and offset calculator
