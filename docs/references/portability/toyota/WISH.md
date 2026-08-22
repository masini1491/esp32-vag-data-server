# Toyota Wish

Reviewed: 2026-08-22  
Role: Phase 13 same-brand / different-generation portability validation target  
Evidence class: `INSUFFICIENT DIRECT EVIDENCE`, `VEHICLE_HYPOTHESIS`  
Reuse status: `REFERENCE_ONLY`；no source, database or speculative profile。

## Confirmed upstream evidence

The supplied targeted research did not identify a sufficiently mature, clearly attributable canonical Toyota Wish diagnostic/CAN project. Current reviewed opendbc Toyota definitions do not provide a Wish entry; no ODBd/Toyota-Wish repository was identified; reviewed CanBusHack Toyota coverage does not contain Wish.

## Architecture / validation implication

Wish remains Phase 13 for same-brand / different-generation validation under weak upstream evidence. Profile Resolver must be evidence-driven and may return `Unknown`、`Ambiguous` 或 `Manual selection required`。

## Do not infer

Do not infer that Wish uses RAV4、New MC、TNGA、KWP or UDS；do not infer CAN bitrate、CAN IDs、diagnostic addresses、OBD capabilities or ECU layout. No Wish or Toyota support is claimed.

## Future validation backlog

Exact vehicle identity → model year/engine/trim → VIN/ECU identification → read-only capability discovery → standard OBD support → ECU fingerprint/routing → passive CAN evidence → candidate profile family → validated Vehicle Profile。全部 `Pending`。

## Revisit trigger

Exact Wish year/engine becomes the target、real Wish trace or direct ECU identification becomes available，或 credible upstream adds Wish-specific coverage。Until then，不重複搜尋 GitHub。
