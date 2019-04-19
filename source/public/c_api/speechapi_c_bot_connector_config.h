//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_bot_connector_config.h: Public API declarations for bot connector configuration related C methods and types
//
#pragma once

#include <speechapi_c_common.h>

SPXAPI bot_connector_config_from_secret_key(SPXSPEECHCONFIGHANDLE* ph_bot_config, const char* secret_key, const char *subscription, const char* region);
