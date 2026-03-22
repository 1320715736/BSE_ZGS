#!/usr/bin/env python3
from pathlib import Path

import pandas as pd


BIN_COUNT = 1
DMAX_UM = 60.0
NEVENTS = 10000
W_EH_EV = 3.6
E_CHARGE_C = 1.602176634e-19
KEV_TO_PC = (1000.0 / W_EH_EV) * E_CHARGE_C * 1e12
LATERAL_SPREAD_UM = 0.1


def main() -> None:
    base_dir = Path(__file__).resolve().parent
    input_csv = base_dir.parent.parent / "step_data.csv"

    output_csv = base_dir / "step_profile_1bin.csv"
    output_txt = base_dir / "sentaurus_input_1bin_for_sentaurus.txt"

    use_cols = ["Depth_um", "Edep_keV"]
    df = pd.read_csv(input_csv, usecols=use_cols)

    for col in use_cols:
        df[col] = pd.to_numeric(df[col], errors="coerce")

    df = df.dropna(subset=use_cols)
    df = df[(df["Depth_um"] >= 0.0) & (df["Depth_um"] < DMAX_UM) & (df["Edep_keV"] >= 0.0)]

    edep_sum_keV = float(df["Edep_keV"].sum())
    dz = DMAX_UM / BIN_COUNT
    z_eq_um = DMAX_UM
    let_keV_per_um = edep_sum_keV / (NEVENTS * dz)
    let_pC_per_um = let_keV_per_um * KEV_TO_PC

    out_df = pd.DataFrame(
        [
            {
                "z_eq_um": z_eq_um,
                "LET_eq_pC_per_um": let_pC_per_um,
                "LET_eq_keV_per_um": let_keV_per_um,
                "E_sum_keV": edep_sum_keV,
                "Dmax_um": DMAX_UM,
                "Nevents": NEVENTS,
                "LateralSpread_um": LATERAL_SPREAD_UM,
            }
        ]
    )
    out_df.to_csv(output_csv, index=False)

    z_line = f"z_um {z_eq_um:.12g}"
    let_line = f"LET_pC_per_um {let_pC_per_um:.12g}"
    spread_line = f"LateralSpread_um {LATERAL_SPREAD_UM:.12g}"
    output_txt.write_text(z_line + "\n" + let_line + "\n" + spread_line + "\n", encoding="utf-8")

    print(f"input: {input_csv}")
    print(f"csv: {output_csv}")
    print(f"txt: {output_txt}")
    print(f"z_um={z_eq_um:.12g}")
    print(f"LET_eq_pC_per_um={let_pC_per_um:.12g}")


if __name__ == "__main__":
    main()
