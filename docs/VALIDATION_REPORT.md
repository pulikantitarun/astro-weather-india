# Validation report

Revision 2 was exported with OpenSCAD’s console renderer and checked with
`trimesh`.

All twelve STL files passed:

- watertight topology;
- consistent winding;
- expected connected-component count;
- positive enclosed volume;
- imported print face at Z=0;
- fit inside the K1C 220 × 220 × 250 mm envelope.

L intentionally contains four separate fit-test pieces in one STL. Every other
STL contains one connected printable component.

The exact dimensions and results are recorded in `STL_VALIDATION.json`.

## Printability design checks

- No shield disc starts above an air gap: D louvres print separately and stack.
- No sensor board clip is suspended below a plate: H and I print separately.
- J is pre-rotated onto a broad side and only its small 4 mm attachment holes
  require ordinary short bridging.
- F rods lie flat.
- K has no geometry below its base.
- Labels are merged into upward-facing or vertical printable surfaces.
- No support material is intended.

## Limits of this report

This is digital geometry validation, not a physical K1C or K2 test print.
Printer flow, elephant foot, filament shrinkage and clone-board dimensions can
change fit. Print L first and measure electronics before committing to the full
set.

## Software validation

For v1.2, continuous integration performs:

- ESP32 firmware compilation with PlatformIO;
- parsing and contract checks for the ASCOM Alpaca and INDI endpoints;
- native INDI driver configuration and C++ compilation on Ubuntu;
- web-configuration, NVS-key and Telegram TLS security contract checks;
- SQM calibration-helper self-test;
- all STL geometry and K1C build-envelope checks above.

Protocol checks do not replace a live conformance test on the finished ESP32,
and digital STL checks do not prove physical fit or weather sealing.
