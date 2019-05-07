# Support Translation Speech Service with Universal Speech Protocol

 The Universal Speech Protocol (USP) is currently being used by Microsoft's Speech Service to convert spoken audio to text. This page describes the extensions of USP to support translation speech service that translates audio voice in one language into text or voice in another language.

## Connection Establishment

Connections to Microsoft speech service as well as translation speech service require a WebSocket connection. The Universal Speech Protocol (USP) follows the WebSocket standard specification [IETF RFC 6455](https://tools.ietf.org/html/rfc6455).

To establish a Websocket connection, the client application sends an HTTPS GET request to the service and includes standard WebSocket upgrade headers along with other required headers. The connection is established after service responds with a successful upgrade. The connection requires the [TLS 1.2](https://en.wikipedia.org/wiki/Transport_Layer_Security) encryption; the use of unencrypted speech requests is not supported.

### Translation Endpoint URL

The current endpoint of the translation service is `https://dev.microsofttranslator.com/speech/translate/v1`. After merging the translation service into Skyman, a different URL endpoint is expected. It is desirable that the same endpoint is used as that for speech recognition, and translation is just a feature parameter. However, this needs to be discussed with the Skyman service team.

The translation service is using conversational model or customized models. We need discuss with the Skyman service team about how to pass customized model information to the service.

**NOTE** What is the expectation regarding backward compatibility for current translation service users? Keeping the old service running for some
 time and asking them to migrate to the new service, or maintaining both services for long term?

#### Query parameters

The following query parameters are supported.

| Parameter | Default Value | Description | Data Type |
| - | - | - | - |
| from | (empty) | Specifies the language of the incoming speech. The value is one of the language identifiers from the speech scope in the response from the [Languages API](https://docs.microsoft.com/azure/cognitive-services/Translator/reference/v3-0-languages). | string |
| to | (empty) | Specifies the language to translate the transcribed text into. The value is one of the language identifiers from the text scope in the response from the Languages API. | string |
| features | (empty) | Comma-separated set of features selected by the client. Available features include: `TextToSpeech`: specifies that the service must return the translated audio of the final translated sentence. `Partial`: specifies that the service must return intermediate recognition results while the audio is streaming to the service. `TimingInfo`: specifies that the service must return timing information associated with each recognition. As an example, a client would specify `features=partial,texttospeech` to receive partial results and text-to-speech, but no timing information. Note that final results are always streamed to the client. | string |
| voice | (empty) | Identifies what voice to use for text-to-speech rendering of the translated text. The value is one of the voice identifiers from the tts scope in the response from the Languages API. If a voice is not specified, the system will automatically choose one when the text-to-speech feature is enabled.| string |
| format | (empty) | Specifies the format of the text-to-speech audio stream returned by the service. Available options are: `audio/wav`: Waveform audio stream. `audio/mp3`: MP3 audio stream. The default is `audio/wav`.| string |
| ProfanityAction | (empty) | Specifies how the service should handle profanities recognized in the speech. Valid actions are: `NoAction`: Profanities are left as is.`Marked`: Profanities are replaced with a marker. See `ProfanityMarker` parameter. `Deleted`: Profanities are deleted. For example, if the word "jackass" is treated as a profanity, the phrase "He is a jackass." will become "He is a .". The default is `Marked`. | string |
| ProfanityMarker | (empty) | Specifies how detected profanities are handled when ProfanityAction is set to Marked. Valid options are: `Asterisk`: Profanities are replaced with the string `\*\*\*`. For example, if the word "jackass" is treated as a profanity, the phrase "He is a jackass." will become "He is a ***.". `Tag`: Profanity is surrounded by a profanity XML tag. For example, if the word "jackass" is treated as a profanity, the phrase "He is a jackass." will become "He is a <profanity>jackass</profanity>.". The default is `Asterisk`. | string |

**NOTE** In future, both `from` and `to` will contain a set of languages. It is also possible that `from` is empty, which means that the service will auto-detect what is the language being spoken.

**NOTE** Since Client SDK should be able to set HTTP headers, there is no need to support access\_token, subscription-key, X-ClientTraceId, X-CorrelationId, X-CLientVersion, X-OsPlatform in query parameters as an alternative way for passing authentication information.

**NOTE** Currently query parameters are set before establishing connection and cannot be changed after that. In future, these parameters can be changed dynamically after setting up connection.

### HTTP Headers

The following parameters can be included in the HTTP headers.

#### Connection Identifier

USP requires that clients **must** include the *X-ConnectionId* header when starting a WebSocket handshake. The *X-ConnectionId* header value must be a [universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier). It is used to uniquely identify the connection.
WebSocket upgrade requests that do not include the *X-ConnectionId*, that do not specify a value for the *X-ConnectionId* header, or that do not include a valid [universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier) value will be rejected by the service with a 400 Bad Request response.

**NOTE** This replaces the X-ClientTraceId used by the current translation service. In USP, each request is also uniquely identified by X-RequestId in the message header.

#### Authorization

In addition to the standard WebSocket handshake headers, USP requires an *Authorization* header. Connection requests without this header are rejected by the service with a 403 Forbidden HTTP response. Both subscription key and authorization token are supported. See [the page](https://docs.microsoft.com/azure/cognitive-services/speech/how-to/how-to-authentication?tabs=Powershell) for details.

#### X-CorrelationId

X-CorelationId is a client-generated identifier used to correlate multiple channels in a conversation. Multiple speech translation sessions can be created to enable conversations between users. In such scenario, all speech translation sessions use the same correlation ID to tie the channels together. This facilitates tracing and diagnostics. The identifier should conform to: `^[a-zA-Z0-9-_.]{1,64}$`.

#### X-ClientVersion

X-ClientVersion identifies the version of the client application. Example: "2.1.0.123".

#### X-OsPlatform

X-OsPlatform Identifies the name and version of the operating system the client application is running on. Examples: "Android 5.0", "iOs 8.1.3", "Windows 8.1".

The X-OsPlatform platform will be removed from the header. Instead, [speech.config](https://speechwiki.azurewebsites.net/partners/speechsdk#speech-config-message) message is used to collect the device data about client.

## Websocket Messages for Translation Speech Service

WebSocket messages in USP can use either a text or a binary encoding as specified by [IETF RFC 6455](https://tools.ietf.org/html/rfc6455). [This page](https://docs.microsoft.com/azure/cognitive-services/Speech/api-reference-rest/websocketprotocol#message-types) describes the format of Text WebSocket Messages and Binary WebSocket Messages supported by USP.

### Client Request Messages

Clients using the current translation speech service only send audio data to the service. The [`Audio` message](https://speechwiki.azurewebsites.net/partners/speechsdk#audio) is used to send audio chunk to the translation speech service.

The first *audio* message must contain a well-formed header that properly specifies that the audio conforms to one of the encoding formats supported by the service. Additional *audio* messages contain only the binary audio stream data. The maximum size of a single audio chunk is 8192 bytes. Audio stream messages are *Binary WebSocket messages*.

Clients may optionally send an *audio* message with a zero-length body; this message tells the service that the client knows that the user has stopped speaking, that the utterance is complete, and that the microphone is turned off.

The service uses the first *audio* message that contains a unique request identifier to signal the start
of a new request/response cycle or *turn*. After receiving an *audio* message with a new request identifier, the service
discards any queued or unsent messages that are associated with any previous turn.

**NOTE** Different than the messages used by the current translation speech service, all messages in USP have required message headers. The following headers are required for all client-originated USP messages. Detailed information about the header is described [here](https://docs.microsoft.com/azure/cognitive-services/Speech/api-reference-rest/websocketprotocol#client-originated-messages).

The following headers are required for all *audio* messages.

| Header | Value |
| - | - |
| Path | *audio* |
| X-RequestId | [Universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier) in "no-dash" format |
| X-Timestamp | Client UTC clock timestamp in ISO 8601 format |
| Content-Type | The audio content type. Must be one of *audio/x-wav* (PCM) or *audio/silk* (SILK) |

**NOTE** The speech client SDK also sends other messages, for example, speech.config, and telemetry messages to the speech service. Details of these messages are described [here](https://docs.microsoft.com/azure/cognitive-services/Speech/api-reference-rest/websocketprotocol#client-originated-messages).

### Service Responses Messages

The Universal Speech Protocol needs to be extended to support new response messages returned by the translation speech service. Same as other USP messages, the translation responses messages must include required message headers.

The following messages are added to support the translation speech service. messages are referenced by the value of the *Path* header.

**NOTE** It is not clear for now whether the client receives response messages returned by the speech service, for example, speech.start, turn.start, speech.hypothesis, speech.phrase, and etc. It is dependent on how Skyman to plumb the speech service and translation service. From user's point of view, some messages, for example, speech.hypothesis/speech.phrase are not useful, but some might be still helpful, e.g turn.start, turn.end, speech.end and etc.

#### Translation Hypothesis

The speech recognition service periodically generates hypotheses about the words the service has recognized. Based on these hypotheses (also known as partial results or intermediate results), the translation service creates *translation.hypothesis* response messages.

The hypothesis messages contain only results in text. They are not sent back to the client by default. The client can use the `features` query parameter to request them. The *translation.hypothesis* message is suitable **only** for enhancing the user speech experience; you must not take any dependency on the content or accuracy of the text in these messages.

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
| sourceLanguage | The name of source language. |
| targetLanguage | The name of target language. |
| recognition | Recognized text in the source language |
| translation | Recognized text translated in the target language |
| audioTimeOffset | Time offset of the start of the recognition in ticks (1 tick = 100 nanoseconds). The offset is relative to the beginning of streaming. |
| audioTimeSize | Duration in ticks (100 nanoseconds) of the recognition. |

The `sourceLanguage` and `targetLanguage` in the response describe the language name, in BCP 47 language tag format, of the recognized text and translated text. These data are needed since user might specify multiple languages, or none language, in `from` or/and `to` parameters.
**NOTE** What does the `id` in the current translation partial result mean? Does it uniquely identify each partial result, or used to correlate the client request and the result? In the later case, the X-RequestId already provides this information.

**NOTE** The following properties are removed: `type` (the Path has this information), `audioSizeOffset`, and `audioSizeBytes` (the speech service does not support bytes based offset).

#### Sample Message

```HTML
Path: translation.hypothesis
Content-Type: application/json; charset=utf-8
X-RequestId: 123e4567e89b12d3a456426655440000

{
  recognition: "what was",
  translation: "Was war",
  sourceLanguage: "en-us",
  targetLanguage: "de-de",
  audioTimeOffset: 2731600000,
  audioTimeSize: 11900000
}
```

#### Translation Phrase

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
| sourceLanguage | The name of source language. |
| targetLanguage | The name of target language. |
| recognition | Recognized text in the source language. The text may be an empty string in the case of a false recognition. |
| recognitionStatus | has the following value representing speech recognition result: `Success`, `NoMatch`, `InitialSlienceTimeout`, `BabbleTimeout`, `Error`. Details see [here](https://speechwiki.azurewebsites.net/partners/speechsdk#speech-phrase)|
| translation | Recognized text translated in the target language. |
| translationStatus | TBD |

| audioTimeOffset | Time offset of the start of the recognition in ticks (1 tick = 100 nanoseconds). The offset is relative to the beginning of streaming. |
| audioTimeSize | Duration in ticks (100 nanoseconds) of the recognition. |

Note that positioning of the recognition in the audio stream is not included in the results by default. The TimingInfo feature must be selected by the client (see features parameter).

**NOTE** The `recognitionStatus` is added to describe recognition status. The value is aligned with the speech recognition service as described [here](https://speechwiki.azurewebsites.net/partners/speechsdk#speech-phrase). The `translationStatus` is also added, but no value defined yet.

**NOTE** The same question about `id` as in the partial result.

#### Sample Message

```HTML
Path: translation.phrase
Content-Type: application/json; charset=utf-8
X-RequestId: 123e4567e89b12d3a456426655440000

{
  recognition: "what was said",
  recognitionStatus: "Success",
  translation: "Was wurde gesagt",
  sourceLanguage: "en-us",
  targetLanguage: "de-de",
  translationStatus: TBD
  audioTimeOffset: 2731600000,
  audioTimeSize: 21900000
}
```

#### Audio output messages

When the text-to-speech feature is enabled (see features parameter below), a translation.hypothesis is followed by the audio of the spoken translated text. Audio data is chunked and sent from the service to the client as a sequence of `translation.synthesis` messages. The `translation.synthesis` messages are *Binary WebSocket messages*.

The first `translation.synthesis` message contains a well-formed header that properly specifies that the audio conforms to one of the encoding formats supported by the service. Additional *audio* messages contain only the binary audio stream data read from the microphone. The service sends an `translation.synthesis` message with a zero-length body to indicate the end of audio data sent from the service.

**NOTE** Different than the audio messages used by the current translation speech service, the `translation.synthesis` message requires message headers.

| Header |  Value |
| - | - |
| WebSocket message encoding | Binary |
| Path | *translation.synthesis* |
| X-RequestId | [Universally unique identifier](https://en.wikipedia.org/wiki/Universally_unique_identifier) in "no-dash" format |
| Content-Type | The audio content type. Must be one of *audio/x-wav* (PCM) or *audio/mp3*. |
| Body | Audio data |

**NOTE** Different than the audio message in the current translation speech service, service should send out a zero-length body to indicate the end of audio, instead of using the FIN bit in WebSocket. This is consistent with the behavior when client sends audio to the service for recognition. In addition, the FIN bit is not available for some Websocket libraries.

**NOTE** The audio output messages are created only for final translation result (i.e. the `translation.phrase` message). There is no audio output for intermediate results. The current translation service does not have any correlation schema between the translation.phrase message and the following translation.synthesis messages. It assumes that all translation.synthesis messages for the current translation.phrase should be sent before the next translaton.phrase. If this assumption does not hold anymore, a kind of identifier needs to added both to translation.phrase and translation.synthesis in order to correctly associate these messages.

**NOTE** Currently there is no plan to support audio output in multiple languages, so there is no information about the language of synthesized audio data in `translation.synthesis`.

### Telemetry Messages

The speech USP protocol defines a set of telemetry messages to be exchanged between client and service. See [Telemetry schema](https://docs.microsoft.com/azure/cognitive-services/speech/api-reference-rest/websocketprotocol#telemetry-schema) details.

**NOTE** Currently, the translation team is working on instrumentation spec for the translation service. Further discussions are need how to define telemetry messages for translation and how to align with the speech telemetry messages.

### Management Messages

#### Translation Discovery Request

The `translation.discoveryRequest` message is used by client for querying the [languages resource](https://docs.microsoft.com/azure/cognitive-services/Translator/reference/v3-0-languages) for the current set of supported languages, for example, the set of languages and voices available for speech recognition, for text translation and for text-to-speech.

| Field | Description |
| - | - |
| WebSocket message encoding | Text |
| Path | *translation.discoveryRequest* |
| X-Timestamp | Client UTC clock timestamp in ISO 8601 format |
| Content-Type | application/json; charset=utf-8 |
| Body | The payload as a JSON structure |

The `translation.discoveryRequest` message must include an X-Timestamp header that records the client UTC clock time at which the message was sent to the service. The `translation.discoveryRequest` message *does not* require an X-RequestId header, since this message is not associated with a particular speech request.

The payload of the `translation.discoveryRequest` message is a JSON structure containing the following properties

| Property | Description |
| - | - |
| scope | The sets of language capability to discover. Possible values are `speech`, `text`, `tts`, and their combination. |
| inputLanguages | A comma-separated list that specifies the languages to query. Only language resource of specified languages will be returned. If the list is empty, resource of all languages is returned.
| Accept-Language | The language in which names of languages or regions are returned. |

The property `scope` defines which sets of languages a user is interested in. `scope=speech` to retrieve the set of languages available to transcribe speech into text. `scope=text` to retrieve the set of languages available to translate transcribed text. And `scope=tts` to retrieve the set of languages and voices available to synthesize translated text into speech. A client can retrieve multiple sets simultaneously by specifying a comma-separated list of choices. For example, `scope=speech,text,tts`.

The `Accept-Language` specifies the language in which names of languages and regions are returned. The language is specified by providing a well-formed BCP 47 language tag. For unsupported languages, the names are provided in the English language. For instance, use the value fr to request names in French or use the value zh-Hant to request names in Chinese Traditional.

**NOTE** The X-ClientTraceId is removed, since the request is not associated with a specific client request. And the X-ConnectionId in the HTTP header should be sufficient to identify the client. If this is required by the translation service, it can be added though.

**NOTE** User should be able to send this message without establishing any Websocket connection, since the discovery response contains data that he needs to set up the Websocket connection. It needs to be decided though how to support this capability.

#### Sample Message

```HTML
Path: translation.discoveryRequest
Content-Type: application/json; charset=utf-8
X-Timestamp: 2018-02-09T13:03:54.183Z

{
  scope: "speech,text,tts",
  inputLanguage: "en-us, zh-cn"
  Accept-Language: "en-us"
}
```

#### Translation Discovery Response

The `translation.discoveryResponse` message is returned by service to the client as response of `translation.discoveryRequest`. It provides the set of languages and voices available for the specified scope.

| Field | Description |
| - | - |
| WebSocket message encoding | Text |
| Path | *translation.discoveryReponse* |
| Content-Type | application/json; charset=utf-8 |
| Body | The payload as a JSON structure |

The payload of the `translation.discoveryResponse` message is a JSON structure:

```JSON
{
    "speech": {
        //... Set of languages supported for speech-to-text
    },
    "text": {
        //... Set of languages supported for text translation
    },
    "tts": {
        //... Set of languages supported for text-to-speech
    }
}
```

Since a client can use the scope query parameter to select which sets of supported languages should be returned, an actual response may only include a subset of all properties shown above.

The value returned for each scope is described below.

#### Response value for scope `speech`

The value associated with the `speech` scope is a dictionary of {key, value} pairs. Each key identifies a language supported for speech-to-text. The key is the language identifier in BCP 47 language tag format, and the value associated with the key is an object with the following properties:
| Property | Description |
| - | - |
| name | Display name of the language. |
| language | Language tag of the associated written language. Used to specify the language key used in [Response Value for scope `text`](#response-value-for-scope-text) below |

An example is:

```JSON
{
    "speech": {
        "en-US": { name: "English", language: "en" }
    }
}
```

#### Response value for scope `text`

The value associated with the `text`scope is also a dictionary where each key identifies a language supported for text translation. The value associated with the key describes the language:
| Property | Description |
| - | - |
| name | Display name of the language. |
| dir  | Directionality that is rtl for right-to-left languages or ltr for left-to-right languages. |

An example is:

```JSON
{
    "text": {
        "en": { name: "English", dir: "ltr" }
    }
}
```

#### Response value for scope `tts`

The value associated with the `tts` is also a dictionary where each key identifies a supported voice. Attributes of a voice object are:
| Property | Description |
| - | - |
| displayName | Display name for the voice. |
| gender | Gender of the voice (male or female). |
| locale | Language tag of the voice with primary language subtag and region subtag. |
| language | Language tag of the associated written language. |
| languageName | Display name of the language. |
| regionName | Display name of the region for this language. |

An example is:

```JSON
{
    "tts": {
        "en-US-Zira": {
            "displayName": "Zira",
            "gender": "female",
            "locale": "en-US",
            "languageName": "English",
            "regionName": "United States",
            "language": "en"
        }
}
```

## Error Handling

The error messages and conditions used by current USP are specified [here](https://speechwiki.azurewebsites.net/architecture/protocol-usp-error-messages.html). There are a couple of error messages used by the current translation service that are not used by USP, for example, HTTP error 429 (Too many requests), WebSocket Error 1003 (Invalid Message Type), 1001 (Endpoint unavailable). If needed, they can be added.
