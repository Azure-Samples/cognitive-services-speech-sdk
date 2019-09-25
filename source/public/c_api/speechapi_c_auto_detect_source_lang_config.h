//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI auto_detect_source_lang_config_from_languages(SPXAUTODETECTSOURCELANGCONFIGHANDLE* hconfig, const char* languages);
SPXAPI_(bool) auto_detect_source_lang_config_is_handle_valid(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig);
SPXAPI auto_detect_source_lang_config_release(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig);
SPXAPI auto_detect_source_lang_config_get_property_bag(SPXAUTODETECTSOURCELANGCONFIGHANDLE hconfig, SPXPROPERTYBAGHANDLE* hpropbag);
