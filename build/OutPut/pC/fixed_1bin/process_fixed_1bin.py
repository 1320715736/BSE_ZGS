#!/usr/bin/env python3
import math
from pathlib import Path

import pandas as pd


BIN_COUNT = 1
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
    return float(math.ceil(q_um))


def main() -> None:
    base_dir = Path(__file__).resolve().parent
    input_csv = base_dir.parent.parent / "step_data.csv"

    output_csv = base_dir / "step_profile_1bin.csv"
    output_txt = base_dir / "sentaurus_input_1bin_for_sentaurus.txt"

    use_cols = ["EventID", "ParticleName", "TrackID", "ParentID", "Depth_um", "Edep_keV"]
    df = pd.read_csv(input_csv, usecols=use_cols)

    for col in ["EventID", "TrackID", "ParentID", "Depth_um", "Edep_keV"]:
        df[col] = pd.to_numeric(df[col], errors="coerce")

    df = df.dropna(subset=["EventID", "TrackID", "ParentID", "Depth_um", "Edep_keV"])
    dmax_um = estimate_dmax_um(df)
    df = df[(df["Depth_um"] >= 0.0) & (df["Depth_um"] < dmax_um) & (df["Edep_keV"] >= 0.0)]

    edep_sum_keV = float(df["Edep_keV"].sum())
    dz = dmax_um / BIN_COUNT
    # 1bin 为特殊输出：深度点采用“多数电子上限值”
    z_center_um = dmax_um
    let_keV_per_um = edep_sum_keV / (NEVENTS * dz)
    let_pC_per_um = let_keV_per_um * KEV_TO_PC

    out_df = pd.DataFrame(
        [
            {
                "z_center_um": z_center_um,
                "LET_pC_per_um": let_pC_per_um,
                "LET_keV_per_um": let_keV_per_um,
                "E_sum_keV": edep_sum_keV,
                "Dmax_um": dmax_um,
                "Nevents": NEVENTS,
                "LateralSpread_um": LATERAL_SPREAD_UM,
            }
        ]
    )
    out_df.to_csv(output_csv, index=False)

    z_line = f"z_center_um {z_center_um:.12g}"
    let_line = f"LET_pC_per_um {let_pC_per_um:.12g}"
    spread_line = f"LateralSpread_um {LATERAL_SPREAD_UM:.12g}"
    output_txt.write_text(z_line + "\n" + let_line + "\n" + spread_line + "\n", encoding="utf-8")

    print(f"input: {input_csv}")
    print(f"csv: {output_csv}")
    print(f"txt: {output_txt}")
    print(f"Dmax_um(q{int(MAJORITY_QUANTILE*100)}): {dmax_um:.12g}")
    print(f"z_center_um={z_center_um:.12g}")
    print(f"LET_pC_per_um={let_pC_per_um:.12g}")


if __name__ == "__main__":
    main()
