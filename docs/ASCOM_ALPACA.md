# ASCOM Alpaca setup

AstroWeather v1.2 exposes ASCOM Alpaca directly from the ESP32. No Windows
driver executable, bridge computer or additional electronics are required.

## Devices

The station advertises two independent device number `0` entries:

- **AstroWeather Observing Conditions** — air temperature, humidity, dew point,
  cloud cover, sky temperature, sky brightness and calibrated estimated SQM.
- **AstroWeather Safety Monitor** — the station's combined advisory result,
  including the dry/wet rain plate state.

Both use HTTP port 80. Standard UDP Alpaca discovery listens on port 32227.
The unique IDs are derived from the ESP32 MAC address and remain stable for
that board.

## Windows setup

1. Put the ESP32 and Windows imaging computer on the same local network.
2. Flash v1.2 firmware and confirm
   `http://astroweather.local/api/weather` opens.
3. In an Alpaca-aware ASCOM chooser, discover devices and select the
   AstroWeather Observing Conditions and/or Safety Monitor device.
4. If discovery is blocked, add the device manually using
   `astroweather.local`, port `80`, device number `0`.
5. Permit private-network UDP port `32227` and TCP port `80` in the Windows
   firewall if required.

The endpoints can be checked in a browser:

```text
http://astroweather.local/management/apiversions
http://astroweather.local/management/v1/configureddevices
http://astroweather.local/api/v1/observingconditions/0/temperature
http://astroweather.local/api/v1/safetymonitor/0/issafe
```

## Data mapping

| ASCOM property | Station field | Behaviour |
|---|---|---|
| `Temperature` | `air_c` | degrees Celsius |
| `Humidity` | `humidity` | 0–100% |
| `DewPoint` | `dew_c` | degrees Celsius |
| `CloudCover` | `cloud_percent` | 0–100% |
| `SkyTemperature` | `sky_c` | MLX90614 zenith IR temperature |
| `SkyBrightness` | `lux` | TSL2591 lux |
| `SkyQuality` | `sqm_estimate` | only after reference calibration |
| `IsSafe` | `advisory_safe` | combined advisory, including rain |

Pressure, wind, seeing/FWHM and measured rain rate are deliberately reported
as unsupported. The rain plate detects wet/dry state but cannot measure
millimetres per hour.

## Safety and SQM limits

`IsSafe` is an advisory, not a certified roof-control interlock. Use a
fail-safe, independent rain/roof system for unattended operation.

`SkyQuality` returns an ASCOM value-not-set error until `SQM_CAL_OFFSET` is
calibrated. This prevents an uncalibrated light count from appearing as a
real SQM measurement.
