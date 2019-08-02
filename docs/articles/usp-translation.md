# Universal Speech Protocol Extension for Speech Translation

This document describes extensions to the Universal Speech Protocol (USP) to support translation of audio voice in one language into text and voice in other languages.

## Connection Establishment

Connections to speech translation service require a WebSocket that conforms to [IETF RFC 6455](https://tools.ietf.org/html/rfc6455). The steps for establishing connections are the same as for the speech recognition service. To establish a Websocket connection, the client application sends an HTTPS GET request to the service and includes standard WebSocket upgrade headers along with other required headers. The connection is established after service responds with a successful upgrade. The connection requires the [TLS 1.2](https://en.wikipedia.org/wiki/Transport_Layer_Security) encryption; the use of unencrypted speech requests is not supported.

### Speech Translation Service Endpoint

The current endpoint of speech translation service is `wss://{region}.s2s.speech.microsoft.com/speech/translation/cognitiveservices/v1`.

The speech translation service uses conversation mode.

### Query Parameters

The translation service supports most query parameters of the standard speech recognition service, except the `language` query parameter. It also supports the use of a custom model for speech recognition. In addition, it supports the following translation specific query parameters.

| Parameter | Default Value | Description | Data Type |
| - | - | - | - |
| from | (empty) | Specifies the language of the incoming speech. | string |
| to | (empty) | Specifies the language to translate the transcribed text into. | string |
| features | (empty) | Comma-separated set of features selected by the client. Available features include: `TextToSpeech`: specifies that the service must return the translated audio of the final translated sentence. | string |
| voice | (empty) | Identifies what voice to use for text-to-speech rendering of the translated text. Currently only one voice name is allowed. | string |
| stableTranslation | (empty) | Stabilize translations in partial results by omitting words in the end. | string |

### HTTP Headers

The parameters that can be included in the HTTP headers are the same as for the speech recognition service.

## Websocket Messages for Speech Translation Service

WebSocket messages in USP can use either a text or a binary encoding as specified by [IETF RFC 6455](https://tools.ietf.org/html/rfc6455). 

### Client-Originated Messages

Client can send [Audio message](https://speechwiki.azurewebsites.net/partners/speechsdk#audio-message), [Speech.Config message](https://speechwiki.azurewebsites.net/partners/speechsdk#speechconfig-message), [Speech.Context message](https://speechwiki.azurewebsites.net/partners/speechsdk#speechcontext-message), [Telemetry Message](https://speechwiki.azurewebsites.net/partners/speechsdk#telemetry-message) to speech translation service. These messages follow the same format the corresponding one for speech recognition service. 

In addition, client can send `Event message` to add or remove target languages during the translation session. Once the service received the event message, the change will take effect after the next final result is generated.

#### *Event* Message

The *event* message is a text WebSocket message. This message can be sent at anytime after connection setup. No X-RequestId is needed in the message.

#### Required Message Headers

| Header Name | Value |
| ----- | ------------|
| Path | *event* |
| X-Timestamp | Client UTC clock timestamp in ISO 8601 format |
| Content-Type | application/json; charset=utf-8 |

The body of the *event* message is a [JSON](http://json.org/) structure. This structure contains the fields specified in the following table.

| Field Name | Value | Description | Usage |
| - | - | -| - |
| id | translation | the event category. The value "translation" indicates that the event message is targeting speech translation service. | Required |
| name | updateLanguage | the event name. The value "updateLanguage" indicates that the event message is for updating target languages of translation. | Required |
| to | an JSON array of string | the list of new target languages. | Required |

#### Example *event* Message

```HTML
Path: event
Content-Type: application/json; charset=utf-8
X-Timestamp: 2019-08-02T09:43:00.180Z

{
    "id":"translation",
    "name":"updateLanguage",
    "to": [ "Zh-CN", "de" ]
}
```

### Service-Originated Messages 

In addtion to [Speech.StartDetected message](https://speechwiki.azurewebsites.net/partners/speechsdk#speechstartdetected-message), [Speech.EndDetected message](https://speechwiki.azurewebsites.net/partners/speechsdk#speechenddetected-message), [Turn.Start message](https://speechwiki.azurewebsites.net/partners/speechsdk#turnstart-message), and [Turn.End message](https://speechwiki.azurewebsites.net/partners/speechsdk#turnend-message), the following messages are added to the Universal Speech Protocol, which are sent by speech translation service to clients. Same as other USP messages, the translation responses messages must include required message headers.

#### *Translation.Hypothesis* Message

The speech recognition service periodically generates hypotheses about the words the service has recognized. Based on these hypotheses (also known as partial results or intermediate results), the translation service creates *translation.hypothesis* response messages.

The hypothesis messages contain only text results. The *translation.hypothesis* message is suitable **only** for enhancing the user speech experience; you must not take any dependency on the content or accuracy of the text in these messages.

| Field | Description |
| - | - |
| WebSocket message encoding | Text |
| Path | *translation.hypothesis* |
| X-RequestId | [Universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier) in "no-dash" format |
| Content-Type | application/json |
| Body | The translation hypothesis JSON structure |

The body is the JSON serialization of an object with the following properties:

| Property | Description |
| - | - |
| Text | Recognized text in the source language |
| Offset | Time offset of the start of the recognition in ticks (1 tick = 100 nanoseconds). The offset is relative to the beginning of streaming. |
| Duration | Duration in ticks (100 nanoseconds) of the recognition. |
| TranslationStatus | the translation status. Allowed values are "Success", and "Error". |
| Translations | a JSON array. Each element in the array is a JSON object representing the translated text in a target language. |
| FailureReason | Failure reason in case of "Error" in TranslationStatus. |

#### Sample Message

```HTML
Path: translation.hypothesis
Content-Type: application/json; charset=utf-8
X-RequestId: 123e4567e89b12d3a456426655440000

{
  "Text": "what was",
  "TranslationStatus": "Success",
  "Translations": [ 
    {
        "Language" : "de",
        "Text" : "Was war"
    },
    {
        "Language" : "zh-CN",
        "Text" : "如何"
    }
  ],
  Offset: 2731600000,
  Duration: 11900000
}
```

#### *Translation.Phrase* Message

The translation.phrase is generated at the end of an utterance. The translation service produces these results after it detects that the user has completed a sentence or phrase.

| Field | Description |
| - | - |
| WebSocket message encoding | Text |
| Path | *translation.phrase* |
| Content-Type | application/json |
| Body | The translation phrase JSON structure |

The body is the JSON serialization of an object with the following properties:

| Property | Description |
| - | - |
| Text | Recognized text in the source language |
| Offset | Time offset of the start of the recognition in ticks (1 tick = 100 nanoseconds). The offset is relative to the beginning of streaming. |
| Duration | Duration in ticks (100 nanoseconds) of the recognition. |
| RecognitionStatus | the speech recognition status. Allowed values are the same as returned by speech recognition status, e.g. "Success", "InitialSilenceTimeout", "EndOfDictation", and etc. |
| TranslationStatus | the translation status. Allowed values are "Success", and "Error". |
| Translations | a JSON array. Each element in the array is a JSON object representing the translated text in a target language. |
| FailureReason | Failure reason in case of "Error" in TranslationStatus. |

#### Sample Message

```HTML
Path: translation.phrase
Content-Type: application/json; charset=utf-8
X-RequestId: 123e4567e89b12d3a456426655440000

{
  "Text": "what is the weather like?",
  "TranslationStatus": "Success",
  "Translations": [ 
    {
        "Language" : "de",
        "Text" : "Wie ist das Wetter?"
    },
    {
        "Language" : "zh-CN",
        "Text" : "天气怎么样？"
    }
  ],
  Offset: 300000,
  Duration: 21900000
}
```

#### *Translation.Synthesis* Message

When the text-to-speech feature is enabled (see features parameter below), translation.hypothesis messages contain audio chunks of the spoken translated text. Audio data is chunked and sent from the service to the client as a sequence of `translation.synthesis` messages. The `translation.synthesis` messages are *Binary WebSocket messages*.

The first `translation.synthesis` message contains a well-formed header that properly specifies that the audio conforms to one of the encoding formats supported by the service. Additional *audio* messages contain only the binary audio stream data read from the microphone. The service sends an `translation.synthesis.end` message to indicate the end of audio data sent from the service.

| Header |  Value |
| - | - |
| WebSocket message encoding | Binary |
| Path | *translation.synthesis* |
| X-RequestId | [Universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier) in "no-dash" format |
| Content-Type | The audio content type. Must be one of *audio/x-wav* (PCM) or *audio/mp3*. |
| Body | Audio data |

**NOTE** The audio output messages are created only for translation.phrase result. There is no audio output for translation.hypothesis results. All translation.synthesis messages for the current translation.phrase must be sent before the next translaton.phrase.

#### *Translation.Synthesis.End* Message

The `translation.synthesis.end` is sent to return the synthesis status of voice output. It also indicates the end of `translation.synthesis` messages.

| Field | Description |
| - | - |
| WebSocket message encoding | Text |
| Path | *translation.synthesis.end* |
| Content-Type | application/json |
| Body | The translation phrase JSON structure |

The body is the JSON serialization of an object with the following properties:

| Property | Description |
| - | - |
| SynthesisStatus | the synthesis status. Allowed values are "Success", and "Error". |
| FailureReason | Failure reason in case of "Error" in SynthesisStatus. |

#### Sample Message

```HTML
Path: translation.synthesis.end
Content-Type: application/json; charset=utf-8
X-RequestId: 123e4567e89b12d3a456426655440000

{
  "SynthesisStatus": "Error",
  "FailureReason": "Synthesis service is temporarily not available."
}
```

## Error Handling

The error messages and conditions used by current USP are specified [here](https://speechwiki.azurewebsites.net/architecture/protocol-usp-error-messages.html).
