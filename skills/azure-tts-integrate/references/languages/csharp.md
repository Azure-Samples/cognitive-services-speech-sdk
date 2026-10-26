# C# integration decisions

Reuse the app's Microsoft.CognitiveServices.Speech package and DI/configuration. No bundled C# adapter is provided or compiled by these Python tests.

| Need | Official API / constraint |
|---|---|
| Complete text / SSML | SpeakTextAsync / SpeakSsmlAsync; inspect result reason, return bytes or app-owned stream |
| Incremental LLM input | SpeechSynthesisRequestInputType.TextStream, InputStream.Write/Close, V2 WebSocket, StartSpeakingAsync(request) sample |
| Audio delivery | Synthesizing events or AudioDataStream; preserve application consumer and playback format |
| Identity | FromSubscription; existing authorization-token flow; Entra FromEndpoint(custom URI, TokenCredential) where installed SDK supports it |

For TextStream configure voice/output and official timeout properties; no SSML input. Wait for first text before starting. Each active call needs exclusive synthesizer ownership; reuse after clean completion, stop/detach on cancellation/error and dispose owned streams/results/synthesizers at app shutdown. Do not transplant Python method names or cleanup APIs.

Sources: [C# TextStream sample](https://github.com/Azure-Samples/cognitive-services-speech-sdk/tree/master/samples/csharp/tts-text-stream), [synthesis](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-speech-synthesis), [Entra](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-configure-azure-ad-auth).
