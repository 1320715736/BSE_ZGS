# fixed_1bin 说明（按图中规则，固定 1 bin）

本目录脚本基于 `../../step_data.csv`，采用固定规则：
- `Dmax = 60 um`
- `Nevents = 10000`
- `N = 1`，`dz = 60 um`
- `LET_keV_per_um = E_sum_keV / (Nevents * dz)`
- `LET_pC_per_um = LET_keV_per_um * (1000/3.6*1.602176634e-7)`

## 运行
```bash
cd /home/ubuntu/BSE_ZGS/build/OutPut/fixed_1bin
/usr/bin/python3 process_fixed_1bin.py
```

## 输出
- `step_profile_1bin.csv`
  - `z_eq_um`
  - `LET_eq_pC_per_um`
  - `LET_eq_keV_per_um`
  - `E_sum_keV`
  - `Dmax_um`, `Nevents`
  - `LateralSpread_um`
- `sentaurus_input_1bin_for_sentaurus.txt`
  - `z_um <value>`
  - `LET_pC_per_um <value>`
  - `LateralSpread_um <value>`
