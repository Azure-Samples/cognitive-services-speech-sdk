//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
  * Defines property ids.
  * Changed in version 1.1.0
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
      * NOTE: This endpoint is not the same as the endpoint used to obtain an access token.
      */
    SPXSpeechServiceConnectionEndpoint = 1001,

    /**
      * The Cognitive Services Speech Service region. Under normal circumstances, you shouldn't have to
      * use this property directly. 
      * Instead, use SPXSpeechConfiguration.initWithEndpoint or SpeechConfiguration.initWithAuthorizationToken.
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
      * The Cognitive Services Custom Speech Service endpoint id. Under normal circumstances, you shouldn't
      * have to use this property directly.
      * Instead use SPXSpeechConfiguration.endpointId.
      * NOTE: The endpoint id is available in the Custom Speech Portal, listed under Endpoint Details.
      */
    SPXSpeechServiceConnectionEndpointId = 1005,

    /**
     * The host name of the proxy server. Not implemented yet.
     * NOTE: This property was added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyHostName = 1100,

    /**
     * The port of the proxy server. Not implemented yet.
     * NOTE: This property was added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyPort = 1101,

    /**
     * The user name of the proxy server. Not implemented yet.
     * NOTE: This property was added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyUserName = 1102,

    /**
     * The password of the proxy server. Not implemented yet.
     * NOTE: This property was added in version 1.1.0.
     */
    SPXSpeechServiceConnectionProxyPassword = 1103,

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
      * The spoken language to be recognized (in BCP-47 format). Under normal circumstances, you shouldn't have to use this property directly.
      * Instead, use SPXSpeechConfiguration.speechRecognitionLanguage.
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
      * The requested Cognitive Services Speech Service response output format (simple or detailed). Not implemented yet.
      */
    SPXSpeechServiceResponseRequestDetailedResultTrueFalse = 4000,

    /**
      * The requested Cognitive Services Speech Service response output profanity level. Currently unused.
      */
    SPXSpeechServiceResponseRequestProfanityFilterTrueFalse = 4001,

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
    SPXLanguageUnderstandingServiceResponseJsonResult = 7000
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
    SPXResultReason_SynthesizingAudioCompleted = 9
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
      */
    SPXCancellationErrorCode_NoError = 0,

    /**
      * Indicates an authentication error.
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
    SPXNoMatchReason_InitialBabbleTimeout = 3
};

