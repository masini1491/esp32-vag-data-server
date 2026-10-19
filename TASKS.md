# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用 declared `masini1491/ai-development-playbook` baseline 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 與符合 scope 的 validation evidence 為準；成功驗證且完成 canonical reconciliation 後移除對應 unfinished item，不建立 Completed 區塊。

---

## VAG-UTF8-001 — Normalized VehicleData UTF-8／Web JSON 正確性修復
- **State:** Hot / Ready for separately authorized Codex launch（使用者已於 2026-10-19 選擇執行）。
- **Owner:** Codex 負責 bounded implementation、host regression 及必要的 `VALIDATION.md` evidence；ChatGPT 負責 Hot admission／完成後 canonical reconciliation。
- **Goal:** 確立 Normalized VehicleData 的文字欄位為 well-formed UTF-8（Unicode scalar values）且不接受 malformed sequences，避免其文字值經 Web JSON 輸出時產生不符合 UTF-8 的 JSON 位元組；不虛構實際外部 attack path。
- **Evidence baseline:** GitHub `main@efb8f51722fea288879c2f2c5c750a110ec21be8`；`src/core/vehicle_data.h` 的 `BoundedText::assign()` 目前檢查 C-string byte length / capacity 而未檢查 UTF-8；`src/application/web_vehicle_data_response.h` 的 `JsonWriter::appendQuoted()` 僅進行 JSON escaping，其他 bytes 直接輸出。BLE payload／Logger export 也會保留 normalized text bytes。此 SHA 是 evidence provenance，非執行 pin；Codex 須以 latest safe sync 核對。
- **Required contract / bounded implementation:**
  1. 遵守現有 brand-independent／header-only C++ core 與 byte-capacity 限制；在共用文字入口（優先評估 `BoundedText`）建立最小、deterministic、allocation-free UTF-8 well-formedness 驗證，使 sample signalId、unit、text value及 capability ID／unit 均套用一致有效的 UTF-8 契約。不可只讓 Web serializer 看似正確而讓非法文字留在 normalized model。不得自動修復／替代非法序列或加入 Unicode normalization；`Capacity` 仍指 bytes 而非 code points。
  2. 拒絕 stray continuation、truncated sequence、illegal lead、overlong forms、UTF-16 surrogate encoding、超過 U+10FFFF 與其他非 well-formed UTF-8；接受合法 ASCII（含需由 JSON escaping 處理的控制字元）與合法 2／3／4-byte Unicode scalar，含恰好達 byte capacity 的案例。保留目前 null、NUL-terminated C-string、overflow、clear/failure 與 caller-output failure atomicity 的語意；如原 contract衝突，STOP 說明而非默改 public semantics。
  3. 加入直接 `BoundedText`／`NormalizedVehicleValue::tryText`／`NormalizedVehicleSample::tryText/tryStatus` 與 Web serializer 的 deterministically reproduced negative/positive regressions；確認 malformed ingress被拒絕，已構造的有效 UTF-8 字串會以 byte-valid JSON 正確序列化，JSON escape、`requiredCapacity`、`InsufficientCapacity` 時 output 不變。做 bounded review 既有 BLE logical payload 與 Logger/export decode round-trip：有效多位元組字串 bytes 不得 drift；從外部 decoded text 引入非法序列須 fail-closed。只新增必要 regressions，不為此重構 serializer / export wire protocol。
  4. 若最小模型入口修正尚不足保證 Web public serializer contract，可在被證明必要的 Web validation 增加 scoped fail-closed 防護；避免 duplicate UTF-8 validators／大型字元處理框架。保留 `ReadOnlyGuard` diagnostic boundary、generic-platform separation與原有序列化欄位/schema。
- **Allowed implementation surface:** 依根因最低必要修改 `src/core/vehicle_data.h`、`src/application/web_vehicle_data_response.h`、`src/application/vehicle_data_export_record.h`、`src/application/ble_vehicle_data_payload.h` 及直接對應 `tests/host/*`；`VALIDATION.md` 僅在取得 scoped evidence 後補真實記錄。不要求同時修改列舉的所有檔案；偏好最少實際變更。
- **Exclusions:** README CI 概述整理、ESP32-C3、TWAI backend／firmware／hardware、車型 DID／mapping、Web UI 新功能、BLE runtime／GATT、Logger persistence、任意診斷 TX／coding／DTC clear、Phase 6c及其他 Cold items；不建立新 dependency、Unicode normalization framework、library split 或不相關 refactor。
- **Validation / evidence:** 先建立 pre-fix reproducer／現有契約證據，再針對本 Stage 進行最小 source fix 與充分 host C++ regression（涵蓋受影響核心／Web／BLE／export）；重跑完整既有 host suite 與 Node fixture suite，`git diff --check` PASS。若需要 CI confirmation，核對與實際 tested commit 相符的 GitHub Actions run；沒有執行就標示 NOT_RUN／Pending。ESP32 compile 若未重跑不得以舊 Phase 1 evidence 升格；Bench／Hardware／Vehicle 與 on-wire 仍 Pending，`VEHICLE_CONFIRMED = none`。Codex於 `VALIDATION.md` 記錄工具、指令、exact tested SHA、有效與負向回歸結果及保留的 evidence boundaries。
- **Completion / STOP:** 限此 UTF-8 contract correctness，確認不變更既有 binary wire/schema、安全 read-only 診斷政策與已保護語意；若需要跨模組架構重設、改變 public behavior、外部規格不確定或超出本 Stage，STOP 並給最小 evidence。經 current governance／permissions／required validation 允許時，可依本次 user launch 授權 scoped commit／non-force push；完成後以 GitHub canonical read-back 提交 evidence，`TASKS.md` completion bookkeeping 依 governance／ChatGPT reconciliation處理，不啟動其他工作。

---
