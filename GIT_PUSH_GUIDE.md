# Git 后续推送说明（BSE_ZGS）

本文档用于说明：在你已经完成首次绑定远程仓库后，如何把后续修改持续推送到 GitHub。

当前仓库默认分支：`main`

## 1. 日常最短推送流程（推荐）

在项目根目录执行：

```bash
cd /home/ubuntu/BSE_ZGS
git status
git add .
git commit -m "你的更新说明"
git push
```

说明：
- `git status`：先看哪些文件改了。
- `git add .`：把当前改动加入暂存区。
- `git commit -m "..."`：生成一次本地提交。
- `git push`：推送到 `origin/main`。

---

## 2. 推送前先同步远程（更稳妥）

多人协作或跨设备开发时，建议先拉再推：

```bash
cd /home/ubuntu/BSE_ZGS
git pull --rebase origin main
git add .
git commit -m "你的更新说明"
git push origin main
```

---

## 3. 只推某几个文件

```bash
cd /home/ubuntu/BSE_ZGS
git add 路径/文件1 路径/文件2
git commit -m "只提交部分文件"
git push
```

示例：

```bash
git add build/OutPut/fixed_20bin/process_fixed_20bin.py build/OutPut/fixed_1bin/process_fixed_1bin.py
git commit -m "统一 20bin/1bin 脚本命名"
git push
```

---

## 4. 常见问题处理

### 4.1 `nothing to commit`
表示没有新改动，不需要提交。

可先检查：

```bash
git status
```

### 4.2 `rejected` / `non-fast-forward`
远程比本地新，先同步再推：

```bash
git pull --rebase origin main
git push origin main
```

### 4.3 提交信息写错（未 push 前）

```bash
git commit --amend -m "新的提交说明"
```

### 4.4 不小心把大文件加进来了
先取消暂存，再加入 `.gitignore`：

```bash
git restore --staged 路径/大文件
```

---

## 5. 建议的提交习惯

- 每完成一个小功能就提交一次。
- 提交信息尽量具体，例如：
  - `fix: fixed_1bin use z_um=60`
  - `feat: add ticurve plotting and integration`
  - `refactor: unify fixed_20bin naming`

---

## 6. 一条命令查看当前远程

```bash
git remote -v
```

你当前应看到：
- `origin  https://github.com/1320715736/BSE_ZGS.git (fetch)`
- `origin  https://github.com/1320715736/BSE_ZGS.git (push)`
