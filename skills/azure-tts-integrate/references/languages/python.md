# Python integration

## Adapter contract

Copy [azure_tts.py](../../assets/python/azure_tts.py) for complete plain text as WAV24 or incremental plain text as raw24 PCM. This contract suffices for reuse; read source only for an internal failure or required change.

```python
from azure_tts import AzureTTS

tts = AzureTTS(subscription=key, region=region, voice=voice)
```

Alternative keywords: credential= for synchronous Entra TokenCredential; endpoint= for resource-base HTTPS. Entra requires its custom domain. Preserve existing identity; STS, SSML and other formats need [direct SDK](../realtime-sdk.md) adaptation. One adapter per immutable voice/identity/location.

| API | Contract |
|---|---|
| await tts.synthesize(text) | Nonempty plain text → WAV24kHz/16-bit/mono; no speakers; waits off-loop |
| await tts.speak_stream(deltas, emit, canceled) | Async new strings preserving whitespace → raw24 PCM; canceled is asyncio.Event. Waits for first text; serial sync/awaitable emit(bytes); drains normal output |
| Failure/stop | Errors propagate; event stop returns None; task cancellation propagates. Stops synthesis, closes source, detaches handlers, gates late audio; bounded best-effort cleanup preserves first error |
| Capacity/reuse | Reserve before native scheduling: max_pending=64 chunks, max_pending_bytes=1MiB per turn; BufferError on overload. Exclusive workers per active call on one loop; only clean success reused |
| await tts.aclose() | Reject new calls, close idle connections; active calls finish without re-pooling. Stop/await first for immediate shutdown; caller owns credential |

Bind existing callbacks and stop/turn/business states; await consumer work, count only delivered success and preserve original exceptions. [Wiring](../python-wiring.md) resolves field/signature mismatches.

Python 3.10+; recorded package/live checks use Speech 1.52.0. max_idle bounds cache, not concurrency; admission bounds pending output, not native/network memory. Verify OS prerequisites, refresh and private/custom routes separately. azure.identity.aio is incompatible.

Sources: [SpeechConfig](https://learn.microsoft.com/python/api/azure-cognitiveservices-speech/azure.cognitiveservices.speech.speechconfig), [platform setup](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform).
