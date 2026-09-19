# Phase 2 ISO-TP bounded revisit — 2026-09-20

Status: COMPLETE
Purpose: Phase 2 readiness evidence only; no execution authority.

## Scope

Bounded revisit triggered by the start of Phase 2 readiness. Rechecked only implementation decisions that were not fully settled by the existing local synthesis.

## Public upstream evidence

1. Linux kernel ISO-TP documentation (ISO 15765-2 / SocketCAN ISO-TP)
   - Source: https://docs.kernel.org/networking/iso15765-2.html
   - Current documentation states normal addressing can use explicit RX/TX CAN IDs.
   - Flow Control parameters include block size, STmin and WFTmax.
   - STmin encoding includes 0x00–0x7F = 0–127 ms and 0xF1–0xF9 = 100–900 us; other values are reserved.
   - The kernel ISO-TP socket binds an RX CAN ID that acts as an RX filter and a TX CAN ID for transmit routing.
   - Reported multi-frame errors include receive timeout, sequence mismatch, flow-control timeout and flow-control overflow.

2. Linux kernel CAN documentation
   - Source: https://docs.kernel.org/networking/can.html
   - Multiple protocol consumers can receive matching CAN frames from the same interface; routing/filtering is separate from the raw CAN frame source.

## Project implications

- Current project `Clock::nowMs()` is sufficient for millisecond protocol timeouts, but it cannot represent ISO-TP sub-millisecond STmin exactly. Phase 2 must define an explicit conservative policy rather than silently treating 0xF1–0xF9 as ordinary milliseconds.
- Current `CanHal::receive()` is destructive FIFO. An ISO-TP implementation must not establish a design that silently consumes unrelated CAN traffic and thereby blocks future passive CAN/shared-bus consumers.
- Prefer a host-testable, externally driven/non-busy-wait ISO-TP state machine. Tests should advance `FakeClock` deterministically.
- Phase 2 v1 remains Classic CAN. CAN FD, extended/mixed addressing and unrelated future transport families are outside the first implementation slice unless separately admitted.
- This evidence does not authorize OBD-II/UDS service transmission or any vehicle-facing diagnostic request path.

## Provenance / reuse

This note records public documentation observations only. No upstream source code is copied. Existing repository-local license/provenance rules remain authoritative.
