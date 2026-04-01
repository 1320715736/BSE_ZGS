#!/usr/bin/env python3
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


BIN_COUNT = 10
MAJORITY_QUANTILE = 0.95
NEVENTS = 10000
W_EH_EV = 3.6
E_CHARGE_C = 1.602176634e-19
KEV_TO_PC = (1000.0 / W_EH_EV) * E_CHARGE_C * 1e12
LATERAL_SPREAD_UM = 0.1


def estimate_dmax_um(df: pd.DataFrame) -> float:
    primary = df[
        (df["ParticleName"] == "e-")
        & (df["TrackID"] == 1)
        & (df["ParentID"] == 0)
        & (df["Depth_um"] >= 0.0)
    ]
    if primary.empty:
        return 60.0

    event_max_depth = primary.groupby("EventID")["Depth_um"].max()
    q_um = float(event_max_depth.quantile(MAJORITY_QUANTILE))
    return float(np.ceil(q_um))


def main() -> None:
    base_dir = Path(__file__).resolve().parent
    input_csv = base_dir.parent.parent / "step_data.csv"

    output_csv = base_dir / "step_profile_10bin.csv"
    output_png = base_dir / "bragg_let_10bin.png"
    output_txt = base_dir / "sentaurus_input_10bin_for_sentaurus.txt"

    df = pd.read_csv(
        input_csv,
        usecols=["EventID", "ParticleName", "TrackID", "ParentID", "Depth_um", "Edep_keV"],
    )
    df["EventID"] = pd.to_numeric(df["EventID"], errors="coerce")
    df["TrackID"] = pd.to_numeric(df["TrackID"], errors="coerce")
    df["ParentID"] = pd.to_numeric(df["ParentID"], errors="coerce")
    df["Depth_um"] = pd.to_numeric(df["Depth_um"], errors="coerce")
    df["Edep_keV"] = pd.to_numeric(df["Edep_keV"], errors="coerce")
    df = df.dropna(subset=["EventID", "TrackID", "ParentID", "Depth_um", "Edep_keV"])

    dmax_um = estimate_dmax_um(df)
    df = df[(df["Depth_um"] >= 0.0) & (df["Depth_um"] < dmax_um) & (df["Edep_keV"] >= 0.0)]

    dz = dmax_um / BIN_COUNT
    edges = np.linspace(0.0, dmax_um, BIN_COUNT + 1)
    df["bin"] = pd.cut(df["Depth_um"], bins=edges, right=False, include_lowest=True)

    grouped = (
        df.groupby("bin", observed=False)
        .agg(Edep_sum_keV=("Edep_keV", "sum"), Count=("Edep_keV", "size"))
        .reset_index()
    )

    grouped["z_left_um"] = grouped["bin"].map(lambda x: x.left).astype(float)
    grouped["z_right_um"] = grouped["bin"].map(lambda x: x.right).astype(float)
    grouped["z_center_um"] = 0.5 * (grouped["z_left_um"] + grouped["z_right_um"])
    grouped["bin_width_um"] = dz
    grouped["LET_keV_per_um"] = grouped["Edep_sum_keV"] / (NEVENTS * dz)
    grouped["LET_pC_per_um"] = grouped["LET_keV_per_um"] * KEV_TO_PC
    grouped["LateralSpread_um"] = LATERAL_SPREAD_UM

    out_cols = [
        "z_left_um",
        "z_right_um",
        "z_center_um",
        "bin_width_um",
        "Count",
        "Edep_sum_keV",
        "LET_keV_per_um",
        "LET_pC_per_um",
        "LateralSpread_um",
    ]
    grouped = grouped[out_cols]
    grouped.to_csv(output_csv, index=False)

    fig, axes = plt.subplots(2, 1, figsize=(9, 7), sharex=True)
    axes[0].plot(grouped["z_center_um"], grouped["LET_keV_per_um"], color="tab:blue", marker="o", lw=1.7)
    axes[0].set_ylabel("LET (keV/um)")
    axes[0].set_title(f"Image-rule Profile (N={BIN_COUNT}, Dmax={dmax_um} um, Nevents={NEVENTS})")
    axes[0].grid(alpha=0.3)

    axes[1].plot(grouped["z_center_um"], grouped["LET_pC_per_um"], color="tab:red", marker="o", lw=1.7)
    axes[1].set_xlabel("Depth (um)")
    axes[1].set_ylabel("LET (pC/um)")
    axes[1].grid(alpha=0.3)

    fig.tight_layout()
    fig.savefig(output_png, dpi=180)
    plt.close(fig)

    z_line = "z_center_um " + " ".join(f"{v:.12g}" for v in grouped["z_center_um"])
    let_line = "LET_pC_per_um " + " ".join(f"{v:.12g}" for v in grouped["LET_pC_per_um"])
    spread_line = "LateralSpread_um " + " ".join(f"{LATERAL_SPREAD_UM:.12g}" for _ in grouped["z_center_um"])
    output_txt.write_text(z_line + "\n" + let_line + "\n" + spread_line + "\n", encoding="utf-8")

    print(f"input: {input_csv}")
    print(f"csv: {output_csv}")
    print(f"png: {output_png}")
    print(f"txt: {output_txt}")
    print(f"Dmax_um(q{int(MAJORITY_QUANTILE*100)}): {dmax_um:.12g}")
    print(f"bins: {len(grouped)}")


if __name__ == "__main__":
    main()
