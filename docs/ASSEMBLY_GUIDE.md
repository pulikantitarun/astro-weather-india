# Assembly guide

The physical letters on the parts match the filename prefixes.

## Printed inventory

| Letter | Part | Quantity |
|---|---|---:|
| A | Electronics box | 1 |
| B | Box lid | 1 |
| C | Radiation-shield base | 1 |
| D | Shield louvre | 6 |
| E | Shield roof | 1 |
| F | Square shield rod | 3 |
| G | SHT31 sled | 1 |
| H | Sky-head optical top | 1 |
| I | Sky-head sensor tray | 1 |
| J | Angled rain-plate mount | 1 |
| K | Main mounting plate | 1 |
| L1–L4 | Fit-test kit | 1 set |

## 1. Verify L before the full print

Push L2 into L1. Push L3 into L4. They should need a light hand push but should
not require a hammer, pliers, or visible bending. See the adjustment values in
`PRINT_SETTINGS_K1C_K2.md` if either interface is wrong.

## 2. Assemble the SHT31 radiation shield

1. Fit the SHT31 board onto G with the sensing opening unobstructed.
2. Slide G into the two guides in C. Route the four-wire cable through the open
   centre and away from the sensor face.
3. Press three F rods into the square sockets in C.
4. Slide six D louvres down the rods. The three spacer bosses and the embossed
   D face upward on every layer.
5. Press E onto the rod ends with the domed side upward.
6. Use the slotted ear on C to attach the shield. Keep it at least 150 mm from
   A so ESP32 heat cannot warm the humidity sensor.

```text
        E  roof
      D D D D D D     six louvres, bosses upward
       F   F   F      three square rods
          G           SHT31 sled inside
          C           shield base
```

## 3. Assemble the sky head

1. Place I with its embossed label upward.
2. Insert the MLX90614 board in the larger 22 × 18 mm pocket, sensor can facing
   upward.
3. Insert the TSL2591 board in the 20 × 18 mm pocket, photodiode facing upward.
4. Lower H over I. The wide open turret goes over the MLX90614; the narrow
   baffled tube goes over the TSL2591.
5. Align the four H holes with the four I pegs and press evenly at the corners.
6. Attach H using its two slotted mounting ears, with both optical tubes
   vertical and looking at unobstructed zenith.

Do not place ordinary glass, acrylic, clear tape, PETG, or epoxy over the
MLX90614 opening. These materials interfere with the long-wave infrared cloud
measurement.

Keep the TSL2591 tube geometry and interior finish unchanged after SQM
calibration. A window, insect mesh, different paint, altered tube length, sensor
replacement, or tilted mounting changes the optical response and requires a new
`SQM_CAL_OFFSET`. The dashboard deliberately withholds SQM and Bortle estimates
until reference calibration is entered.

## 4. Install the rain plate

1. After printing J on its broad side, rotate it into its working position.
2. Slide the common exposed-trace rain PCB between the two guide lips.
3. Put its connector at the raised end and the exposed traces facing upward.
4. Use the two holes at the low end of J for attachment.
5. Route the cable downward with a drip loop.

The slope drains water faster, but dew, dirt and insects can still trigger the
sensor. Inspect the exposed traces regularly.

## 5. Install the electronics box

1. Place the ESP32 and perfboard on the internal rails of A.
2. Connect sensors using `WIRING.csv`; power all I²C modules at 3.3 V.
3. Keep cable entries facing downward and form external drip loops.
4. Place A between the low guides on K and against the end stop.
5. Pass two reusable 12 mm straps through K and over A. The extra slots can
   secure K to a mast or fixed support.
6. Press B into A only after the L1/L2 fit has been verified.

## 6. Final checks before outdoor use

- Confirm the MLX90614 and TSL2591 look straight upward.
- Confirm the SHT31 is shaded and separated from processor heat.
- Confirm the rain plate drains away from A.
- Confirm every cable enters from below.
- Confirm `/api/weather` reports all three I²C health flags as true.
- Mist-test the rain plate and confirm `advisory_safe` becomes false.
- Complete the clear/overcast calibration in `CALIBRATION.md`.

Use the station for several supervised nights before trusting its advisory
status. Physical fit, weather sealing and safety cannot be proven by STL
validation alone.
