//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_result.cpp: Public API definitions for IntentResult related C methods
//

#include "stdafx.h"
#include "string_utils.h"
#include "handle_helpers.h"


using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI_(const_char_ptr) error_get_call_stack(SPXERRORHANDLE errorHandle)
{
    auto errorHandles = CSpxSharedPtrHandleTableManager::Get<ExceptionWithCallStack, SPXERRORHANDLE>();
    if (errorHandles->IsTracked(errorHandle))
    {
        auto error = (*errorHandles)[errorHandle];
        if (error != nullptr)
        {
            return error->GetCallStack();
        }
    }
    return nullptr;
}

SPXAPI error_get_error_code(SPXERRORHANDLE errorHandle) 
{
    auto errorHandles = CSpxSharedPtrHandleTableManager::Get<ExceptionWithCallStack, SPXERRORHANDLE>();
    if (errorHandles->IsTracked(errorHandle)) 
    {
        auto error = (*errorHandles)[errorHandle];
        if (error != nullptr)
        {
            return error->GetErrorCode();
        }
    }
    
    return SPX_NOERROR;
}

SPXAPI_(const_char_ptr) error_get_message(SPXERRORHANDLE errorHandle)
{
    auto errorHandles = CSpxSharedPtrHandleTableManager::Get<ExceptionWithCallStack, SPXERRORHANDLE>();
    if (errorHandles->IsTracked(errorHandle))
    {
        auto error = (*errorHandles)[errorHandle];
        if (error != nullptr)
        {
            return error->what();
        }
    }
    return nullptr;
}

SPXAPI error_release(SPXERRORHANDLE errorHandle)
{
    return Handle_Close<SPXERRORHANDLE, ExceptionWithCallStack>(errorHandle);
}
