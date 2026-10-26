# Voice, input and audio format

## Check the combination

Preserve configured voice/locale. Verify **region + voice/model + route + language + required markup/events** together. Catalog presence alone does not establish feature support; HD/multilingual/OpenAI variants differ. Never invent identifiers or universal styles.

| Route | Input |
|---|---|
| Complete-text SDK | Plain text or SSML via the matching method |
| Prebuilt REST | SSML; wrap plain text safely |
| Text input stream | New plain-text deltas; no SSML |
| Batch | `inputKind` selects PlainText/SSML |

## SSML example

```xml
<speak xmlns="http://www.w3.org/2001/10/synthesis" version="1.0" xml:lang="en-US">
  <voice name="en-US-JennyNeural">Price &lt; 5 &amp; delivery included.</voice>
</speak>
```

Serialize user text as text nodes; validate configured attributes. SSML voice overrides SDK voice. Optional Microsoft elements use `http://www.w3.org/2001/mstts`. Caller markup needs bounded size, DTD/external entities disabled and restricted external references. Well-formed XML does not prove that a voice supports a control. Do not force upstream text interfaces to generate XML.

## Output contract

| Output | Consumer needs |
|---|---|
| Raw PCM | Rate, bit depth, channels and sample representation out of band |
| RIFF/WAV | Correct container header and encoding; matching media type |
| MP3 / Opus | Exact codec/container, decoder and progressive playback support |

Match SDK enum/REST format string, media type and player. Renaming is not transcoding; concatenated WAV headers are not PCM; callback chunks need not be individual files. A backend speaker is not browser playback.

**Timing:** use the playback clock, not event arrival. Verify units (SDK offsets often 100 ns ticks; Batch may use ms). Listening confirms requested pronunciation/style; mocks do not.

Sources: [voices](https://learn.microsoft.com/azure/ai-services/speech-service/language-support?tabs=tts), [SSML](https://learn.microsoft.com/azure/ai-services/speech-service/speech-synthesis-markup). Docs checked 2026-10-19.
