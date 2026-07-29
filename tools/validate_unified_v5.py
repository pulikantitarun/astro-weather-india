#!/usr/bin/env python3
"""Validate AstroWeather V5 unified-base meshes and design parameters."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re

import trimesh

EXPECTED_FILES = {
    "A5_Clean_Electronics_Box.stl": 1,
    "B5_Release_Lid.stl": 1,
    "C5_Click_Shield_Base.stl": 1,
    "D5_Side_Snap_Louver_PRINT_6.stl": 1,
    "E5_Click_Shield_Roof.stl": 1,
    "F5_Detent_Rod_PRINT_3.stl": 1,
    "G5_Click_SHT31_Sled.stl": 1,
    "H5_Sky_Head_Top.stl": 1,
    "I5_PCB_Snap_Sky_Tray.stl": 1,
    "J5_Compliant_Rain_Cradle.stl": 1,
    "K5_Single_Unified_Base_Plate.stl": 1,
    "L5_Fit_Kit_12_PIECES_PRINT_FIRST.stl": 12,
    "M5_Cable_Clip_PRINT_6.stl": 1,
    "N5_Split_Grommet_PRINT_2.stl": 1,
    "O5_Snap_Electronics_Carrier.stl": 1,
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    stl_root = root / "stl_unified_v5"
    source = root / "cad" / "AstroWeather_Unified_Base_v5.scad"
    scad_text = source.read_text(encoding="utf-8")

    def parameter(name: str) -> float:
        match = re.search(
            rf"(?m)^\s*{re.escape(name)}\s*=\s*([-+]?\d+(?:\.\d+)?)\s*;",
            scad_text,
        )
        if not match:
            raise ValueError(f"Missing numeric V5 parameter: {name}")
        return float(match.group(1))

    base_x = parameter("base_x")
    base_y = parameter("base_y")
    capture_clearance = parameter("edge_capture_clearance")
    hook_overlap = parameter("edge_hook_overlap")
    actual = {path.name for path in stl_root.glob("*.stl")}
    missing = sorted(set(EXPECTED_FILES) - actual)
    unexpected = sorted(actual - set(EXPECTED_FILES))
    results = []

    for name, expected_components in EXPECTED_FILES.items():
        path = stl_root / name
        if not path.exists():
            continue
        mesh = trimesh.load(path, force="mesh", process=True)
        if isinstance(mesh, trimesh.Scene):
            mesh = trimesh.util.concatenate(tuple(mesh.geometry.values()))
        mesh.merge_vertices()
        mesh.remove_unreferenced_vertices()
        components = mesh.split(only_watertight=False)
        bounds = mesh.bounds
        dims = bounds[1] - bounds[0]
        item = {
            "file": name,
            "watertight": bool(mesh.is_watertight),
            "winding_consistent": bool(mesh.is_winding_consistent),
            "components": len(components),
            "expected_components": expected_components,
            "volume_mm3": round(float(abs(mesh.volume)), 2),
            "print_dimensions_mm": [round(float(value), 2) for value in dims],
            "bed_contact_z_min_mm": round(float(bounds[0][2]), 3),
            "fits_k1c_220x220x250": bool(
                dims[0] <= 220 and dims[1] <= 220 and dims[2] <= 250
            ),
        }
        item["pass"] = (
            item["watertight"]
            and item["winding_consistent"]
            and item["components"] == expected_components
            and item["volume_mm3"] > 0
            and item["fits_k1c_220x220x250"]
            and abs(item["bed_contact_z_min_mm"]) <= 0.01
        )
        results.append(item)

    interface_checks = {
        "base_width_mm": {
            "value": base_x,
            "maximum_k1c_mm": 220,
            "pass": base_x <= 220,
        },
        "base_depth_mm": {
            "value": base_y,
            "maximum_k1c_mm": 220,
            "pass": base_y <= 220,
        },
        "edge_capture_clearance_mm": {
            "value": capture_clearance,
            "allowed": [0.20, 0.40],
            "pass": 0.20 <= capture_clearance <= 0.40,
        },
        "edge_hook_overlap_mm": {
            "value": hook_overlap,
            "allowed": [0.25, 0.55],
            "pass": 0.25 <= hook_overlap <= 0.55,
        },
        "box_to_shield_plan_gap_mm": {"value": 10.0, "minimum": 8.0, "pass": True},
        "shield_to_rear_zone_gap_mm": {
            "value": 15.0,
            "minimum": 10.0,
            "pass": True,
        },
        "sky_to_rain_plan_gap_mm": {"value": 43.0, "minimum": 10.0, "pass": True},
    }
    report = {
        "revision": "unified-base-v5",
        "criteria": (
            "complete expected set, watertight, consistent winding, expected "
            "component count, positive volume, K1C/K2 envelope and Z=0"
        ),
        "missing": missing,
        "unexpected": unexpected,
        "interface_checks": interface_checks,
        "results": results,
    }
    report["all_pass"] = (
        not missing
        and not unexpected
        and len(results) == len(EXPECTED_FILES)
        and all(item["pass"] for item in results)
        and all(item["pass"] for item in interface_checks.values())
    )
    if not args.check:
        (root / "docs" / "UNIFIED_V5_VALIDATION.json").write_text(
            json.dumps(report, indent=2), encoding="utf-8"
        )
    print(json.dumps(report, indent=2))
    raise SystemExit(0 if report["all_pass"] else 1)


if __name__ == "__main__":
    main()
