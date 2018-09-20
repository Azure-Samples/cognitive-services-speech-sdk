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
/// </summary>
enum class PropertyId
{
    /// <summary>
    /// Subscription key.
    /// </summary>
    SpeechServiceConnection_Key = 1000,

    /// <summary>
    /// Endpoint.
    /// </summary>
    SpeechServiceConnection_Endpoint = 1001,

    /// <summary>
    /// Region.
    /// </summary>
    SpeechServiceConnection_Region = 1002,

    /// <summary>
    /// Authorization token.
    /// </summary>
    SpeechServiceAuthorization_Token = 1003,

    /// <summary>
    /// Authorization type.
    /// </summary>
    SpeechServiceAuthorization_Type = 1004,

    /// <summary>
    /// Endpoint ID.
    /// </summary>
    SpeechServiceConnection_EndpointId = 1005,

    /// <summary>
    /// Translation to languages.
    /// </summary>
    SpeechServiceConnection_TranslationToLanguages = 2000,

    /// <summary>
    /// Translation output voice.
    /// </summary>
    SpeechServiceConnection_TranslationVoice = 2001,

    /// <summary>
    /// Translation features.
    /// </summary>
    SpeechServiceConnection_TranslationFeatures = 2002,

    /// <summary>
    /// Intent region.
    /// </summary>
    SpeechServiceConnection_IntentRegion = 2003,

    /// <summary>
    /// Recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
    /// </summary>
    SpeechServiceConnection_RecoMode = 3000,

    /// <summary>
    /// Recognition language.
    /// </summary>
    SpeechServiceConnection_RecoLanguage = 3001,

    /// <summary>
    /// Session ID.
    /// </summary>
    Speech_SessionId = 3002,

    /// <summary>
    /// Detailed result required.
    /// </summary>
    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,

    /// <summary>
    /// Profanity filtering required.
    /// </summary>
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,

    /// <summary>
    /// JSON result of speech recognition service.
    /// </summary>
    SpeechServiceResponse_JsonResult = 5000,

    /// <summary>
    /// Error details.
    /// </summary>
    SpeechServiceResponse_JsonErrorDetails = 5001,

    /// <summary>
    /// Cancellation reason.
    /// </summary>
    CancellationDetails_Reason = 6000,

    /// <summary>
    /// Cancellation text.
    /// </summary>
    CancellationDetails_ReasonText = 6001,

    /// <summary>
    /// Cancellation detailed text.
    /// </summary>
    CancellationDetails_ReasonDetailedText = 6002,

    /// <summary>
    /// JSON result of language understanding service.
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
    /// Indicates that an error occurred during speech recognition. The ErrorDetails property contains detailed error response.
    /// </summary>
    Error = 1,

    /// <summary>
    /// Indicates that the end of the audio stream was reached.
    /// </summary>
    EndOfStream = 2,
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
