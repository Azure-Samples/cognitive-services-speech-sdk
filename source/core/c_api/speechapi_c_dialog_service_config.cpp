//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_dialog_service_config.cpp: Public API definitions for dialog service configuration related C methods and types
//
#include "stdafx.h"

#include <speechapi_c_dialog_service_config.h>
#include <spxdebug.h>

#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "handle_table.h"
#include "site_helpers.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI dialog_service_config_from_bot_secret(SPXSPEECHCONFIGHANDLE* ph_dialog_service_config, const char* secret_key, const char *subscription, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_dialog_service_config == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, secret_key == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_dialog_service_config = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>("CSpxSpeechConfig", SpxGetRootSite());
        config->InitFromSubscription(subscription, region);

        auto config_handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXAUDIOCONFIGHANDLE>();

        auto properties = SpxQueryInterface<ISpxNamedProperties>(config);

        properties->SetStringValue(GetPropertyName(PropertyId::Conversation_Secret_Key), secret_key);

        *ph_dialog_service_config = config_handles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI dialog_service_config_from_task_dialog_app_id(SPXSPEECHCONFIGHANDLE* ph_dialog_service_config, const char* app_id, const char *subscription, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_dialog_service_config == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, app_id == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_dialog_service_config = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>("CSpxSpeechConfig", SpxGetRootSite());
        config->InitFromSubscription(subscription, region);

        auto config_handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXAUDIOCONFIGHANDLE>();

        auto properties = SpxQueryInterface<ISpxNamedProperties>(config);

        properties->SetStringValue(GetPropertyName(PropertyId::Conversation_TaskDialogAppId), app_id);

        *ph_dialog_service_config = config_handles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
