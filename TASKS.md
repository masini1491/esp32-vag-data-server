# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — README current-state reconciliation

Status: Ready

Goal:
- 對 `README.md` 做 bounded current-state reconciliation，使 public-facing project status 與 current canonical `VALIDATION.md` / `CODEX_PROGRESS.md` / `docs/DEVELOPMENT.md` 一致。
- 只修正已確認的狀態漂移；不得把 README 擴成第二份 validation ledger，也不得改 architecture、source、tests、runtime、TASKS/BACKLOG semantics或任何 physical support claim。

Required corrections:
1. README 頂部「專案狀態」需加入已 canonical closure 的：
   - Phase 7 BLE host-side logical contract / mock PASS；
   - Phase 8 Web API/UI fixture path host PASS；
   - Logger / export host contract PASS。
2. Application-facing VehicleData read model 段落目前的「BLE／Web／Logger 仍未實作」已過時：
   - 改成 host-side consumer contracts 已完成；
   - 同時明確保留 real BLE stack/GATT/radio、real ESP32 Web/Wi-Fi runtime、persistent Logger/storage runtime 尚未實作。
3. 「目前開發狀態」的未完成清單不得再把 BLE / Web 整體描述成未開始：
   - 改為具體列出尚未完成的 physical/runtime integration surfaces；
   - Logger 同樣區分 host export contract PASS 與 persistence/storage runtime 未完成。
4. 保持以下 current truth 不變：
   - Kamiq 2024 仍不宣稱已支援；
   - Phase 6c 仍 evidence-blocked；
   - Bench / Hardware / Vehicle 仍 Pending；
   - `VEHICLE_CONFIRMED = none`；
   - real BLE/Web/storage physical/runtime evidence仍不存在；
   - read-only safety boundary不變。
5. README 保持 public-facing overview：
   - 不新增大段 exact validation ledger；
   - exact tested SHA / toolchain / command細節仍以 `VALIDATION.md` 為 authority；
   - 可保留必要 milestone pointer，但不要把 README 複製成 `VALIDATION.md` / `CODEX_PROGRESS.md`。
6. 不做 unrelated wording cleanup、architecture rewrite、roadmap重排或格式重構。

Validation / completion:
- compare final README claims against current `VALIDATION.md`, `CODEX_PROGRESS.md`, `docs/DEVELOPMENT.md`, `TASKS.md`, `BACKLOG.md`;
- `git diff --check` PASS；
- changed-file set應只有 `README.md`，除非 current canonical authority 明確顯示同一 docs-only correction必須同步另一個文件；若需要擴張，STOP並回報；
- create one minimum docs-only commit and push fast-forward to `origin/main`;
- remote read-back confirm final README content and SHA；
- do not remove this Hot Stage; ChatGPT will reconcile and clean it after remote verification.

Actor ownership:
- Codex: `README.md` mutation、docs-only validation、commit/push/read-back。
- ChatGPT: planning / reconciliation / Hot cleanup。

STOP:
- if README correction requires changing technical authority, source/tests, validation state, architecture, physical support claims or another non-README canonical owner, STOP instead of broadening scope.

---
