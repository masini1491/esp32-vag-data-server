# TASKS

本檔案是本 repository 唯一的 Hot/current executable-work / critical-path coordination surface；只保留目前 Hot／critical-path 的 TODO、Blocked 與 Pending-validation 工作。Cold／future-trigger durable memory 改由 `BACKLOG.md` 的 Cold Registry 承擔，不具 execution authority。

執行任何 Task / Stage 前，先讀最新 `AGENTS.md`，依其 routing 使用 declared `masini1491/ai-development-playbook` baseline 的最低必要章節；Git safety、permission gates、progressive reading、failure taxonomy、model / Context / Agent discipline、validation ladder、evidence reuse 與 Completion Evidence Guard 不在本檔重複維護。

`TASKS.md` 本身不授權 Codex 自動執行其他 Stage。完成工作以 Git history 與符合 scope 的 validation evidence 為準；成功驗證且完成 canonical reconciliation 後移除對應 unfinished item，不建立 Completed 區塊。

---

## VAG-CORR-001 — ISO-TP／OBD 邊界正確性修復
- **State:** Hot / Pending canonical reconciliation by ChatGPT：implementation 與 local validation 已完成於 `27b9c55da0578d42288ca4d111fef4ae672fee20`；scoped evidence 見 `VALIDATION.md`。本 entry 不授權重做 implementation 或啟動 VAG-CI-001；canonical reconciliation 完成前不移除。
- **Owner:** Codex（source、host regression、必要的 `VALIDATION.md` evidence）；ChatGPT 負責 coordination admission 與完成後 canonical reconciliation。
- **Goal:** 針對 2026-10-11 focused correctness 健檢中五項已重現／經 source 覆核的缺陷，做最低充分 root-cause 修補與 deterministic regression，保留既有 read-only diagnostic boundary。
- **Evidence baseline:** 健檢及 GitHub source review 針對 `main@03b751ed21322623098b5cfea67b962983bcbd94`；此 SHA 僅為歷史 evidence，不是 execution pin，Codex 執行前須依 current repo governance fresh-sync / reconcile。涉及 `src/protocol/isotp_transport.h`、`src/protocol/obd_service.h`、`src/core/can_types.h` 與相應 `tests/host/*`。
- **Required corrections / regression:**
  1. **Oversized diagnostic response recovery（P1）：** UDS（並覆核相同 OBD failure path）收到完成的 65-byte ISO-TP 訊息、service buffer 只有 64 bytes 時，terminal `Overflow` 不得遺留 `rxMessageReady_` 導致下一次合法訊息 `Busy`。建立最低充分、明確作用於當前 pending message 的 discard/recovery 契約，服務終止或拒收時確實清理；**保留**一般 caller 對 `receive()` 因 buffer 太小而 `Overflow` 後可以換大 buffer 重試的現有契約。不得清空其他 CAN traffic／CAN HAL FIFO；測試 overflow → 下一個合法 request/response 恢復，以及原本 retry contract。
  2. **OBD invalid request must not cancel active request（P2）：** `requestCurrentData(0x0C)` active 時收到 `requestSupportedPids(0x01)`，新請求可拒絕但不得將原交易變 idle、改寫其 request identity／deadline、清除應有 result 或吞掉原本正常回應。先檢查 active/busy 再處理新輸入合法性；做 active-state 及 response regression，並 bounded review 其他同型 OBD／UDS entry paths。
  3. **ISO-TP cross-block STmin（P2）：** `BS=1, STmin=5ms` 下，新 FlowControl CTS 不得將相鄰兩個 CF 間隔縮短至 0ms；明確記錄前一個成功交給 HAL 的 CF 之適用時間基準，跨 block 保證最小間隔，同時保留第一次 FC 後的首個 CF 與 `TxBusy` retry 正確性。修正既有錯誤 regression expectation，新增時序邊界測試。Host 時序 PASS 不宣稱 on-wire STmin PASS。
  4. **WAIT overflow（P2）：** `maxWaitFrames=255` 在第 256 個 WAIT 應回傳 `WaitLimitExceeded`，不得因 `uint8_t` 回繞繼續 `InProgress`；覆核較小界值。
  5. **CAN frame format validation（P3）：** `CanFrame::isValid()` 僅接受 Standard／Extended；拒絕 `static_cast<CanFrameFormat>(2)` 等非法 enum，維持現有 CAN-ID／length boundary。
- **Scope exclusions:** 不處理 ESP32-C3 portability、TJA1051/車用供電、CAN Hardware/Vehicle 測試、Kamiq profile／DID/scaling、BLE/Web runtime、UI、新協議功能、任意 TX／coding／adaptation／clear DTC 或其他 Cold work。不得為此重構 Generic Core／建立未使用 framework。
- **Validation / evidence:** 新增上述 failure→recovery regression，完整現有 host C++ tests 與 `git diff --check` PASS；如 ESP32-facing participation 未變則記錄 ESP32 compile 是否需重跑及理由，未執行不可報 PASS。更新 `VALIDATION.md` 為 truthful scoped evidence（由 Codex 執行）；Bench／Hardware／Vehicle 維持 Pending、`VEHICLE_CONFIRMED = none`。報告 exact tested SHA、command、結果及任何限制。
- **Completion / STOP:** 本 Stage 只完成五項 source/test correctness 修補與必要 evidence；不執行下一個 CI Stage。發現 architecture、transport ownership、ReadOnlyGuard bypass、相依根因或新增檔案超出必要 scope，先 STOP 並回報最小 evidence。Commit/push 只在本 Stage 的 launch 明確授權且 current governance/permissions/validation 符合時進行；不可藉 Stage 本身推定已完成或有跨 Stage 權限。

## VAG-CI-001 — Node Web fixture CI coverage
- **State:** Hot / Blocked：等待 `VAG-CORR-001` 完成 canonical reconciliation 後，另行啟動。
- **Owner:** Codex（另行授權的 CI workflow 修改與驗證）。
- **Goal:** 現有 `.github/workflows/host-tests.yml` 只執行 C++ host tests；將既有 `tests/web_fixture_tests.cjs` 納入 GitHub Actions，使用明確、適用的 Node 版本與最低充分步驟。
- **Scope / validation:** 不修改 Web runtime/UI 行為、不納入 ESP32 compile CI、不中途擴張其他工作；驗證 exact source candidate 的 C++ host tests、Node fixture tests 與對應 Actions run 成功；若 CI runner／service unavailable，明確記錄 Pending，不宣稱通過。
- **Launch boundary:** 本 item 是獨立 Stage；`VAG-CORR-001` 的 Codex Prompt 不授權它。需重新核對 current repo／Playbook 與 Hot state，再形成下一次授權。

---
