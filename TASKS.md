# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用最新版 `masini1491/ai-development-playbook` 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 為準；成功驗證後移除對應 unfinished item / Stage Prompt，不建立 Completed 區塊。

---
## HOT — Project AI mode + Evidence Staging migration

Status: Ready

Goal:
- 持久化 `Project AI mode: ChatGPT+Codex`。
- 保留既有 `TASKS.md` = Hot coordination、`BACKLOG.md` = Cold Registry 的既定語意，不重新設計。
- 啟用 sanitized Evidence Staging：`/evidence/inbox/*.md`。

Actor ownership:
- ChatGPT：planning／admission、Hot/Cold promotion、`TASKS.md`／`BACKLOG.md` 維護、後續 evidence staging 寫入、Codex completion reconciliation 與本 Stage 完成後的 TASKS cleanup。
- Codex：本 Stage 所需的 governance／supporting repository mutation。

Codex mutation scope:
- `/AGENTS.md`
- 建立最小 `/evidence/inbox/` staging surface（可用 `README.md` 定義用途）
- 不修改 `TASKS.md`、`BACKLOG.md`、`VALIDATION.md`、其他 docs、source、tests、tooling 或 workflow。

Required semantics:
- `AGENTS.md` 唯一有效宣告 `Project AI mode: ChatGPT+Codex`。
- ChatGPT Coordination Write Allowlist 保留 `/TASKS.md`、`/BACKLOG.md`，並新增 `/evidence/inbox/*.md`。
- Hot task dossier 仍不啟用。
- Evidence staging 不具 execution authority、ordinary bootstrap 預設不載入，且只允許 repo-safe / sanitized evidence；raw sensitive material 不得先進 Git。
- Evidence staging 不是 `VALIDATION.md` 或其他 canonical truth 的替代品；正式 validation 結論仍需 reconciliation 後由 canonical owner吸收。

Validation / completion:
- 依 current Playbook 與 project governance 完成必要 deterministic validation。
- `git diff --check` PASS。
- Adoption Doctor 維持 `RESULT PASS` / `DOCTOR_CLEAN YES`。
- 確認沒有不必要的 derived README／manifest／project-scale mutation。
- Commit / push `main` 後回報 final HEAD 與 changed files。
- Codex 不清理本 Stage 的 `TASKS.md`；由 ChatGPT canonical reconciliation 後移除。

STOP:
- 若啟用 evidence staging 必須修改上述 scope 以外檔案，或 current authority／derived dependency 出現 material conflict，停止並回報，不自行擴張。

---
