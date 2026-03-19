#!/usr/bin/env python3
from __future__ import annotations

import re
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def _curve_name_from_header(x_header: str, index: int) -> str:
    match = re.search(r"\(([^)]+)\)\s*X$", x_header)
    if match:
        return match.group(1)
    return f"curve_{index + 1}"


def main() -> None:
    base_dir = Path(__file__).resolve().parent
    input_csv = base_dir.parent / "ticurve.csv"
    output_png = base_dir / "ticurve_4curves_with_integral.png"
    output_csv = base_dir / "ticurve_integrals.csv"

    if not input_csv.exists():
        raise FileNotFoundError(f"未找到输入文件: {input_csv}")

    df = pd.read_csv(input_csv)
    columns = list(df.columns)

    if len(columns) < 8 or len(columns) % 2 != 0:
        raise ValueError("CSV 列数异常，预期至少4组 X/Y 成对列")

    plt.figure(figsize=(10, 6))
    area_rows: list[dict[str, float | str]] = []

    for idx in range(0, len(columns), 2):
        x_col = columns[idx]
        y_col = columns[idx + 1]

        pair_df = df[[x_col, y_col]].copy()
        pair_df[x_col] = pd.to_numeric(pair_df[x_col], errors="coerce")
        pair_df[y_col] = pd.to_numeric(pair_df[y_col], errors="coerce")
        pair_df = pair_df.dropna()

        if pair_df.empty:
            continue

        pair_df = pair_df.sort_values(by=x_col)
        x = pair_df[x_col].to_numpy(dtype=float)
        y = pair_df[y_col].to_numpy(dtype=float)

        if x.size < 2:
            continue

        name = _curve_name_from_header(x_col, idx // 2)
        area = float(np.trapezoid(y, x))

        area_rows.append(
            {
                "curve": name,
                "x_column": x_col,
                "y_column": y_col,
                "area": area,
            }
        )

        plt.plot(x, y, linewidth=1.8, label=f"{name}, area={area:.4e}")

    if not area_rows:
        raise RuntimeError("未解析到可绘制/积分的曲线")

    plt.xscale("log")
    plt.xlabel("Time (s)")
    plt.ylabel("Current (A)")
    plt.title("TI Curves (4 Curves) with Integrated Area")
    plt.grid(True, which="both", linestyle="--", alpha=0.35)
    plt.legend()
    plt.tight_layout()
    plt.savefig(output_png, dpi=300)

    pd.DataFrame(area_rows).to_csv(output_csv, index=False)

    print(f"input: {input_csv}")
    print(f"png: {output_png}")
    print(f"csv: {output_csv}")
    for row in area_rows:
        print(f"{row['curve']}: {row['area']:.12g}")


if __name__ == "__main__":
    main()
