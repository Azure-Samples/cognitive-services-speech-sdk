# Azure TTS Integration

Agent skills for adding Azure Speech text to speech to applications, streaming text or audio, generating Batch audio, and troubleshooting TTS integrations.

## Installation

This repository's `skills/` directory is a distribution layout. Copy the entire `azure-tts-integrate` folder, including `references`, `assets` and `scripts`, into one supported project-level location:

| Target | Project-level destination |
|---|---|
| GitHub Copilot / Codex | `.agents/skills/azure-tts-integrate/` |
| Claude Code | `.claude/skills/azure-tts-integrate/` |
| VS Code alternative | `.github/skills/azure-tts-integrate/` |

Choose one location and review an existing customization before replacing it.

## Skill Structure

```text
azure-tts-integrate/
|-- README.md
|-- SKILL.md
|-- references/
|-- assets/python/azure_tts.py
`-- scripts/feedback_form_url.py
```

## Quick Reference

| Document | Responsibility |
|---|---|
| [SKILL.md](./SKILL.md) | Choose one path, reuse compatible code, validate and report the completion status. |
| [azure_tts.py](./assets/python/azure_tts.py) | Python Speech SDK adapter: key or Entra, region or endpoint; complete text and text stream with synthesizer reuse. |
| [capabilities.md](./references/capabilities.md) | Language, identity, implementation and evidence coverage. |
| [python-wiring.md](./references/python-wiring.md) | Explicit configuration mapping, consumer contract and first local integration check. |
| [realtime-sdk.md](./references/realtime-sdk.md) | Complete text or SSML through the Speech SDK. |
| [realtime-rest.md](./references/realtime-rest.md) | HTTP synthesis and binary audio delivery. |
| [text-input-streaming.md](./references/text-input-streaming.md) | Incremental text input with concurrent audio output. |
| [batch-synthesis.md](./references/batch-synthesis.md) | Asynchronous jobs, polling, artifacts, and retention. |
| [authentication.md](./references/authentication.md) | Identity, endpoint, secret boundary, and token renewal. |
| [voices-and-ssml.md](./references/voices-and-ssml.md) | Voice, input markup, events, and audio format. |
| [troubleshooting.md](./references/troubleshooting.md) | Symptom-driven diagnosis within the current path. |
| [feedback.md](./references/feedback.md) | Optional issue record and one-time feedback invitation with consent and review. |
| [feedback_form_url.py](./scripts/feedback_form_url.py) | Open a prefilled feedback form after consent; rejects obvious sensitive content. |

## Example Prompts

> Add Azure TTS to this API and return MP3 audio through its existing response interface.

> Our C# service already has an LLM. Stream its text into speech without replacing the conversation pipeline.

> Read responses aloud through the existing browser audio player. Keep the Speech key on the backend.

> Generate audio for these documents as asynchronous jobs and store the results in our existing storage.

> Synthesis succeeds, but our application plays no sound. Diagnose the existing implementation.

> I'd like to give feedback about setting up Azure TTS with this skill.

## Requirements

- A coding agent that supports Agent Skills, file editing, and terminal commands.
- A supported application runtime and the dependencies required by the selected path.
- Azure Speech access and authentication for live validation.
- Optional: Python 3.10+ for the feedback helper.

Keep long-lived secrets on a trusted backend, never in chat or client code. Local checks do not prove end-to-end success, and Azure usage may be billable.

## Reading order

1. Read `SKILL.md`. Inspect the existing app, choose the matching language or HTTP path, and load only that contract.
2. Read that path's guide only for details the request and code leave unresolved.
3. Load wiring, identity or voice/format references only for an unresolved question.

Recorded key-auth service checks use SDK 1.52.0: the 2026-10-23 frozen runtime passed complete-text, stream, long-wait, overload, stop/error, concurrent-delivery and shutdown checks. That runtime is unchanged by this documentation cleanup. Results are tied to recorded skill/runtime hashes, not a production SLA or proof of listening quality. Entra, custom domains and token-expiry refresh remain unverified live; see the capability matrix.

## Related Resources

- [Azure Speech text to speech](https://learn.microsoft.com/azure/ai-services/speech-service/text-to-speech)
- [Text to speech quickstart](https://learn.microsoft.com/azure/ai-services/speech-service/get-started-text-to-speech)
- [Speech SDK samples](https://github.com/Azure-Samples/cognitive-services-speech-sdk)
- [Agent Skills specification](https://agentskills.io/specification)

The Python adapter public contract is maintained in references/languages/python.md. Wiring examples adapt app configuration and business messages; SDK/streaming guides cover direct implementation and service behavior; language notes cover runtime differences. No reference requires reading all the others. See references/capabilities.md for dated evidence; the unchanged runtime has recorded live checks, while documentation-only revisions are not new efficiency evidence.
