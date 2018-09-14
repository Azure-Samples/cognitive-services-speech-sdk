//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
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
    SpeechServiceRps_Token = 1005,
    SpeechServiceConnection_EndpointId = 1006,

    SpeechServiceConnection_TranslationFromLanguage = 2000,
    SpeechServiceConnection_TranslationToLanguages = 2001,
    SpeechServiceConnection_TranslationVoice = 2002,
    SpeechServiceConnection_TranslationFeatures = 2003,
    SpeechServiceConnection_IntentRegion = 2004,
    SpeechServiceConnection_IntentSourceLanguage = 2005,

    SpeechServiceConnection_RecoMode = 3000,
    SpeechServiceConnection_RecoMode_Interactive = 3001,
    SpeechServiceConnection_RecoMode_Conversation = 3002,
    SpeechServiceConnection_RecoMode_Dictation = 3004,
    SpeechServiceConnection_RecoLanguage = 3005,
    Speech_SessionId = 3006,

    SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,

    SpeechServiceResponse_Json = 5000,
    SpeechServiceResponse_JsonResult = 5001,
    SpeechServiceResponse_JsonErrorDetails = 5002,

    CancellationDetails_Reason = 6000,
    CancellationDetails_ReasonText = 6001,
    CancellationDetails_ReasonDetailedText = 6002
};
#endif

