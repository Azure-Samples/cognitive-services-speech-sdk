//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_enums.h: Public API declarations for C++ enumerations
//

#pragma once

#include <string>
#include <speechapi_cxx_common.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

/// <summary>
/// Defines speech property ids.
/// Changed in version 1.1.0.
/// </summary>
enum class PropertyId
{
    /// <summary>
    /// The Cognitive Services Speech Service subscription key. If you are using an intent recognizer, you need
    /// to specify the LUIS endpoint key for your particular LUIS app. Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromSubscription"/>.
    /// </summary>
    SpeechServiceConnection_Key = 1000,

    /// <summary>
    /// The Cognitive Services Speech Service endpoint (url). Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromEndpoint"/>.
    /// NOTE: This endpoint is not the same as the endpoint used to obtain an access token.
    /// </summary>
    SpeechServiceConnection_Endpoint = 1001,

    /// <summary>
    /// The Cognitive Services Speech Service region. Under normal circumstances, you shouldn't have to
    /// use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromSubscription"/>, <see cref="SpeechConfig::FromEndpoint"/>,
    /// <see cref="SpeechConfig::FromAuthorizationToken"/>.
    /// </summary>
    SpeechServiceConnection_Region = 1002,

    /// <summary>
    /// The Cognitive Services Speech Service authorization token (aka access token). Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::FromAuthorizationToken"/>,
    /// <see cref="SpeechRecognizer::SetAuthorizationToken"/>, <see cref="IntentRecognizer::SetAuthorizationToken"/>,
    /// <see cref="TranslationRecognizer::SetAuthorizationToken"/>.
    /// </summary>
    SpeechServiceAuthorization_Token = 1003,

    /// <summary>
    /// The Cognitive Services Speech Service authorization type. Currently unused.
    /// </summary>
    SpeechServiceAuthorization_Type = 1004,

    /// <summary>
    /// The Cognitive Services Custom Speech Service endpoint id. Under normal circumstances, you shouldn't
    /// have to use this property directly.
    /// Instead use <see cref="SpeechConfig::SetEndpointId"/>.
    /// NOTE: The endpoint id is available in the Custom Speech Portal, listed under Endpoint Details.
    /// </summary>
    SpeechServiceConnection_EndpointId = 1005,

    /// <summary>
    /// The host name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyHostName = 1100,

    /// <summary>
    /// The port of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyPort = 1101,

    /// <summary>
    /// The user name of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyUserName = 1102,

    /// <summary>
    /// The password of the proxy server used to connect to the Cognitive Services Speech Service. Under normal circumstances,
    /// you shouldn't have to use this property directly.
    /// Instead, use <see cref="SpeechConfig::SetProxy"/>.
    /// NOTE: This property id was added in version 1.1.0.
    /// </summary>
    SpeechServiceConnection_ProxyPassword = 1103,

    /// <summary>
    /// The list of comma separated languages used as target translation languages. Under normal circumstances,
    /// you shouldn't have to use this property directly. Instead use <see cref="SpeechTranslationConfig::AddTargetLanguage"/>
    /// and <see cref="SpeechTranslationConfig::GetTargetLanguages"/>.
    /// </summary>
    SpeechServiceConnection_TranslationToLanguages = 2000,

    /// <summary>
    /// The name of the Cognitive Service Text to Speech Service voice. Under normal circumstances, you shouldn't have to use this
    /// property directly. Instead use <see cref="SpeechTranslationConfig::SetVoiceName"/>.
    /// NOTE: Valid voice names can be found <a href="https://aka.ms/csspeech/voicenames">here</a>.
    /// </summary>
    SpeechServiceConnection_TranslationVoice = 2001,

    /// <summary>
    /// Translation features. For internal use.
    /// </summary>
    SpeechServiceConnection_TranslationFeatures = 2002,

    /// <summary>
    /// The Language Understanding Service region. Under normal circumstances, you shouldn't have to use this property directly.
    /// Instead use <see cref="LanguageUnderstandingModel"/>.
    /// </summary>
    SpeechServiceConnection_IntentRegion = 2003,

    /// <summary>
    /// The Cognitive Services Speech Service recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
    /// This property is intended to be read-only. The SDK is using it internally.
    /// </summary>
    SpeechServiceConnection_RecoMode = 3000,

    /// <summary>
    /// The spoken language to be recognized (in BCP-47 format). Under normal circumstances, you shouldn't have to use this property
    /// directly.
    /// Instead, use <see cref="SpeechConfig::SetSpeechRecognitionLanguage"/>.
    /// </summary>
    SpeechServiceConnection_RecoLanguage = 3001,

    /// <summary>
    /// The session id. This id is a universally unique identifier (aka UUID) representing a specific binding of an audio input stream
    /// and the underlying speech recognition instance to which it is bound. Under normal circumstances, you shouldn't have to use this
    /// property directly.
    /// Instead use <see cref="SessionEventArgs::SessionId"/>.
    /// </summary>
    Speech_SessionId = 3002,

    /// <summary>
    /// The requested Cognitive Services Speech Service response output format (simple or detailed). Under normal circumstances, you shouldn't have
    /// to use this property directly.
    /// Instead use <see cref="SpeechConfig::SetOutputFormat"/>.
    /// </summary>
    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,

    /// <summary>
    /// The requested Cognitive Services Speech Service response output profanity level. Currently unused.
    /// </summary>
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,

    /// <summary>
    /// The Cognitive Services Speech Service response output (in JSON format). This property is available on recognition result objects only.
    /// </summary>
    SpeechServiceResponse_JsonResult = 5000,

    /// <summary>
    /// The Cognitive Services Speech Service error details (in JSON format). Under normal circumstances, you shouldn't have to
    /// use this property directly.
    /// Instead, use <see cref="CancellationDetails::ErrorDetails"/>.
    /// </summary>
    SpeechServiceResponse_JsonErrorDetails = 5001,

    /// <summary>
    /// The cancellation reason. Currently unused.
    /// </summary>
    CancellationDetails_Reason = 6000,

    /// <summary>
    /// The cancellation text. Currently unused.
    /// </summary>
    CancellationDetails_ReasonText = 6001,

    /// <summary>
    /// The cancellation detailed text. Currently unused.
    /// </summary>
    CancellationDetails_ReasonDetailedText = 6002,

    /// <summary>
    /// The Language Understanding Service response output (in JSON format). Available via <see cref="IntentRecognitionResult.Properties"/>.
    /// </summary>
    LanguageUnderstandingServiceResponse_JsonResult = 7000

};

enum class OutputFormat
{
    Simple = 0,
    Detailed = 1
};

/// <summary>
/// Specifies the possible reasons a recognition result might be generated.
/// </summary>
enum class ResultReason
{
    /// <summary>
    /// Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
    /// </summary>
    NoMatch = 0,

    /// <summary>
    /// Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
    /// </summary>
    Canceled = 1,

    /// <summary>
    /// Indicates the speech result contains hypothesis text.
    /// </summary>
    RecognizingSpeech = 2,

    /// <summary>
    /// Indicates the speech result contains final text that has been recognized.
    /// Speech Recognition is now complete for this phrase.
    /// </summary>
    RecognizedSpeech = 3,

    /// <summary>
    /// Indicates the intent result contains hypothesis text and intent.
    /// </summary>
    RecognizingIntent = 4,

    /// <summary>
    /// Indicates the intent result contains final text and intent.
    /// Speech Recognition and Intent determination are now complete for this phrase.
    /// </summary>
    RecognizedIntent = 5,

    /// <summary>
    /// Indicates the translation result contains hypothesis text and its translation(s).
    /// </summary>
    TranslatingSpeech = 6,

    /// <summary>
    /// Indicates the translation result contains final text and corresponding translation(s).
    /// Speech Recognition and Translation are now complete for this phrase.
    /// </summary>
    TranslatedSpeech = 7,

    /// <summary>
    /// Indicates the synthesized audio result contains a non-zero amount of audio data
    /// </summary>
    SynthesizingAudio = 8,

    /// <summary>
    /// Indicates the synthesized audio is now complete for this phrase.
    /// </summary>
    SynthesizingAudioCompleted = 9
};

/// <summary>
/// Defines the possible reasons a recognition result might be canceled.
/// </summary>
enum class CancellationReason
{
    /// <summary>
    /// Indicates that an error occurred during speech recognition.
    /// </summary>
    Error = 1,

    /// <summary>
    /// Indicates that the end of the audio stream was reached.
    /// </summary>
    EndOfStream = 2,
};

/// <summary>
/// Defines error code in case that CancellationReason is Error.
/// Added in version 1.1.0.
/// </summary>
enum class CancellationErrorCode
{
    /// <summary>
    /// No error.
    /// </summary>
    NoError = 0,

    /// <summary>
    /// Indicates an authentication error.
    /// </summary>
    AuthenticationFailure = 1,

    /// <summary>
    /// Indicates that one or more recognition parameters are invalid or the audio format is not supported.
    /// </summary>
    BadRequest = 2,

    /// <summary>
    /// Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
    /// </summary>
    TooManyRequests = 3,

    /// <summary>
    /// Indicates that the free subscription used by the request ran out of quota.
    /// </summary>
    Forbidden = 4,

    /// <summary>
    /// Indicates a connection error.
    /// </summary>
    ConnectionFailure = 5,

    /// <summary>
    /// Indicates a time-out error when waiting for response from service.
    /// </summary>
    ServiceTimeout = 6,

    /// <summary>
    /// Indicates that an error is returned by the service.
    /// </summary>
    ServiceError = 7,

    /// <summary>
    /// Indicates that the service is currently unavailable.
    /// </summary>
    ServiceUnavailable = 8,

    /// <summary>
    /// Indicates an unexpected runtime error.
    /// </summary>
    RuntimeError = 9
};

/// <summary>
/// Defines the possible reasons a recognition result might not be recognized.
/// </summary>
enum class NoMatchReason
{
    /// <summary>
    /// Indicates that speech was detected, but not recognized.
    /// </summary>
    NotRecognized = 1,

    /// <summary>
    /// Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
    /// </summary>
    InitialSilenceTimeout = 2,

    /// <summary>
    /// Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
    /// </summary>
    InitialBabbleTimeout = 3
};

} } } // Microsoft::CognitiveServices::Speech
