---
name: azure-tts-integrate
description: 'Use when adding or repairing Azure Speech TTS in an application: speech audio, incremental LLM text, delivery or Batch synthesis. Excludes STT, Avatar video, Voice Live and voice training.'
---

# Azure TTS in the existing application

Preserve runtime, identity, endpoint, business API and transport. Keep keys on the backend; Entra/custom domains alone do not require REST.

## Inspect

Announce checks of manifests, app entry points and configuration loader. Identify runtime/framework/SDK, text source, audio consumer and stop/shutdown ownership. Reuse known findings for repairs/resumed work; use [diagnosis](references/troubleshooting.md) for unclear failure boundaries.

## Choose

Honor explicit choices; otherwise select the smallest compatible path. Audio output streaming is not incremental text input; file output alone does not require Batch. Read references only for unresolved details.

| Need | Resource |
|---|---|
| Python adapter | [Contract](references/languages/python.md); copy [adapter](assets/python/azure_tts.py) when identity/input/format match |
| Other SDK language | [C#](references/languages/csharp.md), [JS/TS](references/languages/javascript.md), [Java/C++](references/languages/other.md); select one |
| Direct SDK / incremental input | [SDK](references/realtime-sdk.md) / [TextStream](references/text-input-streaming.md) |
| HTTP / durable bulk | [REST](references/realtime-rest.md) / [Batch](references/batch-synthesis.md); HTTP-only work needs no SDK notes |
| Other unresolved contract | [Identity](references/authentication.md), [voice/format](references/voices-and-ssml.md), [Python wiring](references/python-wiring.md), [support](references/capabilities.md); select the relevant guide |

## Recommend

Before editing, present these in the developer's language; mark unknowns:

1. **Recommended path:** SDK/REST/TextStream/Batch and why it fits.
2. **What I found:** up to three facts covering stack/SDK, text/audio flow and backend identity/configuration source/presence. Distinguish proposals; omit secrets.
3. **Architecture:** text → backend synthesis → existing consumer/storage.
4. **What changes:** up to three affected components and the first local check.
5. **Next step:** proceed within authorization or ask one unresolved path/configuration question.

Recommendation-only requests stop here without edits or setup. Reuse existing choices/authorization; wait only for unresolved changes to runtime, identity or delivery. Repairs/resumed work need only changed findings and the next check.

## Configure

Check actual configuration; templates/placeholders are not credentials. Reuse compatible key/Entra/STS providers and setting names. Report configured/missing, never values; presence does not prove access. Ask only for missing settings.

For missing identity, offer supported choices using the question tool or a numbered list: **Configure an existing resource key locally**, **Use an existing Entra identity**, **Continue offline**. Collect missing non-secret settings together. For keys, specify the backend provider or Git-ignored file and setting names; request local saving and readiness confirmation, never secrets in chat. Wait before identity-dependent wiring/live calls; continue independent work. Honor an existing offline choice.

## Implement

Preserve consumer signatures/formats, turn identity, terminal outcomes and resource ownership. Map samples to existing config names. Copy compatible assets unchanged; add missing glue without duplicating lifecycle or changing identity. Check installed APIs when uncertain.

Group edits with focused regressions. At milestones/blockers, report changes, check results and next action without repeating the recommendation.

## Validate

Check affected audio/config/error/stop/cleanup/business behavior through the app entry. Run required suites and affected regressions; batch independent checks and reuse passing results. Expand only for new changes, failures or specific unresolved risks; keep output focused.

Separate local/live evidence. With authorized access, verify service audio decodes through the app; otherwise state the blocker and next check. Mention billable usage once before the first Azure call. Browser audio needs playback/stop checks and listening confirmation. Synthesis completion is not playback; native work may outlive cleanup deadlines.

## Report status

| Status | Evidence |
|---|---|
| Implementation complete; Azure validation pending | Affected local checks pass |
| Integration complete | Also decoded service audio through the app and affected stop/error checks pass. Audible playback requires developer listening confirmation |

Report outcome, files, startup/configuration steps, local/live/playback evidence, remaining work and runtime state. Honor external-action authorization.

After Integration complete and temporary validation resources are released, offer feedback once: **Review feedback draft / Not now**. Respect prior invitations, refusal or silence. Read [feedback](references/feedback.md) only for an explicit feedback request or accepted invitation, never just for errors/retries. Sharing requires approval of reviewed content; feedback never blocks implementation.
