//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_property_bag.h: Public API declarations for Property Bag related C methods
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI_(bool) property_bag_is_valid(SPXPROPERTYBAGHANDLE hpropbag);
SPXAPI property_bag_set_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* value);
SPXAPI__(const char*) property_bag_get_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* defaultValue);
SPXAPI property_bag_free_string(const char* value);
SPXAPI property_bag_release(SPXPROPERTYBAGHANDLE hpropbag);
SPXAPI property_bag_copy(SPXPROPERTYBAGHANDLE hfrom, SPXPROPERTYBAGHANDLE hto);

// NOTE: Currently this enum is duplicated with C++ side,
// because SWIG cannot properly resolve conditional compilation.
#ifndef __cplusplus
enum  PropertyId
{
    SpeechServiceConnection_Key = 1000,
    SpeechServiceConnection_Endpoint = 1001,
    SpeechServiceConnection_Region = 1002,
    SpeechServiceAuthorization_Token = 1003,
    SpeechServiceAuthorization_Type = 1004,
    SpeechServiceConnection_EndpointId = 1005,
    SpeechServiceConnection_Host = 1006,

    SpeechServiceConnection_ProxyHostName = 1100,
    SpeechServiceConnection_ProxyPort = 1101,
    SpeechServiceConnection_ProxyUserName = 1102,
    SpeechServiceConnection_ProxyPassword = 1103,
    SpeechServiceConnection_Url = 1104,

    SpeechServiceConnection_TranslationToLanguages = 2000,
    SpeechServiceConnection_TranslationVoice = 2001,
    SpeechServiceConnection_TranslationFeatures = 2002,
    SpeechServiceConnection_IntentRegion = 2003,

    SpeechServiceConnection_RecoMode = 3000,
    SpeechServiceConnection_RecoLanguage = 3001,
    Speech_SessionId = 3002,
    SpeechServiceConnection_UserDefinedQueryParameters = 3003,

    SpeechServiceConnection_SynthLanguage = 3100,
    SpeechServiceConnection_SynthVoice = 3101,
    SpeechServiceConnection_SynthOutputFormat = 3102,

    SpeechServiceConnection_InitialSilenceTimeoutMs = 3200,
    SpeechServiceConnection_EndSilenceTimeoutMs = 3201,
    SpeechServiceConnection_EnableAudioLogging = 3202,

    SpeechServiceConnection_AutoDetectSourceLanguages = 3300,
    SpeechServiceConnection_AutoDetectSourceLanguageResult = 3301,

    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,
    SpeechServiceResponse_ProfanityOption = 4002,
    SpeechServiceResponse_PostProcessingOption = 4003,
    SpeechServiceResponse_RequestWordLevelTimestamps = 4004,
    SpeechServiceResponse_StablePartialResultThreshold = 4005,
    SpeechServiceResponse_OutputFormatOption = 4006,

    SpeechServiceResponse_TranslationRequestStablePartialResult = 4100,

    SpeechServiceResponse_JsonResult = 5000,
    SpeechServiceResponse_JsonErrorDetails = 5001,
    SpeechServiceResponse_RecognitionLatencyMs = 5002,

    CancellationDetails_Reason = 6000,
    CancellationDetails_ReasonText = 6001,
    CancellationDetails_ReasonDetailedText = 6002,

    LanguageUnderstandingServiceResponse_JsonResult = 7000,

    AudioConfig_DeviceNameForCapture = 8000,
    AudioConfig_NumberOfChannelsForCapture = 8001,
    AudioConfig_SampleRateForCapture = 8002,
    AudioConfig_BitsPerSampleForCapture = 8003,
    AudioConfig_AudioSource = 8004,
    Speech_LogFilename = 9001,
    Conversation_ApplicationId = 10000,
    Conversation_DialogType = 10001,
    Conversation_Initial_Silence_Timeout = 10002,
    Conversation_From_Id = 10003,
    Conversation_Conversation_Id = 10004,
    Conversation_Custom_Voice_Deployment_Ids = 10005,
    Conversation_Speech_Activity_Template = 10006,
    Conversation_ParticipantId = 10007,
    DataBuffer_TimeStamp = 11001,
    DataBuffer_UserId = 11002
};

typedef enum _ParticipantChangedReason
{
    JoinedConversation,
    LeftConversation,
    Updated
} ParticipantChangedReason;
#endif

