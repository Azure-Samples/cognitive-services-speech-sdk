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


enum class SpeechPropertyId
{
    SpeechServiceConnection_Key = 1000,
    SpeechServiceConnection_Endpoint = 1001,
    SpeechServiceConnection_Region = 1002,
    SpeechServiceAuthorization_Token = 1003,
    SpeechServiceAuthorization_Type = 1004,
    SpeechServiceRps_Token = 1005,
    SpeechServiceConnection_DeploymentId = 1006,
    
    SpeechServiceConnection_TranslationFromLanguage = 2000,
    SpeechServiceConnection_TranslationToLanguages = 2001,
    SpeechServiceConnection_TranslationVoice = 2002,
    SpeechServiceConnection_TranslationFeatures = 2003,
    SpeechServiceConnection_IntentRegion = 2004,

    SpeechServiceConnection_RecoMode = 3000,
    SpeechServiceConnection_RecoMode_Interactive = 3001,
    SpeechServiceConnection_RecoMode_Conversation = 3002,
    SpeechServiceConnection_RecoMode_Dictation = 3004,
    SpeechServiceConnection_RecoLanguage = 3005,
    Speech_SessionId = 3006,

    SpeechServiceResponse_OutputFormat = 4000,
    SpeechServiceResponse_OutputFormat_Simple = 4001,
    SpeechServiceResponse_OutputFormat_Detailed = 4002,
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4003,

    SpeechServiceResponse_Json = 5000,
    SpeechServiceResponse_JsonResult = 5001,
    SpeechServiceResponse_JsonErrorDetails = 5002,
    
    CancellationDetails_ReasonCanceled = 6000,
    CancellationDetails_ReasonText = 6001,
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


/// <summary>
/// Enumerates parameters that can be used to configure a recognizer factory.
/// </summary>
enum class FactoryParameter { Region = 1, SubscriptionKey = 2, AuthorizationToken = 3, Endpoint = 4};



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
