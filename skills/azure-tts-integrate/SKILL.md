---
name: azure-tts-integrate
description: 'Use when adding or repairing Azure Speech TTS in an application: speech audio, incremental LLM text, delivery or Batch synthesis. Excludes STT, Avatar video, Voice Live and voice training.'
---

# Azure TTS in the existing application

Preserve runtime, identity, endpoint, business API and transport. Entra/custom domains alone do not require REST. Keep resource keys on the backend.

## Inspect and choose

Inspect manifests, business entry and config parser for runtime, text/audio flow and stop/shutdown ownership. Reuse known findings; use [diagnosis](references/troubleshooting.md) only when the failure boundary or cause remains unclear.

Honor explicit choices; otherwise choose the smallest compatible path. Output streaming differs from incremental text; file output need not use Batch.

| Existing need | Read only the matching resource |
|---|---|
| Python | [Python contract](references/languages/python.md); copy [adapter](assets/python/azure_tts.py) only when identity/input/format match |
| C# / JS or TS / Java or C++ | [C#](references/languages/csharp.md), [JS/TS](references/languages/javascript.md), or [other](references/languages/other.md); select one |
| Direct SDK complete text/SSML / incremental input | [SDK](references/realtime-sdk.md) / [TextStream](references/text-input-streaming.md), only for unresolved details |
| Required HTTP / durable bulk job | [REST](references/realtime-rest.md) / [Batch](references/batch-synthesis.md); skip SDK/language notes for HTTP-only work |

## Implement

Briefly state the path and affected components; proceed within authorization. Ask only for blocking decisions. Missing credentials block live checks, not local implementation.

Reuse existing config names across code/templates/checks; map sample names. Preserve consumer format/signature, turn identity, terminal outcomes and resource ownership. Copy matching assets unchanged; do not duplicate lifecycle or switch language/identity to fit them.

Only for unanswered questions: [identity](references/authentication.md), [voice/format](references/voices-and-ssml.md), [Python wiring](references/python-wiring.md), [support/evidence](references/capabilities.md). Batch independent reads; skip guides when the contract suffices. Never invent APIs.

## Verify and report

Check config, delivered audio and changed stop/error/cleanup/business outcomes through the real entry. Run required project checks and affected regressions; reuse passing evidence and broaden only for a new change, failure or unresolved risk. Native work can outlive cleanup deadlines; synthesis completion is not playback.

## Report status

| Status | Evidence |
|---|---|
| Implementation complete; Azure validation pending | Changed-boundary offline checks pass |
| Integration complete | Also real service audio through app path decodes; changed stop/error paths checked. Audible playback requires developer listening confirmation |

Report changed files, evidence and remaining checks. Honor external-action authorization; optional [feedback](references/feedback.md) never blocks implementation.
