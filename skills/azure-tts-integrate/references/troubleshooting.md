# Diagnose the current integration

Preserve the first redacted error. Inspect one boundary that distinguishes causes; do not restart onboarding or replace a working pipeline.

| Symptom | First discriminating check |
|---|---|
| SDK import failure | Runtime/package/native prerequisites |
| Missing configuration | Actual parser/factory mapping with fake settings |
| 401/403 | Identity, issuer/endpoint, expiry, role/network/key policy |
| 400/415 | Serialized SSML, namespace, headers, voice/format support |
| 429 | Request rate versus voice/region capacity; bounded backoff |
| Success but no sound | Actual bytes → decoder → consumer → player/autoplay |
| Corrupt audio | Codec/container/rate/channels at both ends |
| Audio only at end | Full-body/final-result buffering, proxy/client buffering |
| Repeated/overlapping speech | New deltas versus snapshots, shared queues, retries |
| Speech after stop | Producer + synthesis + playback buffers + late callbacks |
| Service failure hangs | Race failed SDK result/events against a stalled producer |
| Batch timeout/missing output | Original ID/status/deadline, per-input summary, download/expiry/storage |

## Fix and prove

1. State one cause and the smallest check that could disprove it.
2. Repair the failing stage within existing scope.
3. Rerun that check plus affected normal/error/stop paths; preserve behavior during later changes.
4. Report evidence and remaining live validation separately.

Do not disable TLS, silently change identity/voice/region or kill unrelated processes. Never replay delivered speech; reconcile ambiguous Batch work by original ID. Development test retry limits do not define production recovery. Service success is not client playback proof.

Sources: [synthesis](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-speech-synthesis), [limits](https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-quotas-and-limits). Docs checked 2026-10-19.
