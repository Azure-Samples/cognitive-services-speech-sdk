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


SPXAPI_(bool) KeywordRecognitionModel_Handle_IsValid(SPXKEYWORDHANDLE hkeyword)
{
    return Handle_IsValid<SPXKEYWORDHANDLE, ISpxKwsModel>(hkeyword);
}

SPXAPI KeywordRecognitionModel_Handle_Close(SPXKEYWORDHANDLE hkeyword)
{
    return Handle_Close<SPXKEYWORDHANDLE, ISpxKwsModel>(hkeyword);
}

SPXAPI KeywordRecognitionModel_Create_From_File(const wchar_t* fileName, SPXKEYWORDHANDLE* phkwmodel)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phkwmodel = SPXHANDLE_INVALID;

        auto model = SpxCreateObjectWithSite<ISpxKwsModel>("CSpxKwsModel", SpxGetRootSite());
        model->InitFromFile(fileName);
        
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxKwsModel, SPXKEYWORDHANDLE>();
        *phkwmodel = handles->TrackHandle(model);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
