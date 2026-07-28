#!/usr/bin/env python3
"""Calculate the TSL2591 SQM calibration offset from paired observations.

SPDX-License-Identifier: AGPL-3.0-or-later
Copyright (C) 2026 Tarun and contributors
"""

from __future__ import annotations

import argparse
import csv
import math
import statistics
from pathlib import Path


def calibration_offset(reference_sqm: float, visible_counts: int) -> float:
    if not math.isfinite(reference_sqm):
        raise ValueError("reference SQM must be finite")
    if visible_counts <= 0:
        raise ValueError("visible counts must be greater than zero")
    return reference_sqm + 2.5 * math.log10(visible_counts)


def read_offsets(path: Path) -> list[float]:
    offsets: list[float] = []
    with path.open(newline="", encoding="utf-8-sig") as handle:
        for line_number, row in enumerate(csv.DictReader(handle), start=2):
            try:
                reference_sqm = float(row["reference_sqm"])
                visible_text = (row.get("visible_counts") or "").strip()
                if visible_text:
                    visible_counts = int(visible_text)
                else:
                    full = int(row["tsl_full"])
                    infrared = int(row["tsl_ir"])
                    visible_counts = full - infrared
                offsets.append(calibration_offset(reference_sqm, visible_counts))
            except (KeyError, TypeError, ValueError):
                if any((value or "").strip() for value in row.values()):
                    print(f"Skipping invalid row {line_number}")
    return offsets


def self_test() -> None:
    # A count of 100 adds exactly five magnitudes to the reference value.
    assert math.isclose(calibration_offset(20.0, 100), 25.0, abs_tol=1e-9)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Calculate SQM_CAL_OFFSET from paired SQM and TSL2591 readings."
    )
    parser.add_argument("csv_file", nargs="?", type=Path)
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()

    if args.self_test:
        self_test()
        print("SQM calibration helper self-test passed.")
        return 0
    if args.csv_file is None:
        parser.error("csv_file is required unless --self-test is used")

    offsets = read_offsets(args.csv_file)
    if not offsets:
        raise SystemExit("No valid paired calibration rows were found.")

    recommended = statistics.median(offsets)
    spread = max(offsets) - min(offsets)
    print(f"Valid pairs: {len(offsets)}")
    print(f"Recommended SQM_CAL_OFFSET: {recommended:.4f}")
    print(f"Offset range: {min(offsets):.4f} to {max(offsets):.4f}")
    print(f"Range width: {spread:.4f} mag")
    if len(offsets) < 5:
        print("Warning: collect at least five paired readings.")
    if spread > 0.5:
        print("Warning: large spread; check Moon, clouds, alignment, and saturation.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
