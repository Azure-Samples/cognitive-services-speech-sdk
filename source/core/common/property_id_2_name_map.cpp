//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_id_2_name_map.cpp: implemenation of mapping id to its name.
//

#include <unordered_map>

#include "stdafx.h"
#include "speechapi_cxx_properties.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

const char* GetPropertyName(const PropertyId& id)
{
    switch (id)
    {
    case PropertyId::SpeechServiceConnection_Key: return "SPEECH-SubscriptionKey";
    case PropertyId::SpeechServiceConnection_Endpoint: return "SPEECH-Endpoint";
    case PropertyId::SpeechServiceConnection_Region: return "SPEECH-Region";
    case PropertyId::SpeechServiceAuthorization_Token: return "SPEECH-AuthToken";
    case PropertyId::SpeechServiceAuthorization_Type: return "SpeechServiceAuthorization_Type";
    case PropertyId::SpeechServiceConnection_EndpointId: return "SPEECH-ModelId";
    case PropertyId::SpeechServiceConnection_ProxyHostName: return "SPEECH-ProxyHostName";
    case PropertyId::SpeechServiceConnection_ProxyPort: return "SPEECH-ProxyPort";
    case PropertyId::SpeechServiceConnection_ProxyUserName: return "SPEECH-ProxyUserName";
    case PropertyId::SpeechServiceConnection_ProxyPassword: return "SPEECH-ProxyPassword";
    case PropertyId::SpeechServiceConnection_Url: return "SPEECH-ConnectionUrl";
    case PropertyId::SpeechServiceConnection_TranslationToLanguages: return "TRANSLATION-ToLanguages";
    case PropertyId::SpeechServiceConnection_TranslationVoice: return "TRANSLATION-Voice";
    case PropertyId::SpeechServiceConnection_TranslationFeatures: return "TRANSLATION-Features";
    case PropertyId::SpeechServiceConnection_IntentRegion: return "INTENT-region";
    case PropertyId::SpeechServiceConnection_RecoMode: return "SPEECH-RecoMode";
    case PropertyId::SpeechServiceConnection_RecoLanguage: return "SPEECH-RecoLanguage";
    case PropertyId::SpeechServiceConnection_SynthLanguage: return "SPEECH-SynthLanguage";
    case PropertyId::SpeechServiceConnection_SynthVoice: return "SPEECH-SynthVoice";
    case PropertyId::SpeechServiceConnection_SynthOutputFormat: return "SPEECH-SynthOutputFormat";
    case PropertyId::SpeechServiceConnection_UserDefinedQueryParameters: return "SPEECH-UserDefinedQueryParameters";
    case PropertyId::Speech_SessionId: return "SessionId";
    case PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs: return "SPEECH-InitialSilenceTimeoutMs";
    case PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs: return "SPEECH-EndSilenceTimeoutMs";
    case PropertyId::SpeechServiceConnection_EnableAudioLogging: return "SPEECH-EnableAudioLogging";
    case PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse: return "SpeechServiceResponse_RequestDetailedResultTrueFalse";
    case PropertyId::SpeechServiceResponse_RequestProfanityFilterTrueFalse: return "SpeechServiceResponse_RequestProfanityFilterTrueFalse";
    case PropertyId::SpeechServiceResponse_ProfanityOption: return "SpeechServiceResponse_ProfanityOption";
    case PropertyId::SpeechServiceResponse_PostProcessingOption: return "SpeechServiceResponse_PostProcessingOption";
    case PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps: return "SpeechServiceResponse_RequestWordLevelTimestamps";
    case PropertyId::SpeechServiceResponse_StablePartialResultThreshold: return "SpeechServiceResponse_StablePartialResultThreshold";
    case PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult: return "SpeechServiceResponse_TranslationRequestStablePartialResult";
    case PropertyId::SpeechServiceResponse_OutputFormatOption: return "SpeechServiceResponse_OutputFormatOption";
    case PropertyId::SpeechServiceResponse_JsonResult: return "RESULT-Json";
    case PropertyId::LanguageUnderstandingServiceResponse_JsonResult: return "RESULT-LanguageUnderstandingJson";
    case PropertyId::SpeechServiceResponse_JsonErrorDetails: return "RESULT-ErrorDetails";
    case PropertyId::SpeechServiceResponse_RecognitionLatencyMs: return "RESULT-RecognitionLatencyMs";
    case PropertyId::CancellationDetails_Reason: return "CancellationDetails_Reason";
    case PropertyId::CancellationDetails_ReasonText: return "CancellationDetails_ReasonText";
    case PropertyId::CancellationDetails_ReasonDetailedText: return "CancellationDetails_ReasonDetailedText";
    case PropertyId::AudioConfig_DeviceNameForCapture: return "AudioConfig_DeviceNameForCapture";
    case PropertyId::AudioConfig_NumberOfChannelsForCapture: return "AudioConfig_NumberOfChannelsForCapture";
    case PropertyId::AudioConfig_SampleRateForCapture: return "AudioConfig_SampleRateForCapture";
    case PropertyId::AudioConfig_BitsPerSampleForCapture: return "AudioConfig_BitsPerSampleForCapture";
    case PropertyId::AudioConfig_AudioSource: return "AudioConfig_AudioSource";
    case PropertyId::Speech_LogFilename: return "SPEECH-LogFilename";
    case PropertyId::Conversation_Secret_Key: return "DIALOG-SecretKey";
    case PropertyId::Conversation_TaskDialogAppId: return "DIALOG-TaskDialogAppId";
    case PropertyId::Conversation_Initial_Silence_Timeout: return "DIALOG-InitialSilenceTimeout";
    case PropertyId::Conversation_From_Id: return "DIALOG-FromId";
    case PropertyId::DataBuffer_TimeStamp: return "DataBuffer_TimeStamp";
    case PropertyId::DataBuffer_UserId: return "DataBuffer_UserId";

    default:
        LogError("undefined PropertyId of %d", static_cast<int>(id));
        SPX_THROW_ON_FAIL(SPXERR_INVALID_ARG);
        return "";
    }

}
}}}} // Microsoft::CognitiveServices::Speech::Impl
