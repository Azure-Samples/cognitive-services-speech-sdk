//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_luis_model.cpp: Public API definitions for LuisModel related C methods
//

#include "stdafx.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI_(bool) LuisModel_Handle_IsValid(SPXLUISHANDLE hluis)
{
    return Handle_IsValid<SPXLUISHANDLE, ISpxLuisModel>(hluis);
}

SPXAPI LuisModel_Handle_Close(SPXLUISHANDLE hluis)
{
    return Handle_Close<SPXLUISHANDLE, ISpxLuisModel>(hluis);
}

SPXAPI LuisModel_Create_From_Uri(const wchar_t* uri, SPXLUISHANDLE* phluis)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phluis = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLuisModel>("CSpxLuisModel", SpxGetRootSite());
        model->InitEndpoint(uri);
        
        auto luishandles = CSpxSharedPtrHandleTableManager::Get<ISpxLuisModel, SPXLUISHANDLE>();
        *phluis = luishandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI LuisModel_Create_From_Subscription(const wchar_t* hostName, const wchar_t* subscriptionKey, const wchar_t* appId, SPXLUISHANDLE* phluis)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phluis = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLuisModel>("CSpxLuisModel", SpxGetRootSite());
        model->InitSubscriptionInfo(hostName, subscriptionKey, appId);
        
        auto luishandles = CSpxSharedPtrHandleTableManager::Get<ISpxLuisModel, SPXLUISHANDLE>();
        *phluis = luishandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
