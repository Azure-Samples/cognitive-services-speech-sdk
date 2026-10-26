# Real-time REST

Use the existing backend HTTP client and configured **full operation URL**, preserving path/query and approved identity. Regional public example: `https://<region>.tts.speech.microsoft.com/cognitiveservices/v1`. Other resource/private routes differ; see [identity](authentication.md).

## Request contract

```http
POST /cognitiveservices/v1 HTTP/1.1
Host: <region>.tts.speech.microsoft.com
Authorization: Bearer <resolved-at-runtime>
Content-Type: application/ssml+xml
X-Microsoft-OutputFormat: riff-24khz-16bit-mono-pcm
User-Agent: existing-app

<speak xmlns="http://www.w3.org/2001/10/synthesis" version="1.0" xml:lang="en-US">
  <voice name="en-US-JennyNeural">Hello &amp; welcome.</voice>
</speak>
```

The voice is an example, not a default. Key auth replaces Authorization with `Ocp-Apim-Subscription-Key`; never combine or silently change flows. Resolve secrets at runtime. Serialize plain text into XML text nodes; preserve special/non-ASCII characters. Caller SSML needs its own validation/external-reference controls.

## Match delivery

| Consumer | Format / delivery |
|---|---|
| Complete WAV | Matching `riff-...-pcm` + `audio/wav`; read complete binary response |
| MP3 | Compatible `audio-...-mp3` + `audio/mpeg` |
| Raw PCM | Exact rate/bit-depth/channels out of band |
| Progressive output | Read available chunks before EOF; keep response open through iteration |

A large blocking read can wait to fill its buffer. Use the client's `read1`/available-data/streaming interface; wrapping final bytes in an iterator is still buffered. Chunks need not be playable files.

## Failure and cleanup

| Result | Action |
|---|---|
| Success | Return/forward actual bytes, not success JSON |
| Input/auth error | Propagate safely; no unchanged retry |
| Partial transfer/disconnect | Mark incomplete; close owned response and stop delivery |
| Transient error | Existing bounded recovery; never replay speech already delivered |

Close response/iterator on exhaustion, error or cancellation; preserve shared clients. Development live-test retry limits are not production retry configuration. Verify status before treating body as audio, requested format/SSML, decodability and consumer timing. Mocks do not prove Azure access.

Sources: [REST](https://learn.microsoft.com/azure/ai-services/speech-service/rest-text-to-speech), [SSML](https://learn.microsoft.com/azure/ai-services/speech-service/speech-synthesis-markup). Docs checked 2026-10-19.
