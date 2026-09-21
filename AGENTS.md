# Project Instructions

## Language

- Codex 工作回報使用繁體中文。
- Source code symbol / API / protocol name 保持原文；文件以繁體中文為主，必要技術名稱使用英文。

## AI Development Playbook adoption

本專案採用 `masini1491/ai-development-playbook` 作為共通 development baseline。
Playbook baseline: main
Project AI mode: ChatGPT+Codex

新 ChatGPT／AI／coding-agent session 的最小 bootstrap 順序為：

1. 先依本 project governance 與 current `TASKS.md` 確認 repository、work-state 與本地 authority。
2. 只有 activation 需要時，才 resolve 宣告 baseline 的 immutable revision、讀取其 `CHAT_INIT.md`，並依 router 讀取最低充分 canonical owner。

Project-specific governance 與 technical source of truth 高於 common Playbook。採用 Playbook 本身不會新增或擴張 ChatGPT、Codex 或其他 agent 的 repository write、execution、deployment、credential、secret 或 external-service authority。

- Canonical technical source(s): `docs/ARCHITECTURE.md`; `VALIDATION.md`; `src/`; `tests/`; this `AGENTS.md`
- Current coordination surface: `TASKS.md`
- Required validation: `git diff --check`; current `TASKS.md` / `VALIDATION.md` requirements; Bench／Hardware／Vehicle evidence remains Pending without physical evidence
- Project-specific exceptions or restrictions: GitHub `main` source-of-truth with clean fast-forward-only sync; ChatGPT Coordination Write Allowlist `/TASKS.md`, `/BACKLOG.md` and sanitized `/evidence/inbox/*.md`; read-only diagnostic policy; physical evidence cannot be inferred from software or compile evidence

## Authority boundary

Project-specific authority remains with this `AGENTS.md`, the declared technical sources, and the current Hot coordination surface `TASKS.md`; `BACKLOG.md` remains a Cold Registry without execution authority.

## Source of truth and Git safety

- GitHub `main` 是 source of truth；本 repository 僅接受 clean、fast-forward-only 的 remote sync。
- Generic repository identity、Git procedure、permission recovery 與 remote operation gates 依 shared `REPOSITORY_EXECUTION.md`。

## TASKS.md shared queue

- `TASKS.md` 是本 repository 唯一的 Hot/current executable-work coordination surface；`BACKLOG.md` 是 Cold Registry，不具 execution authority。
- 本 repository 的 queue cleanup requirement 以本節為準；Hot／Cold lifecycle、admission 與 prompt semantics 依 shared `AI_CONTEXT.md`、`REPOSITORY_EXECUTION.md` 與 `CHATGPT_WORKFLOW.md`。

## ChatGPT Coordination Write Allowlist

- ChatGPT direct-write coordination allowlist 包含 `/TASKS.md`、`/BACKLOG.md` 與 sanitized `/evidence/inbox/*.md`。未列入 path（包括 `AGENTS.md`、README/docs、source、tests、tooling、workflow 與 validation authority）對 ChatGPT 仍為 read-only。
- Hot task dossier 仍不啟用。`/evidence/inbox/*.md` 僅可保存 repo-safe、sanitized evidence，且不具 execution authority；詳細 evidence-staging lifecycle、reconciliation 與 sanitization method 依 shared `AI_CONTEXT.md` 與 `REPOSITORY_EXECUTION.md`。

## Library-ready design

- Generic protocol/data layers 維持未來可抽離 reusable library 的依賴方向，但目前不得為 library 化提前拆 repository、增加 speculative abstraction、package/release infrastructure 或沒有實際 consumer 的 generic API。
- Core、transport abstraction、ISO-TP、OBD、UDS、ReadOnlyGuard、generic VehicleData 與 profile interfaces 不得依賴 Arduino、ESP32/TWAI、Web、BLE、Wi-Fi 或特定品牌／車型實作。
- ESP32／Arduino-specific code 留在 platform／HAL／firmware boundary；Brand-specific code 不得反向滲入 Generic Core。
- UI、network、storage clients 依賴 VehicleData／application-facing interface；核心協議不得反向依賴 client。避免不必要 global mutable state 與 platform singleton，以維持 host testing／dependency injection 能力。
- 等 ISO-TP → OBD／UDS → Brand Layer → Vehicle Profile → VehicleData 的實際資料流穩定後，再評估 library extraction；目前不要建立 `library.properties`、package、另一個 repository、semantic versioning 或未使用的抽象層。

## Repository file roles and update thresholds

- `AGENTS.md` 是永久工作規則。
- `TASKS.md` 是唯一 active unfinished work／executable scoped Prompt queue。
- `CODEX_PROGRESS.md` 是 human-readable 歷史／project-state 摘要，不是 active queue。
- `VALIDATION.md` 是 validation contract、evidence 與 current Pending authority，不是 task queue。
- `docs/DEVELOPMENT.md` 是 roadmap／phase definitions，不作詳細 validation evidence ledger。
- `CHANGELOG.md` 是 release／change summary，不作 active queue 或 validation authority。

## Validation and hardware evidence

Validation ladder、evidence lifecycle 與 completion method 依 shared `DEBUG_VALIDATION.md`。本 repository-specific rule：沒有實體 evidence 的 Bench、Hardware、Vehicle 層級一律標記 Pending，不得由 software／compile evidence 推導。

## Hardware abstraction

所有 GPIO 與 board-specific capability 必須集中於 `Board Profile → HardwareConfig → HAL`。CAN / ISO-TP / OBD / UDS / VAG / BLE / Web / application logic 不得硬編 GPIO number。

Generic Core 必須保持 brand-independent；VAG-specific routing、CAN ID、DID、scaling 與 semantics 只能存在於 Brand Layer / Vehicle Profile。Repository 維持 VAG-first；future brand support 需先有實際 evidence，不預先加入 speculative code。

單一 firmware build 不要求包含所有品牌；Brand Profile Set、Profile Resolver、Active Vehicle Profile 與 capability state 是 architecture boundaries，不在文件階段建立 implementation。Optional / Deep Diagnostic capability 必須顯式標示 `unsupported`、`unavailable` 或 `pending` / `unknown`，不得以假值代替；Deep Diagnostic 預設 on-demand，所有 diagnostic TX 永遠經 `ReadOnlyGuard`。

Diagnostic/application upper layers 不得假設所有 vehicle link 都是 CAN，亦不得直接依賴 CAN/TWAI-specific types；ISO-TP over Classic CAN 是 v1 concrete path，不是唯一 future transport。Future non-CAN evidence 不授權自行實作 K-Line 或 motorcycle support，且任何 non-CAN diagnostic TX 同樣不得繞過 `ReadOnlyGuard`。

## Read-only

不得引入 coding、adaptation、clear DTC、output tests、security access、flashing 或 actuator control。任何 diagnostic TX path 未來都必須經 `ReadOnlyGuard`。

## Reference reuse

實作 protocol / VAG 功能前先讀 `docs/references/SYNTHESIS.md`，再依 `docs/REFERENCES.md` 的 phase-specific reading set 讀必要 note；不要預設讀完整 reference library。除非符合 local Revisit trigger，禁止因「可能有關」重新 discovery upstream。引用或重用 upstream source 前，先確認 license 與 provenance。
