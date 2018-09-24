//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
  * Defines property ids.
  */
typedef NS_ENUM(NSUInteger, SPXPropertyId)
{
    /**
      * Subscription key.
      */
    SPXSpeechServiceConnectionKey = 1000,

    /**
      * Endpoint.
      */
    SPXSpeechServiceConnectionEndpoint = 1001,

    /**
      * Region.
      */
    SPXSpeechServiceConnectionRegion = 1002,

    /**
      * Authorization token.
      */
    SPXSpeechServiceAuthorizationToken = 1003,

    /**
      * Authorization type.
      */
    SPXSpeechServiceAuthorizationType = 1004,

    /**
      * Endpoint ID.
      */
    SPXSpeechServiceConnectionEndpointId = 1005,

    /**
      * Translation to languages.
      */
    SPXSpeechServiceConnectionTranslationToLanguages = 2000,

    /**
      * Translation output voice.
      */
    SPXSpeechServiceConnectionTranslationVoice = 2001,

    /**
      * Translation features.
      */
    SPXSpeechServiceConnectionTranslationFeatures = 2002,

    /**
      * Intent region.
      */
    SPXSpeechServiceConnectionIntentRegion = 2003,

    /**
      * Recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
      */
    SPXSpeechServiceConnectionRecognitionMode = 3000,

    /**
      * Recognition language.
      */
    SPXSpeechServiceConnectionRecognitionLanguage = 3001,

    /**
      * Session ID.
      */
    SPXSpeechSessionId = 3002,

    /**
      * Detailed result required.
      */
    SPXSpeechServiceResponseRequestDetailedResultTrueFalse = 4000,

    /**
      * Profanity filtering required.
      */
    SPXSpeechServiceResponseRequestProfanityFilterTrueFalse = 4001,

    /**
      * JSON result of speech recognition service.
      */
    SPXSpeechServiceResponseJsonResult = 5000,

    /**
      * Error details.
      */
    SPXSpeechServiceResponseJsonErrorDetails = 5001,

    /**
      * Cancellation reason.
      */
    SPXCancellationDetailsReason = 6000,

    /**
      * Cancellation text.
      */
    SPXCancellationDetailsReasonText = 6001,

    /**
      * Cancellation detailed text.
      */
    SPXCancellationDetailsReasonDetailedText = 6002,

    /**
      * JSON result of language understanding service.
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
      * Indicates the translation result contains hypothesis text and its translation(s) as an intermediate result.
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
      * Indicates that an error occurred during speech recognition. The ErrorDetails property contains a detailed error response.
      */
    SPXCancellationReason_Error = 1,

    /**
      * Indicates that the end of the audio stream was reached.
      */
    SPXCancellationReason_EndOfStream = 2,
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

