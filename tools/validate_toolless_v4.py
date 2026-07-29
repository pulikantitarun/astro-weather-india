#!/usr/bin/env python3
"""Validate AstroWeather tool-less v4 meshes and nominal snap dimensions."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re

import trimesh

EXPECTED_FILES = {
    "A4_Toolless_Electronics_Box.stl": 1,
    "B4_Release_Lid.stl": 1,
    "C4_Click_Shield_Base.stl": 1,
    "D4_Side_Snap_Louver_PRINT_6.stl": 1,
    "E4_Click_Shield_Roof.stl": 1,
    "F4_Detent_Rod_PRINT_3.stl": 1,
    "G4_Click_SHT31_Sled.stl": 1,
    "H4_Sky_Head_Top.stl": 1,
    "I4_PCB_Snap_Sky_Tray.stl": 1,
    "J4_Compliant_Rain_Cradle.stl": 1,
    "K4_Click_Mounting_Plate.stl": 1,
    "L4_Fit_Kit_10_PIECES_PRINT_FIRST.stl": 10,
    "M4_Cable_Clip_PRINT_6.stl": 1,
    "N4_Split_Grommet_PRINT_2.stl": 1,
    "O4_Snap_Electronics_Carrier.stl": 1,
}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    stl_root = root / "stl_toolless_v4"
    source = root / "cad" / "AstroWeather_Toolless_Snap_v4.scad"
    scad_text = source.read_text(encoding="utf-8")

    def parameter(name: str) -> float:
        match = re.search(
            rf"(?m)^\s*{re.escape(name)}\s*=\s*([-+]?\d+(?:\.\d+)?)\s*;",
            scad_text,
        )
        if not match:
            raise ValueError(f"Missing numeric SCAD parameter: {name}")
        return float(match.group(1))

    rod_socket = parameter("rod_socket")
    rod_detent = parameter("rod_detent")
    louver_throat = parameter("louver_throat")
    louver_socket = parameter("louver_socket")
    carrier_head = parameter("carrier_peg_head")
    carrier_hole = parameter("carrier_hole")
    carrier_support = parameter("carrier_support_z")
    carrier_base = parameter("carrier_base_t")
    carrier_axial = parameter("carrier_axial_clearance")
    mount_overlap = parameter("mount_hook_overlap")
    pcb_overlap = parameter("pcb_hook_overlap")
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
        "rod_detent_diametral_interference_mm": {
            "value": round(rod_detent - rod_socket, 3),
            "allowed": [0.05, 0.25],
            "pass": 0.05 <= rod_detent - rod_socket <= 0.25,
        },
        "louver_retaining_shoulder_total_mm": {
            "value": round(louver_socket - louver_throat, 3),
            "allowed": [0.40, 0.80],
            "pass": 0.40 <= louver_socket - louver_throat <= 0.80,
        },
        "carrier_detent_diametral_interference_mm": {
            "value": round(carrier_head - carrier_hole, 3),
            "allowed": [0.05, 0.25],
            "pass": 0.05 <= carrier_head - carrier_hole <= 0.25,
        },
        "carrier_support_plane_mm": {
            "value": carrier_support,
            "required_internal_support_top_mm": 8.0,
            "pass": abs(carrier_support - 8.0) <= 0.01,
        },
        "carrier_head_base_above_carrier_mm": {
            "value": carrier_axial,
            "allowed": [0.05, 0.25],
            "pass": 0.05 <= carrier_axial <= 0.25,
        },
        "carrier_head_top_projection_mm": {
            "value": round(carrier_axial + 0.9, 3),
            "minimum": 0.8,
            "pass": carrier_axial + 0.9 >= 0.8,
        },
        "carrier_base_thickness_mm": {
            "value": carrier_base,
            "expected": 2.0,
            "pass": abs(carrier_base - 2.0) <= 0.01,
        },
        "mount_hook_overlap_mm": {
            "value": mount_overlap,
            "allowed": [0.25, 0.55],
            "pass": 0.25 <= mount_overlap <= 0.55,
        },
        "pcb_hook_overlap_mm": {
            "value": pcb_overlap,
            "allowed": [0.25, 0.60],
            "pass": 0.25 <= pcb_overlap <= 0.60,
        },
    }
    report = {
        "revision": "tool-less-snap-v4",
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
        (root / "docs" / "TOOLLESS_V4_VALIDATION.json").write_text(
            json.dumps(report, indent=2), encoding="utf-8"
        )
    print(json.dumps(report, indent=2))
    raise SystemExit(0 if report["all_pass"] else 1)


if __name__ == "__main__":
    main()
