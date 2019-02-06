//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines property ids.
 * Changed in version 1.1.0.
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
     * The recognition latency. Read-only, available on final speech/translation/intent results.
     * This measures the latency between when an audio input is received by the SDK, and the moment the final result is received from the service.
     * The SDK computes the time difference between the last audio fragment from the audio input that is contributing to the final result, and the time the final result is received from the speech service.
     * The value is in ticks, represented as a string. A single tick represents one hundred nanoseconds or one ten-millionth of a second.
     * Added in version 1.3.0.
     */
    SpeechServiceResponse_RecognitionLatency ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RecognitionLatency),

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
    LanguageUnderstandingServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.LanguageUnderstandingServiceResponse_JsonResult);

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
