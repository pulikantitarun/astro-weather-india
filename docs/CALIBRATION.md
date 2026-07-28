# Calibration

## 1. Verify temperature and humidity

Place the completed radiation shield beside a trustworthy thermometer/hygrometer
in shade for 30–60 minutes. Do not calibrate indoors and then assume the same
offset outdoors. The ESP32 box must be separated from the shield.

For a simple humidity reference, a sealed salt test can create known equilibrium
conditions, but temperature stability and correct salt preparation matter.
For astrophotography, agreement within roughly ±0.5 °C and ±3% RH is usually
more useful than aggressive software correction from a poor reference.

## 2. Learn the local cloud thresholds

The firmware uses:

```text
cloud_delta = ambient_air_temperature - infrared_sky_temperature

cloud % = 100 × (CLEAR_DELTA - cloud_delta)
                / (CLEAR_DELTA - OVERCAST_DELTA)
```

Collect readings only after dark:

1. On at least three visually clear nights, record `cloud_delta_c` every
   5–10 minutes for an hour. Use the median as `CLEAR_DELTA_C`.
2. Repeat on at least three fully overcast, dry nights. Use the median as
   `OVERCAST_DELTA_C`.
3. Enter both values near the top of the firmware and upload again.
4. Validate on mixed-cloud nights. Adjust `CLOUD_LIMIT_PERCENT` to match the
   imaging standard you actually use.

Humid Indian conditions often make clear sky look warmer in IR than dry desert
sky. This is why a universal “clear below −10 °C” rule is unreliable.

Thin high cirrus can remain cold and may be underestimated. Conversely, dew or
water on the MLX window appears warm and can look like cloud. Inspect raw
`sky_c`, `cloud_delta_c`, and recent trends whenever the result seems wrong.

## 3. Interpret clarity

In this firmware:

```text
clarity_score = 100 - cloud_percent
```

It answers “how thermally clear does the zenith look after local calibration?”
It does not measure seeing, atmospheric turbulence, transparency, aerosol
optical depth, or star FWHM.

The TSL2591 `lux`, `tsl_full`, and `tsl_ir` values are intentionally exposed as
relative measurements. Compare nights at the same installation, with similar
Moon altitude/phase and after full darkness. Clouds may brighten an urban sky
but darken a rural sky, so a universal lux-to-clarity direction is impossible.

For absolute mag/arcsec²:

1. Place a commercial SQM beside the station.
2. Collect paired readings over clear nights and multiple brightness levels.
3. Fit an offset/slope for this exact sensor and optical tube.
4. Re-check after changing the tube, window, paint, or mounting angle.

## 4. Dew and imaging advisory

Default advisory limits are:

- RH below 90%;
- air temperature more than 2 °C above dew point;
- estimated cloud below 55%.

These are conservative starting points, not safety standards. A dew heater on
the telescope may allow imaging closer to dew point, but it does not stop
condensation on the station optics.

## 5. Keep a validation log

For the first month, note timestamp, Moon, visual cloud, haze, actual imaging
quality, and the JSON reading. Re-tune thresholds for monsoon, winter, and very
dry seasons if their baselines differ.

## 6. Calibrate the rain plate

1. Let the clean plate dry completely and record `rain_raw` for ten readings.
2. Mist it with a few separate tap-water droplets and record ten readings.
3. Set `RAIN_THRESHOLD_RAW` midway between the highest dry value and the lowest
   repeatable droplet value.
4. Test with a fine spray from several directions. Confirm that
   `rain_detected` becomes true and `advisory_safe` becomes false.
5. The firmware holds rain unsafe for ten minutes after a wet reading. Change
   `600000UL` only after real testing.

Dew, insects, dirt, sea salt, and condensation can trigger the plate. That is a
safe failure direction for attended imaging but makes it unsuitable as the sole
sensor for an unattended roof. The exposed traces are consumable even with
polarity reversal; inspect them monthly during monsoon conditions.
