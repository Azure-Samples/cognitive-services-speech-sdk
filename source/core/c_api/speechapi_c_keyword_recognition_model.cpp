//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_keyword_recognition_model.cpp: Public API definitions for KeywordRecognitionModel related C methods
//

#include "stdafx.h"
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "resource_manager.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;


SPXAPI_(bool) keyword_recognition_model_handle_is_valid(SPXKEYWORDHANDLE hkeyword)
{
    return Handle_IsValid<SPXKEYWORDHANDLE, ISpxKwsModel>(hkeyword);
}

SPXAPI keyword_recognition_model_handle_release(SPXKEYWORDHANDLE hkeyword)
{
    return Handle_Close<SPXKEYWORDHANDLE, ISpxKwsModel>(hkeyword);
}

SPXAPI keyword_recognition_model_create_from_file(const char* fileName, SPXKEYWORDHANDLE* phkwmodel)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phkwmodel == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, fileName == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phkwmodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxKwsModel>("CSpxKwsModel", SpxGetRootSite());
        model->InitFromFile(PAL::ToWString(fileName).c_str());
        
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxKwsModel, SPXKEYWORDHANDLE>();
        *phkwmodel = handles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
