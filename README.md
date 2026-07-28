# AstroWeather India

An affordable, Wi-Fi-connected and almost entirely 3D-printable weather station
for attended astrophotography. It measures temperature, humidity, dew risk,
thermal cloud cover, relative sky brightness and rain, then presents a simple
imaging advisory through an ESP32 web dashboard and JSON API.

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](LICENSE)
[![Validate](https://github.com/pulikantitarun/astro-weather-india/actions/workflows/validate.yml/badge.svg)](https://github.com/pulikantitarun/astro-weather-india/actions/workflows/validate.yml)

> **Safety:** This is an attended-imaging aid, not a certified weather safety
> controller. Never use the low-cost rain plate as the sole input for an
> unattended observatory roof.

![Electronics assembly](assets/electronics-assembly-visual.png)

## Capabilities

- SHT31 ambient temperature and relative humidity
- calculated dew point and dew margin
- MLX90614 infrared zenith temperature
- locally calibrated cloud percentage and clarity score
- TSL2591 lux, full-spectrum and infrared counts
- low-duty-cycle, alternating-polarity rain detection
- ten-minute unsafe rain latch
- configurable `OK` / `UNSAFE` imaging advisory
- live phone/desktop web dashboard, refreshed every ten seconds
- JSON API at `/api/weather`
- sensor health flags and uptime
- normal Wi-Fi client mode with setup access-point fallback
- support-free printable parts labelled A-L
- editable OpenSCAD source and validated STL files
- designed for Creality K1C and all K2 build volumes

The clarity score is a calibrated **thermal cloud score**. It does not measure
seeing, atmospheric turbulence, aerosol optical depth, transparency or star
FWHM. TSL2591 readings are relative until compared against a known sky-quality
meter.

## Approximate India build cost

These are approximate July 2026 India reference prices before delivery charges.
Choose compatible modules from a supplier you trust.

| Group | Cost |
|---|---:|
| ESP32, MLX90614, rain sensor, wiring, solder, flux and power supply | ₹1,550.00 |
| SHT31 module | ₹312.70 |
| TSL2591 module | ₹755.00 |
| Electronics and assembly supplies subtotal | **₹2,617.70** |
| Approximately 250 g PETG | ₹200.00 |
| Reusable mounting straps | ₹50.00 |
| Estimated complete build | **₹2,867.70** |

Round-budget **₹2,900 plus shipping**. A soldering iron and ordinary hand tools
are not included. Optional conformal coating adds approximately ₹250.

See [`docs/BOM_India.csv`](docs/BOM_India.csv) for the complete itemized list.

## Printed assembly

![Exploded assembly](assets/assembly-exploded.png)

1. Print `stl/L_Fit_Kit_4_PIECES_PRINT_FIRST.stl`.
2. Confirm both press-fit pairs before printing the full set.
3. Print the remaining files in their supplied orientation with supports off.
4. Measure clone sensor boards before printing H and I.
5. Follow [`docs/ASSEMBLY_GUIDE.md`](docs/ASSEMBLY_GUIDE.md).

Recommended baseline: PETG, 0.4 mm nozzle, 0.20 mm layers and four walls. Full
K1C/K2 settings are in
[`docs/PRINT_SETTINGS_K1C_K2.md`](docs/PRINT_SETTINGS_K1C_K2.md).

## Electronics

All three digital sensors share the ESP32 I2C bus:

| ESP32 | Function |
|---|---|
| 3V3 | sensor power |
| GND | common ground |
| GPIO21 | SDA |
| GPIO22 | SCL |
| GPIO32 | rain electrode A network |
| GPIO33 | rain electrode B network |

Use only the exposed-trace rain plate, not its LM393 comparator board. The
complete pin map and rain resistor network are in
[`docs/WIRING.csv`](docs/WIRING.csv).

## Firmware

The firmware is provided for Arduino/PlatformIO.

```bash
cd firmware
copy AstroWeather_ESP32\secrets.example.h AstroWeather_ESP32\secrets.h
pio run
pio run --target upload
pio device monitor
```

Edit `secrets.h` with the local Wi-Fi name and password. This file is ignored by
Git and must never be committed.

If normal Wi-Fi connection fails after 15 seconds, the station creates:

- SSID: `AstroWeather-Setup`
- password: `astroclear`

The intended local hostname is `http://astroweather.local`.

### API fields

`GET /api/weather` returns:

- `air_c`, `humidity`, `dew_c`, `dew_margin_c`
- `sky_c`, `cloud_delta_c`, `cloud_percent`, `clarity_score`
- `lux`, `tsl_full`, `tsl_ir`
- `rain_raw`, `rain_detected`
- `sht_ok`, `mlx_ok`, `tsl_ok`
- `advisory_safe`, `uptime_s`

`GET /health` verifies that the HTTP server is responding. Sensor health is
reported separately in the weather JSON.

## Calibration

Local calibration is essential:

1. compare the shaded SHT31 against a trustworthy reference;
2. collect MLX90614 data on at least three clear and three overcast nights;
3. enter the local clear/overcast thermal deltas in the firmware;
4. calibrate dry and droplet readings for the rain threshold;
5. validate the advisory through several supervised imaging sessions.

See [`docs/CALIBRATION.md`](docs/CALIBRATION.md).

## Repository layout

- `assets/` — assembly visuals
- `cad/` — editable OpenSCAD source
- `docs/` — BOM, wiring, printing, assembly and calibration
- `firmware/` — ESP32 source and PlatformIO configuration
- `stl/` — print-ready meshes
- `tools/` — STL validation utility

## Licence

Copyright © 2026 Tarun and contributors.

This project—including firmware, CAD, printable models, documentation and
visuals—is licensed under the
[GNU Affero General Public License v3.0 or later](LICENSE). If you modify and
operate a network-accessible version, the AGPL requires the corresponding source
to be offered to its users. Derivative distributions must preserve the same
licence obligations.
