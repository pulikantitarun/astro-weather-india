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

## 3. Interpret thermal clarity

In this firmware:

```text
clarity_score = 100 - cloud_percent
```

It answers “how thermally clear does the zenith look after local calibration?”
It does not measure seeing, atmospheric turbulence, transparency, aerosol
optical depth, or star FWHM.

Clouds may brighten an urban sky but darken a rural sky, so a universal
light-level-to-clarity direction is impossible. The SQM estimate is reported
separately and is not used by the imaging safety advisory.

## 4. Calibrate the SQM estimate

The TSL2591 uses maximum gain and 600 ms integration. Keep those settings fixed
after calibration. The firmware calculates:

```text
visible_counts = tsl_full - tsl_ir
estimated_SQM = SQM_CAL_OFFSET - 2.5 × log10(visible_counts)
```

`SQM_CAL_OFFSET` defaults to `NAN`, so the dashboard shows `CALIBRATE` instead
of inventing a reading.

To calibrate:

1. Mount the station permanently, pointing the light-sensor opening at the
   zenith.
2. Place a reference SQM beside it with the same pointing direction.
3. Wait until astronomical darkness on a clear, Moon-free night. Avoid the
   Milky Way, direct lamps, haze, cloud and shadows across either aperture.
4. Record at least five simultaneous pairs in
   `docs/SQM_CALIBRATION_TEMPLATE.csv`. Copy `tsl_full`, `tsl_ir`, and
   `tsl_visible` from `/api/weather`.
5. Calculate the offset:

   ```bash
   python tools/calculate_sqm_offset.py docs/SQM_CALIBRATION_TEMPLATE.csv
   ```

6. Enter the recommended value near the top of the firmware:

   ```cpp
   float SQM_CAL_OFFSET = 27.1234;
   ```

7. Upload the firmware and repeat simultaneous readings on at least two
   additional nights.

For a single paired observation, the equivalent calculation is:

```text
SQM_CAL_OFFSET = reference_SQM + 2.5 × log10(visible_counts)
```

The API reports `tsl_light_valid=false` and suppresses the SQM estimate when the
sensor has no usable visible counts or either raw channel is near saturation.
Changing the sensor, optical tube, window, paint, mounting angle, gain or
integration time invalidates the offset.

The displayed Bortle class is a heuristic conversion from estimated SQM:

| Estimated SQM (mag/arcsec²) | Display |
|---:|---:|
| 21.99 or higher | Bortle 1 |
| 21.89–21.98 | Bortle 2 |
| 21.69–21.88 | Bortle 3 |
| 20.49–21.68 | Bortle 4 |
| 19.50–20.48 | Bortle 5 |
| 18.94–19.49 | Bortle 6 |
| 18.38–18.93 | Bortle 7 |
| 17.80–18.37 | Bortle 8 |
| Below 17.80 | Bortle 9 |

Bortle is fundamentally a visual assessment of the whole sky. Treat this value
as a convenient label, not a measured Bortle classification. Moonlight, the
Milky Way, airglow, haze, cloud and nearby lights can shift it substantially.

## 5. Dew and imaging advisory

Default advisory limits are:

- RH below 90%;
- air temperature more than 2 °C above dew point;
- estimated cloud below 55%.

These are conservative starting points, not safety standards. A dew heater on
the telescope may allow imaging closer to dew point, but it does not stop
condensation on the station optics.

## 6. Keep a validation log

For the first month, note timestamp, Moon, visual cloud, haze, actual imaging
quality, and the JSON reading. Re-tune thresholds for monsoon, winter, and very
dry seasons if their baselines differ.

## 7. Calibrate the rain plate

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
