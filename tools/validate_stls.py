from __future__ import annotations

import argparse
import json
from pathlib import Path

import trimesh


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--check",
        action="store_true",
        help="validate without replacing the committed JSON report",
    )
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    results = []
    for path in sorted((root / "stl").glob("*.stl")):
        mesh = trimesh.load(path, force="mesh", process=True)
        if isinstance(mesh, trimesh.Scene):
            mesh = trimesh.util.concatenate(tuple(mesh.geometry.values()))
        mesh.merge_vertices()
        mesh.remove_unreferenced_vertices()
        components = mesh.split(only_watertight=False)
        bounds = mesh.bounds
        dims = bounds[1] - bounds[0]
        item = {
            "file": path.name,
            "watertight": bool(mesh.is_watertight),
            "winding_consistent": bool(mesh.is_winding_consistent),
            "components": len(components),
            "volume_mm3": round(float(abs(mesh.volume)), 2),
            "print_dimensions_mm": [round(float(x), 2) for x in dims],
            "bed_contact_z_min_mm": round(float(bounds[0][2]), 3),
        }
        item["fits_k1c_220x220x250"] = bool(
            dims[0] <= 220 and dims[1] <= 220 and dims[2] <= 250
        )
        expected_components = 4 if path.name.startswith("L_") else 1
        item["expected_components"] = expected_components
        item["pass"] = (
            item["watertight"]
            and item["winding_consistent"]
            and item["components"] == expected_components
            and item["volume_mm3"] > 0
            and item["fits_k1c_220x220x250"]
            and abs(item["bed_contact_z_min_mm"]) <= 0.01
        )
        results.append(item)

    report = {
        "criteria": (
            "watertight, consistent winding, expected component count, "
            "positive volume, K1C envelope, exported at Z=0"
        ),
        "results": results,
        "all_pass": bool(results) and all(x["pass"] for x in results),
    }
    if not args.check:
        (root / "docs" / "STL_VALIDATION.json").write_text(
            json.dumps(report, indent=2), encoding="utf-8"
        )
    print(json.dumps(report, indent=2))
    raise SystemExit(0 if report["all_pass"] else 1)


if __name__ == "__main__":
    main()
