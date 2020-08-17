//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines property ids.
 * Changed in version 1.8.0.
 */
public enum PropertyId {

    /**
     * The Cognitive Services Speech Service subscription key. If you are using an intent recognizer, you need
     * to specify the LUIS endpoint key for your particular LUIS app. Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromSubscription"/>.
     */
    SpeechServiceConnection_Key (1000),

    /**
     * The Cognitive Services Speech Service endpoint (url). Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromEndpoint"/>.
     * NOTE: This endpoint is not the same as the endpoint used to obtain an access token.
     */
    SpeechServiceConnection_Endpoint (1001),

    /**
     * The Cognitive Services Speech Service region. Under normal circumstances, you shouldn't have to
     * use this property directly.
     * Instead, use <see cref="SpeechConfig.fromSubscription"/>, <see cref="SpeechConfig.fromEndpoint"/>, <see cref="SpeechConfig.fromHost"/>,
     * <see cref="SpeechConfig.fromAuthorizationToken"/>.
     */
    SpeechServiceConnection_Region (1002),

    /**
     * The Cognitive Services Speech Service authorization token (aka access token). Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromAuthorizationToken"/>,
     * <see cref="SpeechRecognizer.setAuthorizationToken"/>, <see cref="IntentRecognizer.setAuthorizationToken"/>,
     * <see cref="TranslationRecognizer.setAuthorizationToken"/>.
     */
    SpeechServiceAuthorization_Token (1003),

    /**
     * The Cognitive Services Speech Service authorization type. Currently unused.
     */
    SpeechServiceAuthorization_Type (1004),

    /**
     * The Cognitive Services Custom Speech Service endpoint id. Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead use <see cref="SpeechConfig.setEndpointId"/>.
     * NOTE: The endpoint id is available in the Custom Speech Portal, listed under Endpoint Details.
     */
    SpeechServiceConnection_EndpointId (1005),

    /**
     * The Cognitive Services Speech Service host (url). Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use <see cref="SpeechConfig.fromHost"/>.
     */
    SpeechServiceConnection_Host (1006),

    /**
     * The host name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyHostName (1100),

    /**
     * The port of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyPort (1101),

    /**
     * The user name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyUserName (1102),

    /**
     * The password of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setProxy"/>.
     * NOTE: This property id was added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyPassword (1103),

    /**
     * The URL string built from speech configuration.
     * This property is intended to be read-only. The SDK is using it internally.
     * NOTE: Added in version 1.5.0.
     */
    SpeechServiceConnection_Url (1104),

    /**
     * The list of comma separated languages (BCP-47 format) used as target translation languages. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechTranslationConfig.addTargetLanguage"/>,
     * <see cref="SpeechTranslationConfig.getTargetLanguages"/>, <see cref="TranslationRecognizer.getTargetLanguages"/>.
     */
    SpeechServiceConnection_TranslationToLanguages (2000),

    /**
     * The name of the Cognitive Service Text to Speech Service voice. Under normal circumstances, you shouldn't have to use this
     * property directly. Instead use <see cref="SpeechTranslationConfig.setVoiceName"/>.
     * NOTE: Valid voice names can be found <a href="https://aka.ms/csspeech/voicenames">here</a>.
     */
    SpeechServiceConnection_TranslationVoice (2001),

    /**
     * Translation features. For internal use.
     */
    SpeechServiceConnection_TranslationFeatures (2002),

    /**
     * The Language Understanding Service region. Under normal circumstances, you shouldn't have to use this property directly.
     * Instead, use <see cref="LanguageUnderstandingModel"/>.
     */
    SpeechServiceConnection_IntentRegion (2003),

    /**
     * The Cognitive Services Speech Service recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
     * This property is intended to be read-only. The SDK is using it internally.
     */
    SpeechServiceConnection_RecoMode (3000),

    /**
     * The spoken language to be recognized (in BCP-47 format). Under normal circumstances, you shouldn't have to use this property directly.
     * Instead, use <see cref="SpeechConfig.setSpeechRecognitionLanguage"/>.
     */
    SpeechServiceConnection_RecoLanguage (3001),

    /**
     * The session id. This id is a universally unique identifier (aka UUID) representing a specific binding of an audio input stream
     * and the underlying speech recognition instance to which its bound. Under normal circumstances, you shouldn't have to use this
     * property directly.
     * Instead use <see cref="SessionEventArgs.getSessionId"/>.
     */
    Speech_SessionId (3002),

    /**
     * The spoken language to be synthesized (e.g. en-US)
     * Added in version 1.7.0
     */
    SpeechServiceConnection_SynthLanguage (3100),

    /**
     * The name of the TTS voice to be used for speech synthesis
     * Added in version 1.7.0
     */
    SpeechServiceConnection_SynthVoice (3101),

    /**
     * The string to specify TTS output audio format (e.g. riff-16khz-16bit-mono-pcm)
     * Added in version 1.7.0
     */
    SpeechServiceConnection_SynthOutputFormat (3102),

    /**
     *  The initial silence timeout value (in milliseconds) used by the service.
     *  Added in version 1.5.0
     */
    SpeechServiceConnection_InitialSilenceTimeoutMs (3200),

    /**
     *  The end silence timeout value (in milliseconds) used by the service.
     *  Added in version 1.5.0
     */
    SpeechServiceConnection_EndSilenceTimeoutMs (3201),

    /**
     *  A boolean value specifying whether audio logging is enabled in the service or not.
     *  Added in version 1.5.0
     */
    SpeechServiceConnection_EnableAudioLogging (3202),

    /**
     * The requested Cognitive Services Speech Service response output format (simple or detailed). Under normal circumstances, you shouldn't have
     * to use this property directly.
     * Instead use <see cref="SpeechConfig.setOutputFormat"/>.
     */
    SpeechServiceResponse_RequestDetailedResultTrueFalse (4000),

    /**
     * The requested Cognitive Services Speech Service response output profanity level. Currently unused.
     */
    SpeechServiceResponse_RequestProfanityFilterTrueFalse (4001),

    /**
     *  The requested Cognitive Services Speech Service response output profanity setting.
     *  Allowed values are "masked", "removed", and "raw".
     *  Added in version 1.5.0.
     */
    SpeechServiceResponse_ProfanityOption (4002),

    /**
     *  A string value specifying which post processing option should be used by service.
     *  Allowed values are "TrueText".
     *  Added in version 1.5.0
     */
    SpeechServiceResponse_PostProcessingOption (4003),

    /**
     *  A boolean value specifying whether to include word-level timestamps in the response result.
     *  Added in version 1.5.0
     */
    SpeechServiceResponse_RequestWordLevelTimestamps (4004),

    /**
     *  The number of times a word has to be in partial results to be returned.
     *  Added in version 1.5.0
     */
    SpeechServiceResponse_StablePartialResultThreshold (4005),

    /**
     *  A string value specifying the output format option in the response result. Internal use only.
     *  Added in version 1.5.0.
     */
    SpeechServiceResponse_OutputFormatOption (4006),

    /**
     *  A boolean value to request for stabilizing translation partial results by omitting words in the end.
     *  Added in version 1.5.0.
     */
    SpeechServiceResponse_TranslationRequestStablePartialResult (4100),

    /**
     * The Cognitive Services Speech Service response output (in JSON format). This property is available on recognition result objects only.
     */
    SpeechServiceResponse_JsonResult (5000),

    /**
     * The Cognitive Services Speech Service error details (in JSON format). Under normal circumstances, you shouldn't have to
     * use this property directly.
     * Instead, use <see cref="CancellationDetails.getErrorDetails"/>.
     */
    SpeechServiceResponse_JsonErrorDetails (5001),

    /**
     * The recognition latency in milliseconds. Read-only, available on final speech/translation/intent results.
     * This measures the latency between when an audio input is received by the SDK, and the moment the final result is received from the service.
     * The SDK computes the time difference between the last audio fragment from the audio input that is contributing to the final result, and the time the final result is received from the speech service.
     * Added in version 1.3.0.
     */
    SpeechServiceResponse_RecognitionLatencyMs (5002),

    /**
     * The cancellation reason. Currently unused.
     */
    CancellationDetails_Reason (6000),

    /**
     * The cancellation text. Currently unused.
     */
    CancellationDetails_ReasonText (6001),

    /**
     * The cancellation detailed text. Currently unused.
     */
    CancellationDetails_ReasonDetailedText (6002),

    /**
     * The Language Understanding Service response output (in JSON format). Available via <see cref="IntentRecognitionResult.Properties"/>.
     */
    LanguageUnderstandingServiceResponse_JsonResult (7000),

    /**
     * Playback buffer length, default is 50 milliseconds.
     */
    AudioConfig_PlaybackBufferLengthInMs (8006),

    /**
     * The file name to write logs.
     * Added in version 1.4.0.
     */
    Speech_LogFilename (9001),

    /**
     * Identifier used to connect to the backend service.
     * Added in version 1.5.0.
     */
    Conversation_ApplicationId (10000),

    /**
     * Type of dialog backend to connect to.
     * Added in version 1.7.0.
     */
    Conversation_DialogType (10001),

    /**
     * Silence timeout for listening
     * Added in version 1.5.0.
     */
    Conversation_Initial_Silence_Timeout (10002),

    /**
     * From id to be used on speech recognition activities
     * Added in version 1.5.0.
     */
    Conversation_From_Id (10003),

    /**
     * ConversationId for the session.
     * Added in version 1.8.0.
     */
    Conversation_Conversation_Id (10004),

    /**
     * Comma separated list of custom voice deployment ids.
     * Added in version 1.8.0.
     */
    Conversation_Custom_Voice_Deployment_Ids (10005),

    /**
     * Speech activity template, stamp properties in the template on the activity generated by the service for speech.
     * Added in version 1.10.0.
     */
    Conversation_Speech_Activity_Template (10006),

    /**
     *  The auto detect source languages
     * Added in version 1.8.0.
     */
    SpeechServiceConnection_AutoDetectSourceLanguages (3300),

    /**
     * The auto detect source language result
     * Added in version 1.8.0.
     */
    SpeechServiceConnection_AutoDetectSourceLanguageResult (3301),

    /**
     * The user id associated to data buffer written by client when using Pull/Push audio mode streams.
     * Added in version 1.5.0.
     */
    DataBuffer_UserId (11002),

    /**
     * The time stamp associated to data buffer written by client when using Pull/Push audio mode streams.
     * The time stamp is a 64-bit value with a resolution of 90 kHz. The same as the presentation timestamp in an MPEG transport stream.
     * See https://en.wikipedia.org/wiki/Presentation_timestamp.
     * Added in version 1.5.0.
     */
    DataBuffer_TimeStamp (11001),

    /**
     * The reference text of the audio for pronunciation evaluation.
     * For this and the following pronunciation assessment parameters, see
     * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters for details.
     * Under normal circumstances, you shouldn't have to use this property directly.
     * Added in version 1.14.0
     */
    PronunciationAssessment_ReferenceText (12001),

    /**
     * The point system for pronunciation score calibration (FivePoint or HundredMark).
     * Under normal circumstances, you shouldn't have to use this property directly.
     * Added in version 1.14.0
     */
    PronunciationAssessment_GradingSystem (12002),

    /**
     * The pronunciation evaluation granularity (Phoneme, Word, or FullText).
     * Under normal circumstances, you shouldn't have to use this property directly.
     * Added in version 1.14.0
     */
    PronunciationAssessment_Granularity (12003),

    /**
     * Defines if enable miscue calculation. 
     * With this enabled, the pronounced words will be compared to the reference text, 
     * and will be marked with omission/insertion based on the comparison. The default setting is False.
     * Under normal circumstances, you shouldn't have to use this property directly.
     * Added in version 1.14.0
     */
    PronunciationAssessment_EnableMiscue (12005),

    /**
     * A GUID indicating a customized pronunciation score system. 
     * Under normal circumstances, you shouldn't have to use this property directly.
     * Added in version 1.14.0
     */
    PronunciationAssessment_ScenarioId (12006),

    /**
     * The json string of pronunciation assessment parameters
     * Under normal circumstances, you shouldn't have to use this property directly.
     * Added in version 1.14.0
     */
    PronunciationAssessment_Json (12009),

    /**
     * Pronunciation assessment parameters.
     * This property is intended to be read-only. The SDK is using it internally.
     * Added in version 1.14.0
     */
    PronunciationAssessment_Params (12010);

    private PropertyId(int id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @return the speech property id
     */
    public int getValue() { return this.id; }

    private final int id;
}
