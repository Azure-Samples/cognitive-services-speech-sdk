# Complete-text Speech SDK

For bundled Python plain-text WAV, use the adapter's [public contract](languages/python.md#adapter-contract). Read this for SSML, different output, another language, or direct SDK implementation. Incremental text belongs in [TextStream](text-input-streaming.md).

## Match the actual SDK/runtime

| Operation | Python | C# | JavaScript |
|---|---|---|---|
| Plain text | speak_text_async | SpeakTextAsync | speakTextAsync |
| SSML | speak_ssml_async | SpeakSsmlAsync | speakSsmlAsync |
| Result bytes | audio_data | AudioData | audioData |
| No default speakers | audio_config=None | Explicit null audio config | Explicit null audio config |

Packages: azure-cognitiveservices-speech, Microsoft.CognitiveServices.Speech, microsoft-cognitiveservices-speech-sdk. Keep the app's dependency manager and supported runtime. Verify installed signatures and [identity](authentication.md) for the selected language; no cross-language method-name substitution.

## Direct implementation decisions

| Boundary | Required behavior |
|---|---|
| Config | Set voice and exact output format before constructing the synthesizer. SSML voices override SDK voice |
| Async wait | Python .get() off-loop; JS callbacks need a Promise bridge, not await speakTextAsync(...) alone |
| Result | Require SynthesizingAudioCompleted before treating audio_data as usable output; errors/canceled results propagate |
| Progressive output | Subscribe/read before final completion; forward ordered bytes to the actual consumer and bound introduced buffering |
| Reuse | Exclusive synthesizer per active call, recycle only after completed result/delivery/cleanup; cache ownership follows app lifetime |
| Stop / shutdown | Stop synthesis, gate late output, clear owned player buffers, detach handlers and release resources using the language SDK APIs |

Null differs from omitted output: server speakers are not browser playback. Callback chunks need not be individual WAV files. Match codec/container, media type and consumer format; renaming or concatenating WAV headers is not transcoding. Python file output can use AudioOutputConfig(filename=...) when appropriate.

Python EventSignal supports disconnect_all(), not disconnect(handler). SDK Connection.from_speech_synthesizer(...).close() can close an idle connection; do not invent SpeechSynthesizer.close(). C#/JS disposal differs. Preserve shared credential/client ownership.

For a nondefault format, set the matching SDK enum during configuration in an app-specific factory or a deliberate module adaptation. Do not mutate a pooled worker's format between concurrent users.

**Verify:** fake config through the real factory, invalid values rejected before SDK, actual decodable output, service/caller cancellation and consumer errors, no event-loop blocking or cross-voice leakage. Compilation and doubles do not prove service access or audible playback.

Sources: [synthesis](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-speech-synthesis), [SDK setup](https://learn.microsoft.com/azure/ai-services/speech-service/quickstarts/setup-platform), [reuse guidance](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-lower-speech-synthesis-latency).
