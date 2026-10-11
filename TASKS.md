# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用 declared `masini1491/ai-development-playbook` baseline 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 與符合 scope 的 validation evidence 為準；成功驗證且完成 canonical reconciliation 後移除對應 unfinished item，不建立 Completed 區塊。

---

## VAG-CI-001 — Node Web fixture CI coverage
- **State:** Hot / Ready for separately authorized Codex launch；前置 VAG-CORR-001 已完成 canonical reconciliation，尚未授權本 Stage 執行。
- **Owner:** Codex（另行授權的 CI workflow 修改與驗證）。
- **Goal:** 現有 `.github/workflows/host-tests.yml` 只執行 C++ host tests；將既有 `tests/web_fixture_tests.cjs` 納入 GitHub Actions，使用明確、適用的 Node 版本與最低充分步驟。
- **Scope / validation:** 不修改 Web runtime/UI 行為、不納入 ESP32 compile CI、不中途擴張其他工作；驗證 exact source candidate 的 C++ host tests、Node fixture tests 與對應 Actions run 成功；若 CI runner／service unavailable，明確記錄 Pending，不宣稱通過。
- **Launch boundary:** 本 item 是獨立 Stage；`VAG-CORR-001` 的 Codex Prompt 不授權它。使用者另行明確授權後，需重新核對 current repo／Playbook 與 Hot state，再執行本 Stage。

---
