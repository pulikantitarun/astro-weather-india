# K1C / K2 print settings

## Recommended outdoor material

Use white or light-coloured PETG for the affordable build. ASA is better for
permanent ultraviolet exposure but needs controlled ventilation and suitable
filament experience.

## PETG profile

- Nozzle: 0.4 mm.
- Layer height: 0.20 mm.
- First layer: 0.24 mm.
- Walls: 4.
- Top and bottom shell layers: 5.
- Infill: 20% gyroid; use 30% for K mounting plate.
- Nozzle: start at 240 °C and use the filament maker’s range.
- Bed: start at 75 °C.
- Supports: **off for every STL**.
- Seam: rear or aligned.
- Elephant-foot compensation: 0.15 mm if already calibrated.
- Slow external walls: 80–120 mm/s for cleaner labels.
- Keep the K1C/K2 chamber from becoming excessively hot with PETG; use the
  filament manufacturer’s guidance and watch for heat creep.

## Adhesion

- A–E, G–I, K and L: skirt only on a clean textured PEI plate.
- F rods: add a 3 mm brim or mouse ears because they are narrow.
- J rain mount: add a 5 mm brim because it prints tall on its broad side.
- Dry PETG before printing if it pops, strings, or produces rough labels.

## Import orientation

Do not use auto-orient. Every STL is already exported with its intended face at
Z=0:

- A box upright.
- B lid outer face on the bed, plug upward.
- C base flat.
- D louvre flat, spacer bosses upward; print six.
- E roof flat.
- F rod lying flat; print three.
- G sled flat.
- H sky-head top flat, optical tubes upward.
- I electronics tray flat, board guides upward.
- J rain mount standing on its broad side; this is intentional.
- K mounting plate flat.
- L fit-kit pieces flat.

## First-print decision

Print L before everything else:

- L2 should enter L1 with a light push and remain seated without force.
- L3 should press into L4 by hand without splitting L4.

If too tight, first verify flow calibration and elephant foot. Lightly sand the
test piece. For a source-level adjustment:

- increase `lid_clearance_per_side` from 0.30 to 0.40 mm;
- increase `peg_clearance_d` from 0.15 to 0.25 mm.

Re-export B, H, I and L after changing those values.
