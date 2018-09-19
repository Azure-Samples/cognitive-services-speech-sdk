//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_c_common.h>

typedef enum { SpeechOutputFormat_Simple = 0, SpeechOutputFormat_Detailed = 1 } SpeechOutputFormat;

SPXAPI_(bool) speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE hconfig);
SPXAPI speech_config_from_subscription(SPXSPEECHCONFIGHANDLE* hconfig, const char * subscription, const char * region);
SPXAPI speech_config_from_authorization_token(SPXSPEECHCONFIGHANDLE* hconfig, const char * authToken, const char * region);
SPXAPI speech_config_from_endpoint(SPXSPEECHCONFIGHANDLE * hconfig, const char* endpoint, const char* subscription);
SPXAPI speech_config_release(SPXSPEECHCONFIGHANDLE hconfig);
SPXAPI speech_config_get_property_bag(SPXSPEECHCONFIGHANDLE hconfig, SPXPROPERTYBAGHANDLE* hpropbag);
