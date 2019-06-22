//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_dialog_service_config.h: Public API declarations for dialog service connector configuration related C methods and types
//
#pragma once

#include <speechapi_c_common.h>

SPXAPI dialog_service_config_from_bot_secret(SPXSPEECHCONFIGHANDLE* ph_dialog_service_config, const char* secret_key, const char *subscription, const char* region);
SPXAPI dialog_service_config_from_task_dialog_app_id(SPXSPEECHCONFIGHANDLE* ph_dialog_service_config, const char* app_id, const char *subscription, const char* region);
