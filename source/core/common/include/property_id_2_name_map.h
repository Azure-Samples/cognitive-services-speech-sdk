//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_id_2_name_map.h: internal maping function from id to its name
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

constexpr const char* GetPropertyName(const PropertyId id)
{
    switch (id)
    {
    case PropertyId::SpeechServiceConnection_Key: return "SPEECH-SubscriptionKey";
    case PropertyId::SpeechServiceConnection_Endpoint: return "SPEECH-Endpoint";
    case PropertyId::SpeechServiceConnection_Host: return "SPEECH-Host";
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
    case PropertyId::SpeechServiceConnection_AutoDetectSourceLanguages: return "Auto-Detect-Source-Languages";
    case PropertyId::SpeechServiceConnection_AutoDetectSourceLanguageResult: return "Auto-Detect-Source-Language-Result";
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
    case PropertyId::Conversation_ApplicationId: return "DIALOG-ApplicationId";
    case PropertyId::Conversation_DialogType: return "DIALOG-DialogType";
    case PropertyId::Conversation_Initial_Silence_Timeout: return "DIALOG-InitialSilenceTimeout";
    case PropertyId::Conversation_From_Id: return "DIALOG-FromId";
    case PropertyId::Conversation_Conversation_Id: return "DIALOG-ConversationId";
    case PropertyId::Conversation_Custom_Voice_Deployment_Ids: return "DIALOG-CustomVoiceDeploymentIds";
    case PropertyId::Conversation_Speech_Activity_Template: return "DIALOG-SpeechActivityTemplate";
    case PropertyId::DataBuffer_TimeStamp: return "DataBuffer_TimeStamp";
    case PropertyId::DataBuffer_UserId: return "DataBuffer_UserId";

    default: return nullptr;
    }

}

constexpr auto g_recoModeInteractive = "INTERACTIVE";
constexpr auto g_recoModeDictation = "DICTATION";
constexpr auto g_recoModeConversation = "CONVERSATION";

constexpr auto g_audioSourceMicrophone = "Microphones";
constexpr auto g_audioSourceStream = "Stream";
constexpr auto g_audioSourceFile = "File";

constexpr auto KeywordConfig_EnableKeywordVerification = "KeywordConfig_EnableKeywordVerification";
constexpr auto g_keyword_KeywordOnly = "IsKeywordRecognizer";

constexpr auto g_dialogType_BotFramework = "bot_framework";
constexpr auto g_dialogType_CustomCommands = "custom_commands";

}}}}
