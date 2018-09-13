//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_property_bag.cpp: Public API definitions for Property Bag related C methods
//

#include "stdafx.h"
#include "handle_helpers.h"
#include "service_helpers.h"
#include "site_helpers.h"
#include "mock_controller.h"
#include "property_id_2_name_map.h"
#include "ispxinterfaces.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

std::shared_ptr<ISpxNamedProperties> property_bag_from_handle(SPXPROPERTYBAGHANDLE hpropbag)
{
    std::shared_ptr<ISpxNamedProperties> namedProperties;
    if (SPXFACTORYHANDLE_ROOTSITEPARAMETERS_MOCK == hpropbag)
    {
        namedProperties = SpxQueryService<ISpxNamedProperties>(SpxGetRootSite());
    }
    else
    {
        auto propbaghandles = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        auto propbag = (*propbaghandles)[hpropbag];

        namedProperties = SpxQueryInterface<ISpxNamedProperties>(propbag);
    }
    return namedProperties;
}


SPXAPI_(bool) property_bag_is_valid(SPXPROPERTYBAGHANDLE hpropbag)
{
    return Handle_IsValid<SPXPROPERTYBAGHANDLE, ISpxNamedProperties>(hpropbag);
}

SPXAPI property_bag_close(SPXPROPERTYBAGHANDLE hpropbag)
{
    return Handle_Close<SPXPROPERTYBAGHANDLE, ISpxNamedProperties>(hpropbag);
}
/*
  if name != nullptr, use name + ignore id; if name== nullptr, use id.
*/
SPXAPI__(const char*) property_bag_get_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* defaultValue)
{
    char* result = nullptr;

    if (hpropbag == nullptr)
    {
        return result;
    }

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = property_bag_from_handle(hpropbag);

        const char* name_in_use = name ? name : GetPropertyName(static_cast<SpeechPropertyId>(id));
        auto tempValue = namedProperties->GetStringValue(name_in_use, defaultValue);
        auto size = tempValue.size() + 1;

        result = new char[size];
        PAL::strcpy(result, size, tempValue.c_str(), size, true);
    }
    SPXAPI_CATCH_AND_RETURN(hr, result);
}

SPXAPI property_bag_free_string(const char* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        delete[] value;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI property_bag_set_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* defaultValue)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, defaultValue == nullptr);

    const char* name_in_use = name ? name : GetPropertyName(static_cast<SpeechPropertyId>(id));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = property_bag_from_handle(hpropbag);
        namedProperties->SetStringValue(name_in_use, defaultValue);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

