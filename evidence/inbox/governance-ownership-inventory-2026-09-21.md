# VAG governance ownership inventory — 2026-09-21

Status: REVIEW COMPLETE
Target: `masini1491/esp32-vag-data-server`
Purpose: governance ownership normalization evidence only; no execution authority.

Current shared baseline reviewed:
- `masini1491/ai-development-playbook@42fcd13205df8c351d41ff7df1a4ff51cede6c85`

Cross-repo comparison reviewed:
- `masini1491/ai-divination-playbook@19b26503a62d34b486a40b4c225913c9e5b9fea7`
- `masini1491/tw-stock-assistant@569f6e45671c5e1bb99a1289b7d1c97db016ea1b`
- `masini1491/access-control-system@5b130c0b58b7453dba726764feadaba3c94d1061`

## Decision model

Apply current Playbook ownership rules:
- shared cross-project engineering methodology moves to the shared Playbook canonical owner;
- project/domain truth remains in this repository;
- routing surfaces stay thin;
- machine/executable truth is not copied into prose owners;
- project-native activation is evaluated before shared-baseline loading when a narrower local route exists.

## Section inventory

### KEEP LOCAL — project mapping / project truth

1. `AI Development Playbook adoption`
   - Keep the single baseline declaration and `Project AI mode: ChatGPT+Codex`.
   - Keep canonical technical sources, current coordination surface, required validation summary, and project-specific exception summary.
   - Replace unconditional shared-baseline bootstrap wording with current project-native activation-first mapping.

2. `Authority boundary`
   - Keep: project-specific governance and technical truth outrank shared defaults.

3. `ChatGPT Coordination Write Allowlist`
   - Keep the exact local writable paths: `/TASKS.md`, `/BACKLOG.md`, sanitized `/evidence/inbox/*.md`.
   - Keep all other paths read-only for ChatGPT.
   - Hot task dossier remains disabled.
   - Detailed generic evidence-staging lifecycle/sanitization procedure should route to shared `AI_CONTEXT.md`; retain only the local path and any stricter local boundary.

4. Git source-of-truth restriction
   - Keep GitHub `main` as project source of truth and the project's clean fast-forward-only restriction if this repository intentionally remains stricter than shared defaults.
   - Generic Git safety/permission procedure belongs upstream.

5. `Library-ready design`
   - Keep. This is VAG project architecture truth, including dependency direction and “library-ready, not library-now”.

6. Project file-role map
   - Keep a thin project-specific map for `TASKS.md`, `BACKLOG.md`, `CODEX_PROGRESS.md`, `VALIDATION.md`, `docs/DEVELOPMENT.md`, `CHANGELOG.md`.
   - Do not restate generic Hot/Cold/completion lifecycle.

7. Physical evidence restriction
   - Keep the project-specific rule: Bench / Hardware / Vehicle remain Pending without real physical evidence and cannot be inferred from software/compile evidence.

8. `Hardware abstraction`
   - Keep all VAG/CAN/board/profile dependency rules.

9. `Read-only`
   - Keep the prohibited diagnostic behaviors and the invariant that diagnostic TX must not bypass `ReadOnlyGuard`.

10. `Reference reuse`
    - Keep the phase-specific local reference routing / Revisit trigger / provenance boundary.

11. Documentation language
    - Keep project documentation language/symbol conventions if desired.
    - Generic Codex reporting language/timestamp belongs to shared `CODEX_EXECUTION.md`.

### COLLAPSE TO POINTER — shared methodology already owned upstream

1. Current five-step fresh-session bootstrap
   - Replace with a thin activation/routing statement.
   - Current Playbook adopter contract requires project governance/current task authority first, then shared baseline only when activation requires it.
   - Owner: shared `CHAT_INIT.md` plus project-native activation mapping.

2. `Common playbook routing`
   - Keep one thin pointer only.
   - Do not maintain a local owner inventory or duplicate capability/authority rules.

3. `TASKS.md shared queue`
   - Remove generic Hot/Cold promotion/completion procedure.
   - Keep only local surface mapping and any project-specific exception.
   - Owners: `AI_CONTEXT.md`, `REPOSITORY_EXECUTION.md`, `CHATGPT_WORKFLOW.md`.

4. `Remote-sync bootstrap`
   - Remove generic fetch/status/dirty/diverged/unfinished-operation procedure.
   - Keep only local clean FF-only restriction/pointer.
   - Owner: `REPOSITORY_EXECUTION.md`.

5. `Permission-Gated Operation`
   - Remove generic permission/network/service/credential handling and forbidden workaround inventory unless a rule is uniquely stricter for this project.
   - Owner: `REPOSITORY_EXECUTION.md`.

6. `Short-launch queue semantics`
   - Remove generic Prompt mode/model/reasoning/Codex execution wording.
   - Owners: `CHATGPT_WORKFLOW.md`, `CODEX_EXECUTION.md`.

7. `Windows / PowerShell routing`
   - No current tracked project-owned PowerShell contract exists; generic runtime policy need not be restated here.
   - Owner when applicable: `TOOLCHAIN.md`.

8. `Repository reading and evidence`
   - Remove generic progressive reading, operational failure taxonomy, permission classification, retry semantics.
   - Keep only the physical-evidence project rule.
   - Owners: `AI_CONTEXT.md`, `DEBUG_VALIDATION.md`, `REPOSITORY_EXECUTION.md`.

9. `Validation and hardware evidence`
   - Collapse duplicate generic validation wording into the same local physical-evidence invariant.
   - Generic ladder/evidence lifecycle owner: `DEBUG_VALIDATION.md`.

### REVIEW DURING MUTATION

1. Evidence staging prose
   - Ensure the local writable path and local privacy/sanitized-only restriction survive.
   - Avoid copying shared Evidence Staging lifecycle prose.

2. Git safety prose
   - Preserve only rules demonstrably stricter/project-specific (not merely duplicated shared defaults).

3. File-role/update-threshold prose
   - Preserve project-specific owner mapping; remove generic history/completion methodology already upstream.

## Non-goals

- No Phase 2A implementation change.
- No change to `Project AI mode: ChatGPT+Codex`.
- No expansion of ChatGPT write authority.
- No change to ISO-TP, CAN HAL, read-only diagnostics, hardware evidence, roadmap, source, tests or validation truth.
- No deletion of `TASKS.md`, `BACKLOG.md` or evidence staging.
- No new project-native router unless a real independent retrieval intent is demonstrated.

## Expected result

A thinner `AGENTS.md` that owns:
- project adoption/mapping,
- project-specific authority and stricter exceptions,
- local write surfaces,
- VAG/embedded/read-only architecture invariants,
- local evidence/reference restrictions,

while shared engineering procedure is referenced from the current AI Development Playbook canonical owners rather than duplicated.
