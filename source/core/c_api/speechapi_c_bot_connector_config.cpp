//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_bot_connector_config.cpp: Public API definitions for bot connector configuration related C methods and types
//
#include "stdafx.h"

#include <speechapi_c_bot_connector_config.h>
#include <spxdebug.h>

#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "handle_table.h"
#include "site_helpers.h"
#include "property_id_2_name_map.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI bot_connector_config_from_secret_key(SPXSPEECHCONFIGHANDLE* ph_bot_config, const char* secret_key, const char *subscription, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_bot_config == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, secret_key == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscription == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_bot_config = SPXHANDLE_INVALID;

        auto config = SpxCreateObjectWithSite<ISpxSpeechConfig>("CSpxSpeechConfig", SpxGetRootSite());
        config->InitFromSubscription(subscription, region);

        auto config_handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXAUDIOCONFIGHANDLE>();

        auto properties = SpxQueryInterface<ISpxNamedProperties>(config);

        properties->SetStringValue(GetPropertyName(PropertyId::Conversation_Secret_Key), secret_key);

        *ph_bot_config = config_handles->TrackHandle(config);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
