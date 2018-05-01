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


SPXAPI_(bool) LanguageUnderstandingModel_Handle_IsValid(SPXLUMODELHANDLE hlumodel)
{
    return Handle_IsValid<SPXLUMODELHANDLE, ISpxLanguageUnderstandingModel>(hlumodel);
}

SPXAPI LanguageUnderstandingModel_Handle_Close(SPXLUMODELHANDLE hlumodel)
{
    return Handle_Close<SPXLUMODELHANDLE, ISpxLanguageUnderstandingModel>(hlumodel);
}

SPXAPI LanguageUnderstandingModel_Create_From_Uri(const wchar_t* uri, SPXLUMODELHANDLE* phlumodel)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phlumodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLanguageUnderstandingModel>("CSpxLanguageUnderstandingModel", SpxGetRootSite());
        model->InitEndpoint(uri);
        
        auto languageUnderstandingModelHandles = CSpxSharedPtrHandleTableManager::Get<ISpxLanguageUnderstandingModel, SPXLUMODELHANDLE>();
        *phlumodel = languageUnderstandingModelHandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI LanguageUnderstandingModel_Create_From_Subscription(const wchar_t* subscriptionKey, const wchar_t* appId, const wchar_t* region, SPXLUMODELHANDLE* phlumodel)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phlumodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxLanguageUnderstandingModel>("CSpxLanguageUnderstandingModel", SpxGetRootSite());
        model->InitSubscription(subscriptionKey, appId, region);
        
        auto languageUnderstandingModelHandles = CSpxSharedPtrHandleTableManager::Get<ISpxLanguageUnderstandingModel, SPXLUMODELHANDLE>();
        *phlumodel = languageUnderstandingModelHandles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
