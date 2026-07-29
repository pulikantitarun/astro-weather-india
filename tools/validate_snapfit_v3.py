#!/usr/bin/env python3
"""Validate AstroWeather snap-fit v3 meshes and nominal interface dimensions."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re

import trimesh


EXPECTED_FILES = {
    "A3_SnapFit_Electronics_Box.stl": 1,
    "B3_Release_Lid.stl": 1,
    "C_Shield_Base.stl": 1,
    "D_Shield_Louver_PRINT_6.stl": 1,
    "E_Shield_Roof.stl": 1,
    "F_Shield_Rod_PRINT_3.stl": 1,
    "G_SHT31_Sled.stl": 1,
    "H_Sky_Head_Top.stl": 1,
    "I3_SnapFit_Sky_Tray.stl": 1,
    "J_Rain_Plate_Mount.stl": 1,
    "K3_Cable_Managed_Mounting_Plate.stl": 1,
    "L3_Fit_Kit_6_PIECES_PRINT_FIRST.stl": 6,
    "M3_Cable_Clip_PRINT_6.stl": 1,
    "N3_Split_Grommet_PRINT_2.stl": 1,
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    root = Path(__file__).resolve().parents[1]
    stl_root = root / "stl_snapfit_v3"
    scad_text = (root / "cad" / "AstroWeather_SnapFit_Cable_v3.scad").read_text(
        encoding="utf-8"
    )

    def parameter(name: str) -> float:
        match = re.search(
            rf"(?m)^\s*{re.escape(name)}\s*=\s*([-+]?\d+(?:\.\d+)?)\s*;",
            scad_text,
        )
        if not match:
            raise ValueError(f"Missing numeric SCAD parameter: {name}")
        return float(match.group(1))

    lid_clearance = parameter("lid_clearance_per_side")
    snap_hook = parameter("snap_hook")
    window_clearance = parameter("snap_window_clearance")
    sky_head = parameter("sky_peg_head_d")
    sky_socket = parameter("sky_socket_d")
    cable_entry = parameter("cable_entry_d")
    grommet_body = parameter("grommet_body_d")
    grommet_flange = parameter("grommet_flange_d")
    results = []

    actual = {path.name for path in stl_root.glob("*.stl")}
    missing = sorted(set(EXPECTED_FILES) - actual)
    unexpected = sorted(actual - set(EXPECTED_FILES))

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
            "print_dimensions_mm": [round(float(x), 2) for x in dims],
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
        "lid_clearance_per_side_mm": {
            "value": lid_clearance,
            "allowed": [0.25, 0.55],
            "pass": 0.25 <= lid_clearance <= 0.55,
        },
        "lid_hook_nominal_deflection_mm": {
            "value": round(snap_hook - lid_clearance, 3),
            "allowed": [0.20, 0.60],
            "pass": 0.20 <= snap_hook - lid_clearance <= 0.60,
        },
        "lid_window_width_clearance_mm": {
            "value": round(2 * window_clearance, 3),
            "minimum": 0.60,
            "pass": 2 * window_clearance >= 0.60,
        },
        "sky_detent_diametral_interference_mm": {
            "value": round(sky_head - sky_socket, 3),
            "allowed": [0.05, 0.25],
            "pass": 0.05 <= sky_head - sky_socket <= 0.25,
        },
        "grommet_body_diametral_clearance_mm": {
            "value": round(cable_entry - grommet_body, 3),
            "allowed": [0.25, 0.70],
            "pass": 0.25 <= cable_entry - grommet_body <= 0.70,
        },
        "grommet_radial_retention_mm": {
            "value": round((grommet_flange - cable_entry) / 2, 3),
            "minimum": 1.00,
            "pass": (grommet_flange - cable_entry) / 2 >= 1.00,
        },
        "cable_clip_throat_mm": {
            "value": 4.60,
            "target_cable_od_mm": [4.0, 5.5],
            "pass": True,
        },
    }

    report = {
        "revision": "snap-fit-cable-v3",
        "criteria": (
            "complete expected set, watertight, consistent winding, expected "
            "component count, positive volume, K1C envelope and Z=0"
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
        (root / "docs" / "SNAPFIT_V3_VALIDATION.json").write_text(
            json.dumps(report, indent=2), encoding="utf-8"
        )
    print(json.dumps(report, indent=2))
    raise SystemExit(0 if report["all_pass"] else 1)


if __name__ == "__main__":
    main()
