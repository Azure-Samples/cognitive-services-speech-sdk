//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_result.cpp: Public API definitions for IntentResult related C methods
//

#include "stdafx.h"
#include "string_utils.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI_(const_char_ptr) Error_GetCallStack(SPXERRORHANDLE errorHandle)
{
    auto errorHandles = CSpxSharedPtrHandleTableManager::Get<ExceptionWithCallStack, SPXERRORHANDLE>();
    auto error = (*errorHandles)[errorHandle];
    if (error != nullptr)
    {
        return error->GetCallStack();
    }
    return nullptr;
}

SPXAPI Error_GetCode(SPXERRORHANDLE errorHandle) 
{
    auto errorHandles = CSpxSharedPtrHandleTableManager::Get<ExceptionWithCallStack, SPXERRORHANDLE>();
    auto error = (*errorHandles)[errorHandle];
    if (error != nullptr)
    {
        return error->GetErrorCode();
    }
    return SPX_NOERROR;
}
