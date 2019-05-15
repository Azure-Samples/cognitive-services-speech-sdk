//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_language_understanding_model.cpp: Public API definitions for LanguageUnderstandingModel related C methods
//

#include "stdafx.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) language_understanding_model_handle_is_valid(SPXLUMODELHANDLE hlumodel)
{
    return Handle_IsValid<SPXLUMODELHANDLE, ISpxLanguageUnderstandingModel>(hlumodel);
}

SPXAPI language_understanding_model_create_from_uri(SPXLUMODELHANDLE* hlumodel, const char* uri)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hlumodel == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, uri == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hlumodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLanguageUnderstandingModel>("CSpxLanguageUnderstandingModel", SpxGetRootSite());
        model->InitEndpoint(PAL::ToWString(uri).c_str());
        
        auto languageUnderstandingModelHandles = CSpxSharedPtrHandleTableManager::Get<ISpxLanguageUnderstandingModel, SPXLUMODELHANDLE>();
        *hlumodel = languageUnderstandingModelHandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI language_understanding_model_create_from_app_id(SPXLUMODELHANDLE* hlumodel, const char* appId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hlumodel == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, appId == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hlumodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLanguageUnderstandingModel>("CSpxLanguageUnderstandingModel", SpxGetRootSite());
        model->InitAppId(PAL::ToWString(appId).c_str());

        auto languageUnderstandingModelHandles = CSpxSharedPtrHandleTableManager::Get<ISpxLanguageUnderstandingModel, SPXLUMODELHANDLE>();
        *hlumodel = languageUnderstandingModelHandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI language_understanding_model_create_from_subscription(SPXLUMODELHANDLE* hlumodel, const char* subscriptionKey, const char* appId, const char* region)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hlumodel == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, subscriptionKey == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, appId == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, region == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hlumodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLanguageUnderstandingModel>("CSpxLanguageUnderstandingModel", SpxGetRootSite());
        model->InitSubscription(PAL::ToWString(subscriptionKey).c_str(), PAL::ToWString(appId).c_str(), PAL::ToWString(region).c_str());

        auto languageUnderstandingModelHandles = CSpxSharedPtrHandleTableManager::Get<ISpxLanguageUnderstandingModel, SPXLUMODELHANDLE>();
        *hlumodel = languageUnderstandingModelHandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI language_understanding_model__handle_release(SPXLUMODELHANDLE hlumodel)
{
    return Handle_Close<SPXLUMODELHANDLE, ISpxLanguageUnderstandingModel>(hlumodel);
}
