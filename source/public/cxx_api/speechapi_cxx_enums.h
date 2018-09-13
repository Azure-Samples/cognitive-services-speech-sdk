//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
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
enum class SpeechPropertyId
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
    /// RPS token.
    /// </summary>
    SpeechServiceRps_Token = 1005,

    /// <summary>
    /// Endpoint ID.
    /// </summary>
    SpeechServiceConnection_EndpointId = 1006,

    /// <summary>
    /// Translation from language.
    /// </summary>
    SpeechServiceConnection_TranslationFromLanguage = 2000,

    /// <summary>
    /// Translation to languages.
    /// </summary>
    SpeechServiceConnection_TranslationToLanguages = 2001,

    /// <summary>
    /// Translation output voice.
    /// </summary>
    SpeechServiceConnection_TranslationVoice = 2002,

    /// <summary>
    /// Translation features.
    /// </summary>
    SpeechServiceConnection_TranslationFeatures = 2003,

    /// <summary>
    /// Intent region.
    /// </summary>
    SpeechServiceConnection_IntentRegion = 2004,

    /// <summary>
    /// Intent source language. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_IntentSourceLanguage = 2005,

    /// <summary>
    /// Recognition mode.
    /// </summary>
    SpeechServiceConnection_RecoMode = 3000,

    /// <summary>
    /// Interactive mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Interactive = 3001,

    /// <summary>
    /// Conversation mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Conversation = 3002,

    /// <summary>
    /// Dictation mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Dictation = 3004,

    /// <summary>
    /// Recognition language.
    /// </summary>
    SpeechServiceConnection_RecoLanguage = 3005,

    /// <summary>
    /// Session id. TODO: REMOVE
    /// </summary>
    Speech_SessionId = 3006,

    /// <summary>
    /// Detailed result required.
    /// </summary>
    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,

    /// <summary>
    /// Profanity filtering required.
    /// </summary>
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,

    /// <summary>
    /// TODO:WHAT IS THIS FOR?
    /// </summary>
    SpeechServiceResponse_Json = 5000,

    /// <summary>
    /// JSON in result.
    /// </summary>
    SpeechServiceResponse_JsonResult = 5001,

    /// <summary>
    /// Error details.
    /// </summary>
    SpeechServiceResponse_JsonErrorDetails = 5002,

    /// <summary>
    /// Cancellation reason.
    /// </summary>
    CancellationDetails_ReasonCanceled = 6000,

    /// <summary>
    /// Cancellation text.
    /// </summary>
    CancellationDetails_ReasonText = 6001,

    /// <summary>
    /// Cancellation detailed text.
    /// </summary>
    CancellationDetails_ReasonDetailedText = 6002
};

enum class OutputFormat
{
    Simple = 0,
    Detailed = 1
};

/// <summary>
/// Specifies the possible reasons a recognition result might be generated.
/// </summary>
enum class Reason
{
    /// <summary>
    /// Indicates the result is a phrase that has been successfully recognized.
    /// </summary>
    Recognized,

    /// <summary>
    /// Indicates the result is a hypothesis text that has been recognized.
    /// </summary>
    IntermediateResult,

    /// <summary>
    /// Indicates that speech was detected in the audio stream, but no words from the target language were matched.
    /// Possible reasons could be wrong setting of the target language or wrong format of audio stream.
    /// </summary>
    NoMatch,

    /// <summary>
    /// Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
    /// </summary>
    InitialSilenceTimeout,

    /// <summary>
    /// Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
    /// </summary>
    InitialBabbleTimeout,

    /// <summary>
    /// Indicates that an error occurred during recognition. The ErrorDetails in Property contains detailed error reasons.
    /// </summary>
    Canceled
};

/// <summary>
/// Specifies properties that can be retrieved from a RecognitionResult.
/// </summary>
enum class ResultProperty { Json = 1, LanguageUnderstandingJson = 2, ErrorDetails = 3 };

/// <summary>
/// Enumerates parameters that can be used to configure a recognizer.
/// </summary>
enum class RecognizerParameter { DeploymentId = 1 };

} } } // Microsoft::CognitiveServices::Speech

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Translation {

/// <summary>
/// Defines the status code of translation result.
/// </summary>
enum class TranslationStatusCode {
    /// <summary>
    /// The translation is successful.
    /// </summary>
    Success,

    /// <summary>
    /// An error occurred during translation.
    /// </summary>
    Error
};

/// <summary>
/// Defines the status code of synthesis result.
/// </summary>
enum class SynthesisStatusCode {
    /// <summary>
    /// The audio data contained in the message is valid.
    /// </summary>
    Success,

    /// <summary>
    /// Indicates the end of audio data. No audio data is included in this message.
    /// </summary>
    SynthesisEnd,

    /// <summary>
    /// An error occurred during translation.
    /// </summary>
    Error
};

} } } } // Microsoft::CognitiveServices::Speech::Translation
