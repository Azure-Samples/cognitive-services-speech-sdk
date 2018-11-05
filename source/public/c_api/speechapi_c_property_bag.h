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

// NOTE: Currenlty this enum is duplicated with C++ side,
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

    SpeechServiceConnection_ProxyHostName = 1100,
    SpeechServiceConnection_ProxyPort = 1101,
    SpeechServiceConnection_ProxyUserName = 1102,
    SpeechServiceConnection_ProxyPassword = 1103,

    SpeechServiceConnection_TranslationToLanguages = 2000,
    SpeechServiceConnection_TranslationVoice = 2001,
    SpeechServiceConnection_TranslationFeatures = 2002,
    SpeechServiceConnection_IntentRegion = 2003,

    SpeechServiceConnection_RecoMode = 3000,
    SpeechServiceConnection_RecoLanguage = 3001,
    Speech_SessionId = 3002,

    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,

    SpeechServiceResponse_JsonResult = 5000,
    SpeechServiceResponse_JsonErrorDetails = 5001,

    CancellationDetails_Reason = 6000,
    CancellationDetails_ReasonText = 6001,
    CancellationDetails_ReasonDetailedText = 6002,

    LanguageUnderstandingServiceResponse_JsonResult = 7000
};
#endif

