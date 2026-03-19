# fixed_10bin 说明（严格图中规则）

本目录脚本基于 `../step_data.csv`，采用固定规则：
- `Dmax = 60 um`
- `Nevents = 10000`
- `N = 10`，`dz = Dmax / N`
- `LET_keV_per_um = E_sum_keV / (Nevents * dz)`
- `LET_pC_per_um = LET_keV_per_um * (1000/3.6*1.602176634e-7)`

## 运行
```bash
cd /home/ubuntu/BSE_ZGS/build/OutPut/fixed_10bin
/usr/bin/python3 process_fixed_10bin.py
```

## 输出
- `step_profile_10bin.csv`
  - `z_center_um`（入射深度, um）
  - `LET_pC_per_um`（LET, pC/um）
  - `LateralSpread_um`（固定 0.1 um）
  - `LET_keV_per_um`
- `bragg_let_10bin.png`
- `sentaurus_input_10bin_for_sentaurus.txt`
  - `z_center_um ...`
  - `LET_pC_per_um ...`
  - `LateralSpread_um ...`
