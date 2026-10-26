# JavaScript / TypeScript integration decisions

Reuse microsoft-cognitiveservices-speech-sdk and the existing Node/browser delivery. No bundled JS implementation or browser player is supplied.

| Need | Choice / constraint |
|---|---|
| Complete text / SSML | speakTextAsync / speakSsmlAsync callbacks; explicitly bridge success/error to a Promise |
| Progressive audio | Synthesizing events/output stream where runtime supports it; this is audio-output streaming |
| Incremental LLM text input | Current official TextStream list excludes JS. Use a compatible backend or agree a complete-text/chunking tradeoff |
| Identity | Backend subscription config or approved short-lived authorization token; preserve expiry/issuer routing |
| Resource/custom endpoint | Use installed SDK endpoint factory, not an automatic switch to REST |

Backend bytes require explicit null audio output where supported; browser playback needs autoplay/user gesture, codec and playback-clock checks. Do not embed a resource key in frontend code. Refresh the active synthesizer token with the supported API. Use SDK close callbacks on owned synthesizers; no Python disconnect_all substitutions. For Entra combinations verify runtime-specific official support instead of assuming Python TokenCredential overloads exist in JS.

Sources: [JS synthesizer API](https://learn.microsoft.com/javascript/api/microsoft-cognitiveservices-speech-sdk/speechsynthesizer), [SpeechConfig API](https://learn.microsoft.com/javascript/api/microsoft-cognitiveservices-speech-sdk/speechconfig), [TextStream supported languages](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-lower-speech-synthesis-latency).
