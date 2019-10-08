//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines property ids.
 * Changed in version 1.7.0.
 */
public enum PropertyId {

    /**
     * The Cognitive Services Speech Service subscription key. If you are using an intent recognizer, you need
     * to specify the LUIS endpoint key for your particular LUIS app. Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromSubscription"/>.
     */
    SpeechServiceConnection_Key (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Key),

    /**
     * The Cognitive Services Speech Service endpoint (url). Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromEndpoint"/>.
     * NOTE: This endpoint is not the same as the endpoint used to obtain an access token.
     */
    SpeechServiceConnection_Endpoint (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Endpoint),

    /**
     * The Cognitive Services Speech Service region. Under normal circumstances, you shouldn't have to
     * use this property directly.
     * Instead, use <see cref="SpeechConfig.fromSubscription"/>, <see cref="SpeechConfig.fromEndpoint"/>,
     * <see cref="SpeechConfig.fromAuthorizationToken"/>.
     */
    SpeechServiceConnection_Region (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Region),

    /**
     * The Cognitive Services Speech Service authorization token (aka access token). Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromAuthorizationToken"/>,
     * <see cref="SpeechRecognizer.setAuthorizationToken"/>, <see cref="IntentRecognizer.setAuthorizationToken"/>,
     * <see cref="TranslationRecognizer.setAuthorizationToken"/>.
     */
    SpeechServiceAuthorization_Token (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceAuthorization_Token),

    /**
     * The Cognitive Services Speech Service authorization type. Currently unused.
     */
    SpeechServiceAuthorization_Type (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceAuthorization_Type),

    /**
     * The Cognitive Services Custom Speech Service endpoint id. Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead use <see cref="SpeechConfig.setEndpointId"/>.
     * NOTE: The endpoint id is available in the Custom Speech Portal, listed under Endpoint Details.
     */
    SpeechServiceConnection_EndpointId (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_EndpointId),

    /**
     * The host name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyHostName (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyHostName),

    /**
     * The port of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyPort (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyPort),

    /**
     * The user name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyUserName (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyUserName),

    /**
     * The password of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyPassword (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyPassword),

    /**
     * The URL string built from speech configuration.
     * This property is intended to be read-only. The SDK is using it internally.
     * NOTE: Added in version 1.5.0.
     */
    SpeechServiceConnection_Url (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Url),

    /**
     * The list of comma separated languages (BCP-47 format) used as target translation languages. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechTranslationConfig.addTargetLanguage"/>,
     * <see cref="SpeechTranslationConfig.getTargetLanguages"/>, <see cref="TranslationRecognizer.getTargetLanguages"/>.
     */
    SpeechServiceConnection_TranslationToLanguages (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationToLanguages),

    /**
     * The name of the Cognitive Service Text to Speech Service voice. Under normal circumstances, you shouldn't have to use this
     * property directly. Instead use <see cref="SpeechTranslationConfig.setVoiceName"/>.
     * NOTE: Valid voice names can be found <a href="https://aka.ms/csspeech/voicenames">here</a>.
     */
    SpeechServiceConnection_TranslationVoice (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationVoice),

    /**
     * Translation features. For internal use.
     */
    SpeechServiceConnection_TranslationFeatures (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationFeatures),

    /**
     * The Language Understanding Service region. Under normal circumstances, you shouldn't have to use this property directly.
     * Instead, use <see cref="LanguageUnderstandingModel"/>.
     */
    SpeechServiceConnection_IntentRegion (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_IntentRegion),

    /**
     * The Cognitive Services Speech Service recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
     * This property is intended to be read-only. The SDK is using it internally.
     */
    SpeechServiceConnection_RecoMode (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode),

    /**
     * The spoken language to be recognized (in BCP-47 format). Under normal circumstances, you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setSpeechRecognitionLanguage"/>.
     */
    SpeechServiceConnection_RecoLanguage ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoLanguage),

    /**
     * The session id. This id is a universally unique identifier (aka UUID) representing a specific binding of an audio input stream
     * and the underlying speech recognition instance to which its bound. Under normal circumstances, you shouldn't have to use this
     * property directly.
     * Instead use <see cref="SessionEventArgs.getSessionId"/>.
     */
    Speech_SessionId ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Speech_SessionId),

    /**
     * The spoken language to be synthesized (e.g. en-US)
     * Added in version 1.7.0
     */
    SpeechServiceConnection_SynthLanguage ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_SynthLanguage),

    /**
     * The name of the TTS voice to be used for speech synthesis
     * Added in version 1.7.0
     */
    SpeechServiceConnection_SynthVoice ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_SynthVoice),

    /**
     * The string to specify TTS output audio format (e.g. riff-16khz-16bit-mono-pcm)
     * Added in version 1.7.0
     */
    SpeechServiceConnection_SynthOutputFormat ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_SynthOutputFormat),

    /**
     *  The initial silence timeout value (in milliseconds) used by the service.
     *  Added in version 1.5.0
     */
    SpeechServiceConnection_InitialSilenceTimeoutMs ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_InitialSilenceTimeoutMs),

    /**
     *  The end silence timeout value (in milliseconds) used by the service.
     *  Added in version 1.5.0
     */
    SpeechServiceConnection_EndSilenceTimeoutMs ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs),

    /**
     *  A boolean value specifying whether audio logging is enabled in the service or not.
     *  Added in version 1.5.0
     */
    SpeechServiceConnection_EnableAudioLogging ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_EnableAudioLogging),

    /**
     * The requested Cognitive Services Speech Service response output format (simple or detailed). Under normal circumstances, you shouldn't have
     * to use this property directly.
     * Instead use <see cref="SpeechConfig.setOutputFormat"/>.
     */
    SpeechServiceResponse_RequestDetailedResultTrueFalse (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse),

    /**
     * The requested Cognitive Services Speech Service response output profanity level. Currently unused.
     */
    SpeechServiceResponse_RequestProfanityFilterTrueFalse (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse),

    /**
     *  The requested Cognitive Services Speech Service response output profanity setting.
     *  Allowed values are "masked", "removed", and "raw".
     *  Added in version 1.5.0.
     */
    SpeechServiceResponse_ProfanityOption ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_ProfanityOption),

    /**
     *  A string value specifying which post processing option should be used by service.
     *  Allowed values are "TrueText".
     *  Added in version 1.5.0
     */
    SpeechServiceResponse_PostProcessingOption ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_PostProcessingOption),

    /**
     *  A boolean value specifying whether to include word-level timestamps in the response result.
     *  Added in version 1.5.0
     */
    SpeechServiceResponse_RequestWordLevelTimestamps ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps),

    /**
     *  The number of times a word has to be in partial results to be returned.
     *  Added in version 1.5.0
     */
    SpeechServiceResponse_StablePartialResultThreshold ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_StablePartialResultThreshold),

    /**
     *  A string value specifying the output format option in the response result. Internal use only.
     *  Added in version 1.5.0.
     */
    SpeechServiceResponse_OutputFormatOption ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_OutputFormatOption),

    /**
     *  A boolean value to request for stabilizing translation partial results by omitting words in the end.
     *  Added in version 1.5.0.
     */
    SpeechServiceResponse_TranslationRequestStablePartialResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_TranslationRequestStablePartialResult),

    /**
     * The Cognitive Services Speech Service response output (in JSON format). This property is available on recognition result objects only.
     */
    SpeechServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_JsonResult),

    /**
     * The Cognitive Services Speech Service error details (in JSON format). Under normal circumstances, you shouldn't have to
     * use this property directly.
     * Instead, use <see cref="CancellationDetails.getErrorDetails"/>.
     */
    SpeechServiceResponse_JsonErrorDetails ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_JsonErrorDetails),

    /**
     * The recognition latency in milliseconds. Read-only, available on final speech/translation/intent results.
     * This measures the latency between when an audio input is received by the SDK, and the moment the final result is received from the service.
     * The SDK computes the time difference between the last audio fragment from the audio input that is contributing to the final result, and the time the final result is received from the speech service.
     * Added in version 1.3.0.
     */
    SpeechServiceResponse_RecognitionLatencyMs ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RecognitionLatencyMs),

    /**
     * The cancellation reason. Currently unused.
     */
    CancellationDetails_Reason ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_Reason),

    /**
     * The cancellation text. Currently unused.
     */
    CancellationDetails_ReasonText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonText),

    /**
     * The cancellation detailed text. Currently unused.
     */
    CancellationDetails_ReasonDetailedText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonDetailedText),

    /**
     * The Language Understanding Service response output (in JSON format). Available via <see cref="IntentRecognitionResult.Properties"/>.
     */
    LanguageUnderstandingServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.LanguageUnderstandingServiceResponse_JsonResult),

    /**
     * The file name to write logs.
     * Added in version 1.4.0.
     */
    Speech_LogFilename ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Speech_LogFilename),

    /**
     * Identifier used to connect to the backend service.
     * Added in version 1.5.0.
     */
    Conversation_ApplicationId ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Conversation_ApplicationId),

    /**
     * Type of dialog backend to connect to.
     * Added in version 1.7.0.
     */
    Conversation_DialogType ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Conversation_DialogType),

    /**
     * Silence timeout for listening
     * Added in version 1.5.0.
     */
    Conversation_Initial_Silence_Timeout ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Conversation_Initial_Silence_Timeout),

    /**
     * From id to be used on speech recognition activities
     * Added in version 1.5.0.
     */
    Conversation_From_Id ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Conversation_From_Id),

    /**
     *  The auto detect source languages
     * Added in version 1.8.0.
     */
    SpeechServiceConnection_AutoDetectSourceLanguages ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguages),

    /**
     * The auto detect source language result
     * Added in version 1.8.0.
     */
    SpeechServiceConnection_AutoDetectSourceLanguageResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult),

    /**
     * The user id associated to data buffer written by client when using Pull/Push audio mode streams.
     * Added in version 1.5.0.
     */
    DataBuffer_UserId ( com.microsoft.cognitiveservices.speech.internal.PropertyId.DataBuffer_UserId),

    /**
     * The time stamp associated to data buffer written by client when using Pull/Push audio mode streams.
     * The time stamp is a 64-bit value with a resolution of 90 kHz. The same as the presentation timestamp in an MPEG transport stream.
     * See https://en.wikipedia.org/wiki/Presentation_timestamp.
     * Added in version 1.5.0.
     */
    DataBuffer_TimeStamp ( com.microsoft.cognitiveservices.speech.internal.PropertyId.DataBuffer_TimeStamp);

    private PropertyId(com.microsoft.cognitiveservices.speech.internal.PropertyId id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @return the speech property id
     */
    public com.microsoft.cognitiveservices.speech.internal.PropertyId getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.PropertyId id;
}
