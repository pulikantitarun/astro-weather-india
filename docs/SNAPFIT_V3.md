# Snap-fit and cable-management mechanical revision 3

This is an optional non-cellular mechanical revision. It preserves the original
v2 STLs and adds a separate `stl_snapfit_v3/` set.

## What changed

- A3 has two labelled 10 mm cable entries, internal strain-relief anchors,
  routing fences and board tie-down anchors.
- B3 has two opposing PETG cantilever latches. Press both labelled A3 windows
  while lifting the lid to release it.
- I3 has four tapered detent heads that snap into the existing H sockets.
- K3 adds a protected cable spine with four cable-tie stations.
- M3 is a flexible clip for approximately 4-5.5 mm cable; print six.
- N3 is a split grommet for the A3 cable entries; print two.
- L3 reproduces the exact latch, sky detent, cable clip and grommet interfaces.

The SHT31 shield rods and louvres remain the v2 press-fit design. The rain board
remains an adjustable slide fit because low-cost PCB dimensions vary.

## Print L3 first

`L3_Fit_Kit_6_PIECES_PRINT_FIRST.stl` contains six separate pieces:

| Piece | Test |
|---|---|
| L1 | A3 wall and release window |
| L2 | B3 flexible latch tongue |
| L3 | I3 sky-head detent peg |
| L4 | H sky-head socket |
| L5 | M3 cable clip |
| L6 | N3 split grommet |

Use the same orientation already stored in the STL. PETG baseline:

- 0.4 mm nozzle;
- 0.20 mm layers;
- four walls;
- five bottom layers;
- 20% gyroid;
- supports off;
- 0.15 mm elephant-foot compensation if calibrated.

For L1/L2, hold the bases so the tongue enters the wall from the open side.
The ramp should slide through and click into the window. Press the hook through
the window to release it. Ten insert/release cycles should not whiten or crack
the tongue.

L3 should click through L4 with a light hand press. It must not require pliers.
L5 should accept the intended cable without cutting its jacket. L6 should
compress through a 10 mm hole and remain captured by both flanges.

If the latch is too tight, first correct flow and elephant foot. Then increase
`lid_clearance_per_side` in the SCAD source by 0.05 mm. If the sky detent is too
tight, reduce `sky_peg_head_d` by 0.05 mm. Re-export the related full part and
L3 after every change.

## Electronics box assembly

1. Press one N3 grommet around the 5 V cable and one around the sensor loom.
2. Compress each split grommet into its labelled A3 entry.
3. Route power through `POWER` and the combined sensor loom through `SENSORS`.
4. Secure each cable to the nearby internal anchor with a small cable tie.
5. Mount the ESP32 and perfboard on the rails and use the raised board anchors
   for tie-downs. Do not pull ties through the enclosure floor.
6. Keep cables below the top latch windows.
7. Align B3 and press evenly above the two latch sides until both click.
8. To open, press both `PUSH` windows inward while lifting B3.

Do not seal the lid until all electronics and rain tests pass. Snap fits improve
assembly but do not make the enclosure waterproof.

## External cable routing

K3 is 160 x 100 mm and fits the K1C/K2. The original box location is unchanged;
the added 20 mm spine is the side labelled `CABLE`.

1. Run the combined low-voltage sensor loom between the two raised spine rails.
2. Use small UV-resistant cable ties through each pair of slots.
3. Use M3 clips on a mast or crossbar where the loom leaves K3.
4. Clip spacing should be about 80-120 mm.
5. Leave a 30-50 mm service loop and a lower drip loop at every sensor head.
6. Keep all cables clear of the MLX90614 and TSL2591 upward-looking apertures.

Use outdoor-rated flexible cable. Loose Dupont jumpers are for bench testing
only, not outdoor installation.

## Compatibility limits

- A3/B3 replaces A/B as a matched pair.
- I3 works with the existing H socket dimensions but must pass the L3/L4 test.
- K3 replaces K when the integrated cable spine is wanted.
- M3 is intended for 4-5.5 mm cable.
- N3 is intended for approximately 4-5 mm cable jacket inside a 10 mm A3 hole.
- Board clone dimensions, cable jacket hardness and printer flow still require
  a physical test.

Mesh validation cannot prove snap force, fatigue life, UV durability, weather
sealing or fit with a specific electronics clone.
