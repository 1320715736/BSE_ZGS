#!/usr/bin/env python3
"""
Calculate LET in pairs/cm^3 with real lateral spread from step data.
Output format for Sentaurus: z_center_cm LET_pairs_cm3 LateralSpread_cm
All output units in cm for Sentaurus compatibility.
"""
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


BIN_COUNT = 10
DMAX_UM = 60.0
NEVENTS = 10000
W_EH_EV = 3.6  # Energy to create one e-h pair in Si (eV)


def main() -> None:
    base_dir = Path(__file__).resolve().parent
    input_csv = base_dir.parent.parent / "step_data.csv"

    output_csv = base_dir / "step_profile_10bin.csv"
    output_png = base_dir / "bragg_let_10bin.png"
    output_txt = base_dir / "sentaurus_input_10bin_for_sentaurus.txt"

    df = pd.read_csv(input_csv, usecols=[
        "Depth_um", "Edep_keV",
        "PreX_mm", "PreY_mm", "PostX_mm", "PostY_mm"
    ])

    df["X_um"] = (df["PreX_mm"] + df["PostX_mm"]) / 2.0 * 1000.0
    df["Y_um"] = (df["PreY_mm"] + df["PostY_mm"]) / 2.0 * 1000.0

    df["Depth_um"] = pd.to_numeric(df["Depth_um"], errors="coerce")
    df["Edep_keV"] = pd.to_numeric(df["Edep_keV"], errors="coerce")
    df = df.dropna(subset=["Depth_um", "Edep_keV"])
    df = df[(df["Depth_um"] >= 0.0) & (df["Depth_um"] < DMAX_UM) & (df["Edep_keV"] >= 0.0)]

    dz_um = DMAX_UM / BIN_COUNT
    dz_cm = dz_um * 1e-4
    edges = np.linspace(0.0, DMAX_UM, BIN_COUNT + 1)
    df["bin"] = pd.cut(df["Depth_um"], bins=edges, right=False, include_lowest=True)

    # Custom aggregation function for energy-weighted variance
    def calc_energy_weighted_spread(group):
        E = group["Edep_keV"].values
        X = group["X_um"].values
        Y = group["Y_um"].values
        E_sum = E.sum()

        if E_sum == 0 or len(E) < 2:
            return pd.Series({
                "Edep_sum_keV": E_sum,
                "Count": len(E),
                "LateralSpread_um": 0.1
            })

        # Step 1: Energy-weighted centroid
        X_bar = np.sum(E * X) / E_sum
        Y_bar = np.sum(E * Y) / E_sum

        # Step 2: Energy-weighted variance
        sigma_X_sq = np.sum(E * (X - X_bar)**2) / E_sum
        sigma_Y_sq = np.sum(E * (Y - Y_bar)**2) / E_sum

        # Step 3: Combined lateral spread (1σ radius)
        w_um = np.sqrt(sigma_X_sq + sigma_Y_sq)

        return pd.Series({
            "Edep_sum_keV": E_sum,
            "Count": len(E),
            "LateralSpread_um": w_um
        })

    grouped = df.groupby("bin", observed=False).apply(calc_energy_weighted_spread).reset_index()

    grouped["z_left_um"] = grouped["bin"].map(lambda x: x.left).astype(float)
    grouped["z_right_um"] = grouped["bin"].map(lambda x: x.right).astype(float)
    grouped["z_center_um"] = 0.5 * (grouped["z_left_um"] + grouped["z_right_um"])
    grouped["z_center_cm"] = grouped["z_center_um"] * 1e-4
    grouped["bin_width_um"] = dz_um

    grouped["LateralSpread_um"] = grouped["LateralSpread_um"].clip(lower=0.1)
    grouped["LateralSpread_cm"] = grouped["LateralSpread_um"] * 1e-4

    # Calculate LET in keV/um
    grouped["LET_keV_per_um"] = grouped["Edep_sum_keV"] / (NEVENTS * dz_um)

    # Calculate LET in pairs/cm^3 (volume concentration)
    # Formula: C_max = 8.84e13 * E_L / w^2
    # where E_L is LET (keV/um), w is lateral spread (um)
    grouped["LET_pairs_cm3"] = 8.84e13 * grouped["LET_keV_per_um"] / (grouped["LateralSpread_um"] ** 2)

    out_cols = [
        "z_left_um", "z_right_um", "z_center_um", "z_center_cm", "bin_width_um",
        "Count", "Edep_sum_keV", "LET_keV_per_um",
        "LET_pairs_cm3", "LateralSpread_um", "LateralSpread_cm",
    ]
    grouped = grouped[out_cols]
    grouped.to_csv(output_csv, index=False)

    fig, axes = plt.subplots(2, 1, figsize=(9, 7), sharex=True)
    axes[0].plot(grouped["z_center_cm"], grouped["LET_pairs_cm3"], color="tab:blue", marker="o", lw=1.7)
    axes[0].set_ylabel("LET (pairs/cm³)")
    axes[0].set_title(f"LET Profile (N={BIN_COUNT}, Dmax={DMAX_UM} um, Nevents={NEVENTS})")
    axes[0].grid(alpha=0.3)
    axes[0].ticklabel_format(style='scientific', axis='y', scilimits=(0,0))

    axes[1].plot(grouped["z_center_cm"], grouped["LateralSpread_cm"], color="tab:green", marker="s", lw=1.7)
    axes[1].set_xlabel("Depth (cm)")
    axes[1].set_ylabel("Lateral Spread (cm)")
    axes[1].grid(alpha=0.3)

    fig.tight_layout()
    fig.savefig(output_png, dpi=180)
    plt.close(fig)

    # Output for Sentaurus (all units: cm)
    z_line = "z_center_cm " + " ".join(f"{v:.12g}" for v in grouped["z_center_cm"])
    let_line = "LET_pairs_cm3 " + " ".join(f"{v:.12g}" for v in grouped["LET_pairs_cm3"])
    spread_line = "LateralSpread_cm " + " ".join(f"{v:.12g}" for v in grouped["LateralSpread_cm"])
    output_txt.write_text(z_line + "\n" + let_line + "\n" + spread_line + "\n", encoding="utf-8")

    print(f"Input: {input_csv}")
    print(f"CSV: {output_csv}")
    print(f"PNG: {output_png}")
    print(f"TXT: {output_txt}")
    print(f"Bins: {len(grouped)}")


if __name__ == "__main__":
    main()
