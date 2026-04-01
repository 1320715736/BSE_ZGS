# fixed_20bin 说明（按 fixed_5bin 逻辑，固定 20 bin）

本目录脚本基于 `../../step_data.csv`，采用固定规则：
- `Dmax = 60 um`
- `Nevents = 10000`
- `N = 20`，`dz = Dmax / N = 3 um`
- `LET_keV_per_um = E_sum_keV / (Nevents * dz)`
- `LET_pC_per_um = LET_keV_per_um * (1000/3.6*1.602176634e-7)`

## 运行
```bash
cd /home/ubuntu/BSE_XK/build/OutPut/fixed_20bin
/usr/bin/python3 process_fixed_20bin.py
```

## 输出
- `step_profile_20bin.csv`
  - `z_center_um`（入射深度, um）
  - `LET_pC_per_um`（LET, pC/um）
  - `LateralSpread_um`（固定 0.1 um）
  - `LET_keV_per_um`
- `bragg_let_20bin.png`
- `sentaurus_input_20bin_for_sentaurus.txt`
  - `z_center_um ...`
  - `LET_pC_per_um ...`
  - `LateralSpread_um ...`
