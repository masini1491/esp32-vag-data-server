# Phase 3 OBD-II readiness / ReadOnlyGuard ordering — 2026-09-22

Status: COMPLETE
Purpose: Phase 3 readiness evidence only; no execution authority.

## Current authority / baseline

- Target repository baseline reviewed: `aaaf5b3ad965f36333d714af41c03b53503f9974`
- Shared Playbook reviewed: `masini1491/ai-development-playbook@53158c560333bd8f701ceced77f998fcc4a6d093`
- Current Hot state before this review: `TASKS.md = EMPTY`
- Phase 2A host-testable Classic CAN ISO-TP / `DiagnosticTransport` core is complete.

## Project-native constraints

Current project authority requires:
- all active diagnostic TX to pass through `ReadOnlyGuard` / diagnostic policy gate;
- no arbitrary raw diagnostic TX bypass from application/Web/BLE/API;
- no coding, adaptation, Clear DTC, SecurityAccess, output tests, flashing, actuator control or other write/control behavior;
- OBD/UDS/application layers remain independent of CAN/TWAI-specific implementation types.

The existing Cold trigger `ReadOnlyGuard ordering before live diagnostic service integration` is now satisfied because the next roadmap responsibility is Generic OBD-II and any real OBD request would create an active diagnostic TX path.

## Phase 3 readiness findings

### 1. Existing Phase 2A boundary is sufficient as the lower transport dependency

`DiagnosticTransport` already provides the minimum lower-layer operations needed by a future service client:
- start outbound diagnostic payload;
- deterministic poll/progress;
- receive completed diagnostic payload;
- transport-level status propagation.

`IsoTpTransport` remains responsible for ISO-TP framing/state and CAN IDs. OBD-II service semantics should not know CAN/TWAI frame details.

### 2. ReadOnlyGuard must precede OBD request implementation

Do not first build an OBD client against raw `DiagnosticTransport` and retrofit policy later. The minimum safe dependency direction is:

`OBD service client → ReadOnlyGuard → DiagnosticTransport → ISO-TP`

The raw transport remains an internal lower-layer primitive. Future service/application code must depend on the guard boundary for outbound requests.

### 3. Minimum Phase 3A guard profile

The first guard slice should be deliberately narrow:
- support outbound OBD single-PID read requests only;
- initially allow OBD Mode `0x01` (current data) and Mode `0x09` (vehicle information);
- reject all other OBD modes fail-closed, including Mode `0x04`;
- do not add UDS policy yet;
- do not add OBD DTC-read modes yet even though future read-only DTC support may be legitimate;
- do not expose arbitrary raw diagnostic payload TX through the guard.

A minimum API may accept semantic OBD request inputs such as `mode + pid`, construct the two-byte service payload internally, then delegate to `DiagnosticTransport`. Exact class/method spelling is implementation detail, but denial must be distinguishable from malformed input and lower transport failure.

Allowed outbound examples under this first profile:
- `01 00` — supported Mode 01 PID discovery root;
- `01 <pid>` — one Mode 01 current-data PID;
- `09 02` — VIN request.

Denied examples under this first profile:
- Mode `04` and every mode not explicitly allowlisted;
- raw/multi-service arbitrary diagnostic payload;
- UDS services;
- multi-PID OBD requests.

### 4. Public OBD protocol recheck

A bounded public recheck was done because local evidence did not fully settle the exact first OBD service slice.

Sources:
- CSS Electronics OBD2 overview: https://www.csselectronics.com/pages/obd2-explained-simple-intro
- CSS Electronics OBD2 PID table: https://www.csselectronics.com/pages/obd2-pid-table-on-board-diagnostics-j1979

Relevant public observations:
- Mode `0x01` is used for current data; PID `0x00` reports supported PIDs `0x01–0x20`, with `0x20/0x40/...` continuing support discovery.
- Positive OBD responses use the request mode plus `0x40` (for example Mode `01` → response `41`).
- Mode `0x09`, PID `0x02` is used for VIN and can produce a multi-frame response.
- These observations are implementation aids only; project read-only policy remains the authority for what this repository may transmit.

No third-party source code is copied by this evidence note.

## Proposed Stage split

### Phase 3A — ReadOnlyGuard safety gate

Implement only the outbound policy boundary and deterministic host tests.

Required behavior:
- distinct `ReadOnlyGuard`-level dependency in front of raw `DiagnosticTransport`;
- semantic single-PID OBD send entry point;
- allow exactly Mode `01` and Mode `09` in this slice;
- reject everything else without invoking lower transport TX;
- preserve lower transport statuses for allowed requests;
- receive/poll path remains deterministic and does not create a second transport state machine;
- no raw application-facing diagnostic TX API;
- no OBD response parser, no UDS, no DTC client, no CAN routing, no VehicleData.

This Stage should be completed before the OBD service client exists.

### Phase 3B — Generic OBD-II read-only service semantics

Phase 3A is now complete at `5eefb442bc00e58005cc933fa6d56ab8bbe45ef6`; the outbound safety gate is available and Phase 3B may depend on it directly.

Bounded public protocol recheck was completed against the current CSS Electronics OBD2 overview / PID table:
- Mode `01` positive response service is `0x41`; generic positive response identity is request mode + `0x40`.
- Mode `01` PID `00` reports support for PIDs `01–20`; `20/40/.../C0` are subsequent support blocks.
- Mode `09` PID `02` requests VIN; the positive response service is `0x49`, and the common single-VIN response contains NODI `0x01` followed by 17 VIN bytes after service/PID.
- Multi-frame VIN transport is already handled below this layer by ISO-TP; OBD service semantics should consume the reassembled payload rather than reason about FF/CF/FC.

Phase 3B minimum service contract:
- depend on `ReadOnlyGuard` and `Clock`, not directly on `DiagnosticTransport`, CAN or TWAI;
- exactly one outstanding OBD request at a time;
- semantic Mode `01` single-PID request plus positive response mode/PID matching;
- supported-PID block request/parse for `00/20/40/60/80/A0/C0`; return a 32-bit mask for one block at a time rather than auto-querying multiple ECUs or creating a scheduler;
- semantic VIN request fixed to Mode `09` PID `02`, with bounded validation of response service/PID/NODI/17-byte VIN payload;
- deterministic response/no-response timeout using `Clock`;
- lower guard/transport start and poll failures propagate without being reported as service success;
- unrelated/mismatched response must never satisfy the outstanding request; it may be reported as unexpected while the original request remains pending until a valid response or timeout;
- matching but structurally invalid supported-PID/VIN response is a service-level invalid-response failure;
- generic Mode `01` data is returned as raw service data bytes in this slice; no SAE PID scaling/physical-unit conversion yet.

Phase 3B non-goals:
- no DTC modes;
- no UDS;
- no Mode `04`;
- no arbitrary raw diagnostic TX;
- no automatic functional-broadcast or multi-ECU aggregation;
- no CAN ID/routing ownership;
- no scheduler/poll cadence;
- no PID scaling database;
- no VehicleData mapping;
- no VAG-specific semantics.

## Non-goals / unchanged state

- No OBD-II implementation is performed by this evidence review.
- No UDS implementation.
- No live vehicle-facing diagnostic path.
- No application/Web/BLE diagnostic TX.
- No change to Phase 2A transport semantics.
- Bench / Hardware / Vehicle remain Pending.
- Generic namespace rename, 64-bit frame timestamp and ESP32 backend CI remain Cold.
