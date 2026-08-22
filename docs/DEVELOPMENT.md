# Development Roadmap

- Phase 0: Repository / Architecture Freeze v0.3
- Phase 1: Board abstraction + HAL + basic TWAI
- Phase 1A-1: Host-testable HAL / CAN foundation（本輪；不含 TWAI backend）
- Phase 2: ISO-TP / DiagnosticTransport boundary
- Phase 3: Generic OBD-II read-only
- Phase 4: Generic UDS read-only + ReadOnlyGuard
- Phase 5: VehicleData + Scheduler
- Phase 6: VAG Brand Layer + Kamiq Profile
- Phase 6a: Brand extension boundary + Brand Profile Set / Active Profile（future implementation planning）
- Phase 6b: Capability registry + normalized signal registry（future implementation planning）
- Phase 6c: Deep Diagnostic on-demand path（future implementation planning）
- Phase 7: BLE
- Phase 8: Web
- Phase 9: Passive CAN
- Phase 10: Kamiq hardware/vehicle validation
- Phase 11: **Future / Pending** — T-Roc VAG portability validation
- Phase 12: **Future / Pending** — RAV4 cross-brand architecture validation
- Phase 13: **Future / Pending** — Wish Toyota cross-generation validation

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
