# Map the application boundary

Use when the actual configuration or business callback differs from the adapter's [public contract](languages/python.md#adapter-contract). Keep existing types and names; no universal getter unless the app promises multiple shapes.

| Existing evidence | Bind using |
|---|---|
| dict/Mapping parser | Required keys |
| Typed settings | Declared attributes, including nesting |
| Secret provider | Resolved values, never environment dumps |
| Unknown shape | Inspect parser and caller before changing either |

## Two separate configuration examples

Fake values and illustrative field names only. Implement the application's promised shape.

```python
from azure_tts import AzureTTS

settings = {"speech_key": "FAKE-KEY", "speech_region": "eastus", "speech_voice": "en-US-JennyNeural"}
tts = AzureTTS(subscription=settings["speech_key"], region=settings["speech_region"], voice=settings["speech_voice"])
```

```python
from dataclasses import dataclass
from azure_tts import AzureTTS

@dataclass
class SpeechSettings:
    credential: str
    location: str
    speaker: str

settings = SpeechSettings("FAKE-KEY", "eastus", "en-US-JennyNeural")
tts = AzureTTS(subscription=settings.credential, region=settings.location, voice=settings.speaker)
```

A field named credential can contain a key; inspect its type rather than inferring identity from the name. An actual resource base URI maps to endpoint=; a synchronous Entra TokenCredential maps to credential=. Preserve raw STS token providers through the selected SDK/REST flow.

## Preserve business semantics

| App boundary | Adapter input / app responsibility |
|---|---|
| send_audio(data, *, turn, content_type) | Bind turn and media type in a small callback; return/await the send |
| Consumer callable returns an awaitable | Pass it directly if its signature is emit(bytes); otherwise return that awaitable from the wrapper |
| stopped boolean, disconnect signal, custom cancel object | Connect it to the app-owned asyncio.Event; keep the event per turn |
| started/audio/completed/interrupted/failed messages | The app owns those envelopes and exactly-once terminal state; the adapter emits audio only |
| Counters, history, auth, request IDs | Preserve app behavior; successful synthesis alone must not increment a delivery-success counter |
| Generator consumer | An extra facade owns iteration and early-close cancellation; not the callback API |

Test the real app entry with fake configuration and a recording consumer. Assert mapped identity/region/voice, exact business envelope and terminal state on success, stop and failure. Keep the original exception when reporting failure also fails. Do not rewrite unrelated history or business code. Synthesis lifecycle and shutdown rules remain in the public contract, not in a second queue/task manager.
