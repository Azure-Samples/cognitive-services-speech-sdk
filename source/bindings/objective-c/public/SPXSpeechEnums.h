//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Defines property ids.
 *
 * Changed in version 1.4.0
 */
typedef NS_ENUM(NSUInteger, SPXPropertyId)
{

    /**
     * The Cognitive Services Speech Service subscription key. If you are using an intent recognizer, you need
     * to specify the LUIS endpoint key for your particular LUIS app. Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use SPXSpeechConfiguration.initWithSubscription.
     */
    SPXSpeechServiceConnectionKey = 1000,

    /**
     * The Cognitive Services Speech Service endpoint (url). Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use SPXSpeechConfiguration.initWithEndpoint.
     *
     * NOTE: This endpoint is not the same as the endpoint used to obtain an access token.
     */
    SPXSpeechServiceConnectionEndpoint = 1001,

    /**
     * The Cognitive Services Speech Service region. Under normal circumstances, you shouldn't have to
     * use this property directly. 
     * Instead, use SPXSpeechConfiguration.initWithEndpoint, SPXSpeechConfiguration.initWithHost, or
     * SPXSpeechConfiguration.initWithAuthorizationToken.
     */
    SPXSpeechServiceConnectionRegion = 1002,

    /**
     * The Cognitive Services Speech Service authorization token (aka access token). Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead, use SPXSpeechConfiguration.initWithAuthorizationToken,
     * SPXSpeechRecognizer.authorizationToken, SPXIntentRecognizer.authorizationToken,
     * SPXTranslationRecognizer.authorizationToken.
     */
    SPXSpeechServiceAuthorizationToken = 1003,

    /**
     * The Cognitive Services Speech Service authorization type. Currently unused.
     */
    SPXSpeechServiceAuthorizationType = 1004,

    /**
     * The Cognitive Services Custom Speech Service endpoint id.
     *
     * NOTE: The endpoint id is available in the Custom Speech Portal, listed under Endpoint Details.
     */
    SPXSpeechServiceConnectionEndpointId = 1005,

    /**
     * The Cognitive Services Speech Service host (url). Under normal circumstances, you shouldn't
     * have to use this property directly.
     * Instead, use SPXSpeechConfiguration.initWithHost.
     */
    SPXSpeechServiceConnectionHost = 1006,

    /**
     * The host name of the proxy server. Not implemented yet.
     *
     * NOTE: Added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyHostName = 1100,

    /**
     * The port of the proxy server. Not implemented yet.
     *
     * NOTE: Added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyPort = 1101,

    /**
     * The user name of the proxy server. Not implemented yet.
     *
     * NOTE: Added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyUserName = 1102,

    /**
     * The password of the proxy server. Not implemented yet.
     *
     * NOTE: Added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyPassword = 1103,

    /**
     * The URL string built from speech configuration.
     * This property is intended to be read-only. The SDK is using it internally.
     *
     * NOTE: Added in version 1.5.0.
     */
    SPXSpeechServiceConnectionUrl = 1104, 

    /**
     * The list of comma separated languages (BCP-47 format) used as target translation languages. Under normal circumstances,
     * you shouldn't have to use this property directly.
     * Instead use SPXSpeechTranslationConfiguration.addTargetLanguage
     * and the read-only SPXSpeechTranslationConfiguration.targetLanguages and SPXTranslationRecognizer.targetLangauges
     * collections.
     */
    SPXSpeechServiceConnectionTranslationToLanguages = 2000,

    /**
     * The name of the Cognitive Service Text to Speech Service voice. Under normal circumstances, you shouldn't have to use this
     * property directly.
     * Instead use SPXSpeechTranslationConfiguration.voiceName.
     *
     * NOTE: Valid voice names can be found <a href="https://aka.ms/csspeech/voicenames">here</a>.
     */
    SPXSpeechServiceConnectionTranslationVoice = 2001,

    /**
     * Translation features. For internal use.
     */
    SPXSpeechServiceConnectionTranslationFeatures = 2002,

    /**
     * The Language Understanding Service region. Under normal circumstances, you shouldn't have to use this property directly.
     * Instead use SPXLanguageUnderstandingModel.
     */
    SPXSpeechServiceConnectionIntentRegion = 2003,

    /**
     * The Cognitive Services Speech Service recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
     * This property is intended to be read-only. The SDK is using it internally.
     */
    SPXSpeechServiceConnectionRecognitionMode = 3000,

    /**
     * The spoken language to be recognized (in BCP-47 format).
     */
    SPXSpeechServiceConnectionRecognitionLanguage = 3001,

    /**
     * The session id. This id is a universally unique identifier (aka UUID) representing a specific binding of an audio input stream
     * and the underlying speech recognition instance to which it is bound. Under normal circumstances, you shouldn't have to use this
     * property directly.
     * Instead, use SPXSessionEventArgs.sessionId.
     */
    SPXSpeechSessionId = 3002,

    /**
     * The spoken language to be synthesized (e.g. en-US)
     */
    SPXSpeechServiceConnectionSynthesisLanguage = 3100,

    /**
     * The name of the voice to be used for speech synthesis
     */
    SPXSpeechServiceConnectionSynthesisVocie = 3101,

    /**
     * The string to specify speech synthesis output audio format (e.g. riff-16khz-16bit-mono-pcm)
     */
    SPXSpeechServiceConnectionSynthesisOutputFormat = 3102,

    /**
     * The initial silence timeout value (in milliseconds) used by the service.
     * Added in version 1.5.0
     */
    SPXSpeechServiceConnectionInitialSilenceTimeoutMs = 3200,

    /**
     * The end silence timeout value (in milliseconds) used by the service.
     * Added in version 1.5.0
     */
    SPXSpeechServiceConnectionEndSilenceTimeoutMs = 3201,

    /**
     * A boolean value specifying whether audio logging is enabled in the service or not.
     * Added in version 1.5.0
     */
    SPXSpeechServiceConnectionEnableAudioLogging = 3202,

    /**
     * The source language candidates used for auto language detection
     * Added in version 1.12.0
     */
    SPXSpeechServiceConnectionAutoDetectSourceLanguages = 3300,

    /**
     * The auto language detection result
     * Added in version 1.12.0
     */
    SPXSpeechServiceConnectionAutoDetectSourceLanguageResult = 3301,

    /**
     * The requested Cognitive Services Speech Service response output format (simple or detailed). Not implemented yet.
     */
    SPXSpeechServiceResponseRequestDetailedResultTrueFalse = 4000,

    /**
     * The requested Cognitive Services Speech Service response output profanity level. Currently unused.
     */
    SPXSpeechServiceResponseRequestProfanityFilterTrueFalse = 4001,

    /**
     * The requested Cognitive Services Speech Service response output profanity setting.
     * Allowed values are "masked", "removed", and "raw".
     * Added in version 1.5.0.
     */
    SPXSpeechServiceResponseProfanityOption = 4002,

    /**
     * A string value specifying which post processing option should be used by service.
     * Allowed values are "TrueText".
     * Added in version 1.5.0
     */
    SPXSpeechServiceResponsePostProcessingOption = 4003,

    /**
     * A boolean value specifying whether to include word-level timestamps in the response result.
     * Added in version 1.5.0
     */
    SPXSpeechServiceResponseRequestWordLevelTimestamps = 4004,

    /**
     * The number of times a word has to be in partial results to be returned.
     * Added in version 1.5.0
     */
    SPXSpeechServiceResponseStablePartialResultThreshold = 4005,

    /**
     * A string value specifying the output format option in the response result. Internal use only.
     * Added in version 1.5.0.
     */
    SPXSpeechServiceResponseOutputFormatOption = 4006,

    /**
     * A boolean value to request for stabilizing translation partial results by omitting words in the end.
     * Added in version 1.5.0.
     */
    SPXSpeechServiceResponseTranslationRequestStablePartialResult = 4100,

    /**
     * The Cognitive Services Speech Service response output (in JSON format). This property is available on recognition result objects only.
     */
    SPXSpeechServiceResponseJsonResult = 5000,

    /**
     * The Cognitive Services Speech Service error details (in JSON format). Under normal circumstances, you shouldn't have to
     * use this property directly.
     * Instead, use SPXCancellationDetails.errorDetails.
     */
    SPXSpeechServiceResponseJsonErrorDetails = 5001,

    /**
     * The recognition latency in milliseconds. Read-only, available on final speech/translation/intent results.
     * This measures the latency between when an audio input is received by the SDK, and the moment the final result is received from the service.
     * The SDK computes the time difference between the last audio fragment from the audio input that is contributing to the final result, and the time the final result is received from the speech service.
     * Added in version 1.3.0.
     */
    SPXSpeechServiceResponseRecognitionLatencyMs = 5002,

    /**
     * The cancellation reason. Currently unused.
     */
    SPXCancellationDetailsReason = 6000,

    /**
     * The cancellation text. Currently unused.
     */
    SPXCancellationDetailsReasonText = 6001,

    /**
     * The cancellation detailed text. Currently unused.
     */
    SPXCancellationDetailsReasonDetailedText = 6002,

    /**
     * The Language Understanding Service response output (in JSON format). Available via IntentRecognitionResult.Properties.
     */
    SPXLanguageUnderstandingServiceResponseJsonResult = 7000,

    /**
     * The file name to write logs.
     *
     * NOTE: Added in version 1.4.0.
     */
    SPXSpeechLogFilename = 9001
};

/**
  * Defines possible values of output format.
  */
typedef NS_ENUM(NSUInteger, SPXOutputFormat)
{
    /**
     * Simple output format.
     */
    SPXOutputFormat_Simple = 0,

    /**
     * Detailed output format.
     */
    SPXOutputFormat_Detailed = 1
};

/**
  * Specifies the possible reasons a recognition result might be generated.
  */
typedef NS_ENUM(NSUInteger, SPXResultReason)
{
    /**
     * Indicates speech could not be recognized. More details can be found in the SPXNoMatchDetails object.
     */
    SPXResultReason_NoMatch = 0,

    /**
     * Indicates that the recognition was canceled. More details can be found using the SPXCancellationDetails object.
     */
    SPXResultReason_Canceled = 1,

    /**
     * Indicates the speech result contains hypothesis text as an intermediate result.
     */
    SPXResultReason_RecognizingSpeech = 2,

    /**
     * Indicates the speech result contains final text that has been recognized.
     * Speech recognition is now complete for this phrase.
     */
    SPXResultReason_RecognizedSpeech = 3,

    /**
     * Indicates the intent result contains hypothesis text as an intermediate result.
     */
    SPXResultReason_RecognizingIntent = 4,

    /**
     * Indicates the intent result contains final text and intent.
     * Speech recognition and intent determination are now complete for this phrase.
     */
    SPXResultReason_RecognizedIntent = 5,

    /**
     * Indicates the translation result contains hypothesis text and translation(s) as an intermediate result.
     */
    SPXResultReason_TranslatingSpeech = 6,

    /**
     * Indicates the translation result contains final text and corresponding translation(s).
     * Speech recognition and translation are now complete for this phrase.
     */
    SPXResultReason_TranslatedSpeech = 7,

    /**
     * Indicates the synthesized audio result contains a non-zero amount of audio data
     */
    SPXResultReason_SynthesizingAudio = 8,

    /**
     * Indicates the synthesized audio is now complete for this phrase.
     */
    SPXResultReason_SynthesizingAudioCompleted = 9,

    /**
     * Indicates the speech synthesis is now started.
     */
    SPXResultReason_SynthesizingAudioStarted = 12
};

/**
  * Defines the possible reasons a recognition result might be canceled.
  */
typedef NS_ENUM(NSUInteger, SPXCancellationReason)
{
    /**
     * Indicates that an error occurred during speech recognition.
     */
    SPXCancellationReason_Error = 1,

    /**
     * Indicates that the end of the audio stream was reached.
     */
    SPXCancellationReason_EndOfStream = 2
};

/**
  * Defines error code in case that CancellationReason is set to Error.
  * Added in version 1.1.0.
  */
typedef NS_ENUM(NSUInteger, SPXCancellationErrorCode)
{
    /**
     * No error.
     * If SPXCancellationReason is SPXCancellationReason_EndOfStream, SPXCancellationErrorCode 
     * is set to SPXCancellationErrorCode_NoError.
     */
    SPXCancellationErrorCode_NoError = 0,

    /**
     * Indicates an authentication error.
     * An authentication error occurs if subscription key or authorization token is invalid, expired, 
     * or does not match the region being used.
     */
    SPXCancellationErrorCode_AuthenticationFailure = 1,

    /**
     * Indicates that one or more recognition parameters are invalid or the audio format is not supported.
     */
    SPXCancellationErrorCode_BadRequest = 2,

    /**
     * Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
     */
    SPXCancellationErrorCode_TooManyRequests = 3,

    /**
     * Indicates that the free subscription used by the request ran out of quota.
     */
    SPXCancellationErrorCode_Forbidden = 4,

    /**
     * Indicates a connection error.
     */
    SPXCancellationErrorCode_ConnectionFailure = 5,

    /**
     * Indicates a time-out error when waiting for response from service.
     */
    SPXCancellationErrorCode_ServiceTimeout = 6,

    /**
     * Indicates that an error is returned by the service.
     */
    SPXCancellationErrorCode_ServiceError = 7,

    /**
     * Indicates that the service is currently unavailable.
     */
    SPXCancellationErrorCode_ServiceUnavailable = 8,

    /**
     * Indicates an unexpected runtime error.
     */
    SPXCancellationErrorCode_RuntimeError = 9
};

/**
  * Defines the possible reasons a recognition result might not have been recognized.
  */
typedef NS_ENUM(NSUInteger, SPXNoMatchReason)
{
    /**
     * Indicates that speech was detected, but not recognized.
     */
    SPXNoMatchReason_NotRecognized = 1,

    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     */
    SPXNoMatchReason_InitialSilenceTimeout = 2,

    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     */
    SPXNoMatchReason_InitialBabbleTimeout = 3,

    /**
     * Indicates that the spotted keyword has been rejected by the keyword verification service.
     * Added in version 1.5.0.
     */
    SPXNoMatchReason_KeywordNotRecognized = 4
};

/**
 *  Defines channels used to send service properties.
 *  Added in version 1.5.0.
 */
typedef NS_ENUM(NSUInteger, SPXServicePropertyChannel)
{
    /**
     * Uses URI query parameter to pass property settings to service.
     */
    SPXServicePropertyChannel_UriQueryParameter = 0
};

/**
 *  Defines the setting for the profanity filter.
 *  Added in version 1.5.0.
 */
typedef NS_ENUM(NSUInteger, SPXSpeechConfigProfanityOption)
{
    /**
     * Mask profanity.
     */
    SPXSpeechConfigProfanityOption_ProfanityMasked = 0,

    /**
     * Remove profanity.
     */
    SPXSpeechConfigProfanityOption_ProfanityRemoved = 1,

    /**
     * Don't filter profanity.
     */
    SPXSpeechConfigProfanityOption_ProfanityRaw = 2,
};

/**
 * Defines the possible speech synthesis output audio format.
 * 
 * Added in version 1.7.0
 */
typedef NS_ENUM(NSUInteger, SPXSpeechSynthesisOutputFormat)
{
    /**
     * raw-8khz-8bit-mono-mulaw
     */
    SPXSpeechSynthesisOutputFormat_Raw8Khz8BitMonoMULaw = 1,

    /**
     * riff-16khz-16kbps-mono-siren
     */
    SPXSpeechSynthesisOutputFormat_Riff16Khz16KbpsMonoSiren = 2,

    /**
     * audio-16khz-16kbps-mono-siren
     */
    SPXSpeechSynthesisOutputFormat_Audio16Khz16KbpsMonoSiren = 3,

    /**
     * audio-16khz-32kbitrate-mono-mp3
     */
    SPXSpeechSynthesisOutputFormat_Audio16Khz32KBitRateMonoMp3 = 4,

    /**
     * audio-16khz-128kbitrate-mono-mp3
     */
    SPXSpeechSynthesisOutputFormat_Audio16Khz128KBitRateMonoMp3 = 5,

    /**
     * audio-16khz-64kbitrate-mono-mp3
     */
    SPXSpeechSynthesisOutputFormat_Audio16Khz64KBitRateMonoMp3 = 6,

    /**
     * audio-24khz-48kbitrate-mono-mp3
     */
    SPXSpeechSynthesisOutputFormat_Audio24Khz48KBitRateMonoMp3 =7,

    /**
     * audio-24khz-96kbitrate-mono-mp3
     */
    SPXSpeechSynthesisOutputFormat_Audio24Khz96KBitRateMonoMp3 = 8,

    /**
     * audio-24khz-160kbitrate-mono-mp3
     */
    SPXSpeechSynthesisOutputFormat_Audio24Khz160KBitRateMonoMp3 = 9,

    /**
     * raw-16khz-16bit-mono-truesilk
     */
    SPXSpeechSynthesisOutputFormat_Raw16Khz16BitMonoTrueSilk = 10,

    /**
     * riff-16khz-16bit-mono-pcm
     */
    SPXSpeechSynthesisOutputFormat_Riff16Khz16BitMonoPcm = 11,

    /**
     * riff-8khz-16bit-mono-pcm
     */
    SPXSpeechSynthesisOutputFormat_Riff8Khz16BitMonoPcm = 12,

    /**
     * riff-24khz-16bit-mono-pcm
     */
    SPXSpeechSynthesisOutputFormat_Riff24Khz16BitMonoPcm = 13,

    /**
     * riff-8khz-8bit-mono-mulaw
     */
    SPXSpeechSynthesisOutputFormat_Riff8Khz8BitMonoMULaw = 14,

    /**
     * raw-16khz-16bit-mono-pcm
     */
    SPXSpeechSynthesisOutputFormat_Raw16Khz16BitMonoPcm = 15,

    /**
     * raw-24khz-16bit-mono-pcm
     */
    SPXSpeechSynthesisOutputFormat_Raw24Khz16BitMonoPcm = 16,

    /**
     * raw-8khz-16bit-mono-pcm
     */
    SPXSpeechSynthesisOutputFormat_Raw8Khz16BitMonoPcm = 17
};

/**
 * Defines the possible status of audio data stream.
 * 
 * Added in version 1.7.0
 */
typedef NS_ENUM(NSUInteger, SPXStreamStatus)
{
    /**
     * The audio data stream status is unknown
     */
    SPXStreamStatus_Unknown = 0,

    /** <summary>
     * The audio data stream contains no data
     */
    SPXStreamStatus_NoData = 1,

    /**
     * The audio data stream contains partial data of a speak request
     */
    SPXStreamStatus_PartialData = 2,

    /**
     * The audio data stream contains all data of a speak request
     */
    SPXStreamStatus_AllData = 3,

    /**
     * The audio data stream was cancelled
     */
    SPXStreamStatus_Canceled = 4
};
