# Development Roadmap

- Phase 0: Repository / Architecture Freeze v0.4
- Phase 1: Board abstraction + HAL + basic TWAI
- Phase 1A-1: Host-testable HAL / CAN foundation（本輪；不含 TWAI backend）
- Phase 1A-2: Deterministic Mock CAN / Fake Clock / host tests（本輪；不含 ISO-TP）
- Phase 1A-3: ESP32-S3 Classic CAN / TWAI backend（本輪；不含 protocol behavior）
- Phase 1A-4: Foundation consolidation / Phase 1 implementation gate（本輪）
- Phase 2A: Host-testable Classic CAN ISO-TP / DiagnosticTransport boundary（PASS；後續 service/application layers 尚未開始）
- Phase 3: Generic OBD-II read-only（Phase 3A `ReadOnlyGuard`、Phase 3B PID/VIN host service 與 Phase 3C stored-DTC Mode `0x03` host slice PASS；其他 DTC modes／vehicle decoding 尚未開始）
- Phase 4: Generic UDS read-only（Phase 4A `ReadOnlyGuard` `0x22` safety extension、Phase 4B host service core 與 Phase 4C `0x19/0x02` raw DTC/status host slice PASS；其他 UDS service semantics 尚未開始）
- Phase 5A: normalized VehicleData sample/value semantics（PASS；僅 single-sample representation；Store/Cache 另見 Phase 5B）
- Phase 5B: bounded VehicleData Store / Cache（PASS；eviction/TTL/automatic aging 尚未開始；registry 另見 Phase 6b）
- Phase 5C: cooperative Scheduler core（PASS；Startup / Periodic / OnDemand，single active job；profile/application integration 尚未開始）
- Phase 6: VAG Brand Layer + Kamiq Profile
- Phase 6a: Profile identity + Active Profile manual selection boundary（host PASS；單一 Pending Kamiq 2024 target identity；automatic resolver、Brand runtime routing/mapping 尚未開始）
- Phase 6b: Capability registry + normalized signal registry（host PASS；fixed immutable metadata；Kamiq_NW4 四個 candidate 均 Pending；runtime support learning/mapping 尚未實作）
- Phase 6c: Deep Diagnostic on-demand path（Blocked；等待 EV-1 所述具體 Kamiq profile-owned route/DID/decode evidence；SOP 本身不解鎖 implementation）
- Application-facing VehicleData read model / snapshot API：host PASS；readonly Profile／capability／Store projection，無 diagnostic TX／Scheduler／client serialization。
- Phase 7: BLE
- Phase 8: Web
- Phase 9: Passive CAN
- Phase 10: Kamiq hardware/vehicle validation
- Phase 11: **Future / Pending** — T-Roc VAG portability validation
- Phase 12: **Future / Pending** — RAV4 cross-brand architecture validation
- Phase 13: **Future / Pending** — Wish Toyota cross-generation validation

EV-1 [Kamiq Read-only Evidence Acquisition Protocol](KAMIQ_READ_ONLY_EVIDENCE.md) is a docs-only research procedure. It acquires no vehicle evidence and does not change Bench / Hardware / Vehicle Pending status.

## Validation sequence

```text
Kamiq → T-Roc → RAV4 → Wish
```

### Kamiq — first Hardware / Vehicle PASS

Required minimum dataset：VIN、`vehicle.speed`、`vehicle.rpm`、`vehicle.coolantTemp`、`vehicle.voltage`。若取得，再加入 oil temperature、gear、wheel speeds、steering angle、ACC set speed。Status：`Pending`，直到取得真正的 Hardware / Vehicle evidence。

### T-Roc — same-brand portability（Future / Pending）

驗證 VAG Brand Layer + 不同 VAG Vehicle Profile 是否成立。理想上只需變更 VAG routing、profile 與 vehicle-specific CAN/DID/scaling，不大量修改 ISO-TP、OBD-II、UDS、VehicleData、BLE、Web 或 Logger。若需大幅修改 Generic Core，應視為 architecture feedback。

### RAV4 — cross-brand portability（Future / Pending）

驗證 Generic Core 是否可保留，只新增 Toyota Brand Layer + RAV4 Vehicle Profile；不預先實作 Toyota code。

### Wish — cross-generation portability（Future / Pending）

驗證 Toyota Brand Layer 能否透過不同 Vehicle Profile 支援另一世代；不假設 Wish 與 RAV4 使用相同 protocol 或 CAN layout。

## Architecture Portability PASS

條件是 Kamiq → VAG Profile、T-Roc → second VAG Profile、RAV4 → future Toyota Brand Layer + Profile 都能在不修改 Generic Core semantics 的情況下輸出共同 normalized VehicleData。實車完成前狀態維持 `Pending`。不同車款若無法提供某項 normalized signal，應回報 `unsupported` / `unavailable`，不以假值代替。

## Phase 1 foundation status

- Software PASS：Generic CAN model、Board Profile → HardwareConfig → HAL、Mock CAN、Fake Clock 與 ESP32-S3 TWAI backend 已完成。
- Host Test PASS：Stage 4T 已以 `clang++ -std=c++17 -Wall -Wextra -pedantic -I. tests/host/main.cpp` local revalidation，tested commit `0c699d6`；current evidence authority：`VALIDATION.md`。
- CAN foundation edge-case regression coverage 已建立。
- ESP32 Compile：PASS。Stage 4T 已以 Arduino CLI 1.5.1、Arduino-ESP32 3.3.11、ESP-IDF 5.5.5、`esp32:esp32:esp32s3` 與獨立 TEMP build path 驗證 `src/esp32_twai_can.cpp`，並確認 backend participation；tested commit `0c699d6`。詳見 `VALIDATION.md`。
- Bench PASS：Pending。
- Hardware PASS：Pending。
- Vehicle PASS：Pending。

這代表 Phase 1 software foundation components、Stage 4R / Stage 4T hardening、Stage 5 evidence consolidation、Phase 2A host-testable ISO-TP / DiagnosticTransport core、Phase 3A `ReadOnlyGuard` safety gate、Phase 3B Generic OBD-II PID/VIN host service core、Phase 3C stored-DTC Mode `0x03` host slice、Phase 4A UDS `0x22` guard extension、Phase 4B Generic UDS `ReadDataByIdentifier` host service core、Phase 4C Generic UDS `0x19/0x02` host slice、Phase 5A normalized VehicleData sample/value semantics，Phase 5B bounded VehicleData Store/Cache，以及 Phase 5C cooperative Scheduler core 已完成。Phase 3C 僅保存最多 16 筆 raw two-byte DTC record，忽略 `0x0000` padding，並對 malformed／overflow fail closed；不含 DTC decoding、UDS `0x19` 或 vehicle semantics。Phase 4B 僅涵蓋 one-request-at-a-time 的 `0x22` raw DID data、terminal negative NRC 與 bounded `0x78` response-pending；Phase 5A 僅涵蓋單一 brand-independent normalized sample/value representation、copy-safe opaque ID/text、metadata、availability/quality invariants 與 64-bit timestamp；Phase 5B 僅涵蓋 fixed-capacity latest-state ownership、same-ID timestamp ordering、whole-sample replacement 與 copy-based read/snapshot，不包含 eviction、TTL/automatic aging、registry、Scheduler、profile mapping 或 application-facing diagnostic TX。其他 OBD DTC modes、UDS services/session/其他 `0x19` subfunctions、TesterPresent、VAG routing 與 application-facing diagnostic TX 尚未開始。Phase 2A / 3A / 3B / 3C / 4A / 4B / 4C / 5A / 5B / 5C 均未重新執行 ESP32 compile，因 ESP32-facing source participation / platform boundary 未改變；這不代表實體 TWAI receive 或 vehicle validation 已完成。

Phase 4C 僅涵蓋 `19 02 <mask>`、`59 02 <availability>` 與最多 15 筆 raw DTC/status records；全零 DTC 不視為 padding。Malformed／overflow fail closed；共用既有 Busy、timeout、terminal NRC 與 bounded `0x78`。不含其他 subfunction、DTC 解碼、品牌／VehicleData mapping 或實體驗證；current evidence 見 `VALIDATION.md`。

Application read projection 已於 `654f4b3a6e2c80248ba3204d8b54d045f32e72f2` 建立並通過 host tests：顯示 current selection／optional identity、associated immutable capability descriptors 與 copied Store snapshot；缺少 registry 不猜測 Unsupported，容量不足保持 caller output unchanged。此層不觸發 polling／diagnostic TX，不實作 BLE／Web／Logger。ESP32 compile evidence 沿用 Phase 1 原 scope；physical evidence 仍 Pending。

Phase 5C 僅涵蓋 injected 64-bit Clock 的 cooperative timing/lifecycle kernel；Periodic 維持 absolute cadence 並跳過 missed periods，OnDemand pending/active request 合併，完成前只允許一個 active job。RealtimeTriggered、priority、callback/executor、automatic retry/backoff、Store aging/update、profile polling、FreeRTOS binding 與 application integration 尚未開始。

Phase 2 v1 implementation 是 ISO-TP over Classic CAN；future non-CAN transports 僅為 architecture boundaries，不新增 K-Line implementation phase，也不改變 Kamiq → T-Roc → RAV4 → Wish validation sequence。

Phase 6b 僅建立 separate capability-support state 與 fixed normalized signal metadata lookup；重用現有 SignalId/Unit，Kamiq_NW4 的 speed/rpm/coolantTemp/voltage 都是 Pending candidate，不宣稱實車 support。Registry 不擁有 sample availability、Store、mapping、polling、Scheduler 或 diagnostic execution。ESP32 compile 未重跑，因 platform boundary／ESP32-facing source participation 未改變；current host evidence 見 `VALIDATION.md`。

Phase 6a 已建立 brand-independent opaque profile identity、四種 selection states 與 fixed admitted identity set；VAG Profile Set 只保存 Pending 的 `Kamiq_NW4` target identity。Manual selection 不建立 capability 或 VehicleData values；Phase 6c、automatic detection、routes/DID/scaling/passive CAN mappings 與實車驗證仍待後續工作。ESP32 compile 未重跑，因 platform boundary 與 ESP32-facing source participation 未改變；current host evidence 見 `VALIDATION.md`。
