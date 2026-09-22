# Phase 4 UDS readiness — 2026-09-22

Status: COMPLETE
Purpose: Phase 4 readiness evidence only; no execution authority.

## Current authority / baseline

- Target repository baseline reviewed: `a9a793aa06b0bf2eabfac8f50a25a52751841cde`
- Shared Playbook reviewed: `masini1491/ai-development-playbook@403c7f9b3a1cfcce410c3389debcb864646e6444`
- Current Hot state before this review: `TASKS.md = EMPTY`
- Phase 2A ISO-TP, Phase 3A ReadOnlyGuard OBD gate and Phase 3B Generic OBD-II host service are complete.

## Project-native constraints

Current project authority requires:
- every active diagnostic TX to pass through `ReadOnlyGuard`;
- Generic UDS to remain brand-independent and free of CAN/TWAI-specific types;
- brand-specific ECU routes, DIDs, scaling and VAG semantics to remain outside Generic Core;
- no ClearDiagnosticInformation `0x14`, ECU Reset, SecurityAccess, WriteDataByIdentifier, IO Control, RoutineControl, coding, adaptation, flashing, output tests or actuator control;
- Bench / Hardware / Vehicle claims remain evidence-gated.

## Local reference result

`docs/references/SYNTHESIS.md` identifies read-only UDS concepts `0x22`, `0x19`, `0x3E`, only-when-needed `0x10`, negative response `0x7F` and NRC `0x78`.

`docs/references/can/OPENHALDEX_S3.md` confirms transferable UDS `0x22` / `0x19`, response-routing, timeout and NRC `0x78` patterns, but explicitly excludes reuse of active-control/write behavior, fixed routes and `0x14`.

Local evidence is sufficient for architecture direction but not for freezing the exact first generic UDS request/response shape, so a bounded public protocol recheck was justified by the repository Revisit trigger.

## Public UDS protocol recheck

Sources:
- py-uds Diagnostic Services: https://uds.readthedocs.io/en/stable/pages/knowledge_base/service.html
- py-uds Diagnostic Message: https://uds.readthedocs.io/en/latest/pages/knowledge_base/diagnostic_message.html

Relevant observations:
- `ReadDataByIdentifier` request SID is `0x22`; a request contains one or more 16-bit DIDs.
- Positive `ReadDataByIdentifier` response SID is `0x62`; response echoes the DID before its data record.
- Generic UDS positive response SID is request SID + `0x40`.
- Generic negative response shape is `7F <request SID> <NRC>`.
- NRC `0x78` means `requestCorrectlyReceived-ResponsePending`: the original request remains pending and the server is expected eventually to send a positive response or a different terminal negative response.

No third-party source code is copied by this evidence note.

## Phase 4 scope decision

Do not implement all read-only-looking UDS services at once.

First slice:
- only `ReadDataByIdentifier (0x22)`;
- one DID per request;
- no brand/VAG DID registry or scaling;
- no session-control automation;
- no DTC service;
- no TesterPresent;
- no application-facing live diagnostic TX.

Reasons:
- `0x22` is the minimum generic data-read primitive that directly supports later profile-owned DID semantics.
- `0x10` changes diagnostic session and is allowed by project policy only when a concrete read requires it; no current evidence requires it for this Stage.
- `0x19` has substantial subfunction/DTC-record semantics and should not be pulled into the first generic UDS slice.
- `0x3E` is session/timing maintenance and is unnecessary until a concrete session lifecycle exists.

## Proposed Stage split

### Phase 4A — UDS ReadOnlyGuard safety extension

Extend the existing guard with a semantic single-DID UDS read entry point.

Minimum contract:
- semantic input is a 16-bit DID, not arbitrary bytes;
- internally construct exactly `22 DID_hi DID_lo`;
- preserve lower `DiagnosticTransport::startSend` status in the existing guard result model;
- OBD guard behavior remains unchanged;
- do not expose generic/raw UDS TX;
- do not expose `0x10`, `0x19`, `0x3E` or any other UDS service yet;
- guard API remains Generic Core and independent of CAN/TWAI and VAG-specific types.

### Phase 4B — Generic UDS ReadDataByIdentifier service core (not admitted yet)

After Phase 4A completion, a separate readiness/admission may implement:
- one outstanding `0x22` DID request at a time;
- positive response matching `62 DID_hi DID_lo <data...>`;
- generic negative response parsing `7F 22 NRC`;
- NRC `0x78` keeps the same request active and refreshes/extends a bounded response-pending deadline;
- other matching NRC values terminate the request as a negative response while exposing the NRC;
- wrong positive SID/DID or negative response for another SID must not satisfy the active request;
- matching but malformed positive/negative response is deterministic invalid response;
- injected `Clock` controls initial and response-pending timeouts;
- raw DID data bytes are returned without scaling/meaning interpretation;
- no DID registry, VAG routing, VehicleData mapping, session control, DTC, TesterPresent or scheduler.

## Non-goals / unchanged state

- No UDS implementation is performed by this evidence review.
- No `0x10`, `0x19`, `0x3E`, `0x14`, SecurityAccess or write/control service implementation.
- No CAN/ECU routing or functional-addressing ownership.
- No VAG DID/scaling semantics.
- No application/Web/BLE diagnostic TX.
- No change to ISO-TP or OBD service semantics.
- Bench / Hardware / Vehicle remain Pending.


## Phase 4B readiness finalization

Phase 4A completed at `4b8609302dc6569af53d7718d4ea75c152c682cb`, providing the semantic `0x22` guard path.

Public protocol verification reconfirmed:
- `ReadDataByIdentifier` request SID `0x22`;
- positive response SID `0x62`;
- generic negative response shape `7F <request SID> <NRC>`;
- NRC `0x78` is response-pending and keeps the request logically active rather than completing it as failure.

### Phase 4B exact service contract

Implement a Generic UDS `ReadDataByIdentifier` service with:
- dependency only on `ReadOnlyGuard` + injected `Clock`;
- exactly one outstanding DID read at a time;
- outbound request only through `ReadOnlyGuard::startUdsReadDataByIdentifier`;
- positive response must match `62 DID_hi DID_lo <data...>`;
- wrong SID or DID is `UnexpectedResponse` and must not complete the active request;
- matching positive response with no data bytes is `InvalidResponse`;
- matching negative response `7F 22 NRC`:
  - NRC `0x78`: keep request active and refresh a bounded response-pending deadline;
  - any other NRC: terminal negative response, exposing the NRC;
- malformed negative response is `InvalidResponse`;
- negative response for another request SID is `UnexpectedResponse`;
- use injected `Clock` for initial response timeout and bounded response-pending timeout;
- lower guard/transport start/poll/receive failures propagate deterministically;
- return raw DID bytes only; no VAG scaling/meaning.

Recommended timeout model:
- initial response timeout begins only after lower transmission reaches response-wait state;
- each valid NRC `0x78` refreshes the pending deadline;
- configure a maximum pending-count (or equivalent bounded pending budget) so repeated `0x78` cannot extend forever;
- exhausting pending-count or deadline produces timeout/pending-limit failure deterministically.

### Phase 4B non-goals

- no `0x10`, `0x19`, `0x3E`;
- no DID registry;
- no VAG routing/scaling;
- no CAN/ECU addressing ownership;
- no scheduler/VehicleData;
- no application-facing diagnostic TX;
- no ISO-TP change.
