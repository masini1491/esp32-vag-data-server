# Project Instructions

## Language

- Codex 工作回報使用繁體中文。
- Source code symbol / API / protocol name 保持原文；文件以繁體中文為主，必要技術名稱使用英文。

## Source of truth and Git safety

- GitHub `main` 是 source of truth；開始工作前依任務需要確認 `git status`、branch、HEAD。
- Working tree 若有來源不明修改，停止，不自行清除。
- 除非明確要求，不得 `reset --hard`、force push、discard unknown changes、rewrite history 或自行刪除 unknown files。

## Repository reading and evidence

採 progressive expansion：Level 0（diff/error/log/evidence）→ Level 1（direct symbol）→ Level 2（caller/callee）→ Level 3（完整相關檔案）→ Level 4（module/directory）→ Level 5（repository-wide）。只有上一級不足時才擴大。

Debug 流程：Evidence → Root Cause → Focused Patch → Targeted Validation。Root cause 分類為 `CONFIRMED ROOT CAUSE`、`HIGH-CONFIDENCE LIKELY ROOT CAUSE` 或 `INSUFFICIENT OBSERVABILITY`；後者先增加最小 diagnostics，不直接重構。

同一 root cause 最多 retry 一次；第二次失敗需重新分類為 SOURCE、TOOLCHAIN、ENVIRONMENT、INFRASTRUCTURE 或 SERVICE。Infrastructure/service error 重複兩次時停止 coding loop。

## Validation and hardware evidence

優先順序：static check → targeted verifier → targeted test → relevant build → required build matrix → full regression。明確區分 Software、Compile、Static/Test、Bench、Hardware、Network、Production PASS；沒有實體 evidence 的層級標記 Pending。

## Hardware abstraction

所有 GPIO 與 board-specific capability 必須集中於 `Board Profile → HardwareConfig → HAL`。CAN / ISO-TP / OBD / UDS / VAG / BLE / Web / application logic 不得硬編 GPIO number。

Generic Core 必須保持 brand-independent；VAG-specific routing、CAN ID、DID、scaling 與 semantics 只能存在於 Brand Layer / Vehicle Profile。Repository 維持 VAG-first；future brand support 需先有實際 evidence，不預先加入 speculative code。

## Read-only

不得引入 coding、adaptation、clear DTC、output tests、security access、flashing 或 actuator control。任何 diagnostic TX path 未來都必須經 `ReadOnlyGuard`。

## Reference reuse

實作 protocol / VAG 功能前先查看 `docs/REFERENCES.md` 的 upstream evidence；不得因「可能有關」重新大量 discovery。引用或重用 upstream source 前，先確認 license 與 provenance。
