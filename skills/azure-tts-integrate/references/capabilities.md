# Capability and verification matrix

Choose by application constraints, then language and identity. Official support does not mean this skill supplies a reusable module, and an offline constructor check does not establish service access. Read one relevant language note only when needed.

| Application runtime | Complete text / SSML | Incremental text input | Skill implementation |
|---|---|---|---|
| [Python](languages/python.md) | SDK supports both; adapter synthesize accepts plain text only | Official SDK TextStream | Copyable plain-text WAV / raw24 stream adapter |
| [C#](languages/csharp.md) | Official SDK | Official SDK TextStream | Guidance and official samples; no bundled C# runtime |
| [JavaScript / TypeScript](languages/javascript.md) | Official SDK, Node or browser as supported | Not listed for TextStream by current official guidance | Guidance; complete-text output streaming or Python/C# backend |
| [Java / C++ and HTTP runtimes](languages/other.md) | SDK where supported, or REST | C++ listed for TextStream; Java not listed | Official reference links; no bundled implementation |
| Any HTTP backend | REST requires SSML; Batch accepts PlainText/SSML | Standard REST is not incremental-text input | Request/recovery guides; no reusable HTTP/Batch client |

## Identity and evidence are separate

| Combination | Official mechanism / adapter coverage | Evidence here |
|---|---|---|
| Python key + public region | SpeechConfig subscription/region; V2 for TextStream | Recorded key-auth live regression on the unchanged runtime |
| Python key + resource base endpoint | SDK endpoint; derive V2 resource route for TextStream | Regional endpoint live regression; custom domain offline only |
| Python Entra credential + custom domain | SpeechConfig token_credential/endpoint | Constructor/routing offline only; real service and expiry refresh unverified |
| Speech STS bearer token | SDK auth_token or REST Bearer; issuer/host must match | SDK supports it; bundled adapter does not. Preserve app provider |
| Entra REST | Operation-specific Bearer format, see [identity](authentication.md) | Guidance only; do not infer Batch/SDK token formats from REST |
| Browser identity | Trusted backend token issuance or backend audio; no embedded resource key | No bundled browser token issuer/player, no new online evidence |

Evidence is recorded by exact skill/runtime hash in the repository evaluation reports. The 2026-10-23 frozen runtime passed 10 live checks covering key/region, regional endpoint, first-text waiting, long pauses, overload, stop/error/reuse and shutdown. Later documentation edits do not change that tested runtime, but do not establish new agent efficiency. No multi-language build, Entra live refresh, custom/private routing, listening or production load guarantee.

## Existing-business workflow

1. Identify the existing parser/identity, producer, consumer and ownership; preserve interfaces.
2. Apply hard constraints: runtime, HTTP requirement, incremental input, SSML, codec and endpoint.
3. Reuse a matching module; implement only missing glue. Otherwise follow the selected official SDK/REST path.
4. Check the real app entry offline: mapping, ordered audio, stop/error/cleanup, concurrent isolation and overload when relevant.
5. With existing authorization, validate actual service access and decoding separately; listening and token-expiry claims need their own evidence.

Sources checked 2026-10-23: [text streaming and reuse](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-lower-speech-synthesis-latency), [Entra](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-configure-azure-ad-auth), [REST](https://learn.microsoft.com/azure/ai-services/speech-service/rest-text-to-speech), [SDK samples](https://github.com/Azure-Samples/cognitive-services-speech-sdk).
