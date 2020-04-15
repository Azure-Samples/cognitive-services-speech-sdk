//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include "stdafx.h"
#include "handle_helpers.h"
#include "speechapi_cxx_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

inline SPXHR async_operation_wait_for(SPXASYNCHANDLE async_handle, uint32_t milliseconds)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto async_handles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        auto async_operation = (*async_handles)[async_handle];
        auto completed = async_operation->WaitFor(milliseconds);
        if (!completed)
        {
            return SPXERR_TIMEOUT;
        }
        async_operation->Future.get();
        return SPX_NOERROR;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename Result>
SPXHR async_operation_wait_for(SPXASYNCHANDLE async_handle, uint32_t milliseconds, SPXHANDLE* result_handle)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, result_handle == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *result_handle = SPXHANDLE_INVALID;

        auto async_handles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<Result>>, SPXASYNCHANDLE>();
        auto async_operation = (*async_handles)[async_handle];
        auto completed = async_operation->WaitFor(milliseconds);
        if (!completed)
        {
            return SPXERR_TIMEOUT;
        }
        auto result = async_operation->Future.get();
        if (result == nullptr)
        {
            return SPXERR_TIMEOUT;
        }
        auto result_handles = CSpxSharedPtrHandleTableManager::Get<Result, SPXRESULTHANDLE>();
        *result_handle = result_handles->TrackHandle(result);
        return SPX_NOERROR;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename OperationFn, typename WaitFn, typename... Args>
inline SPXHR async_to_sync(SPXHANDLE handle, OperationFn operationFn, WaitFn waitFn, Args&&... args)
{
    SPX_INIT_HR(hr);
    SPXHANDLE async_handle = SPXHANDLE_INVALID;
    Utils::ScopeGuard guard{ [&]()
    {
        SPX_REPORT_ON_FAIL(recognizer_async_handle_release(async_handle));
    } };
    hr = operationFn(handle, std::forward<Args&&>(args)..., &async_handle);
    SPX_RETURN_ON_FAIL(hr);
    hr = waitFn(async_handle, UINT32_MAX);
    SPX_RETURN_ON_FAIL(hr);
    SPX_RETURN_HR(hr);
}

template<typename OperationFn, typename WaitFn, typename... Args>
inline SPXHR async_to_sync_with_result(SPXHANDLE handle, SPXHANDLE* resultHandle, OperationFn operationFn, WaitFn waitFn, Args&&... args)
{
    SPX_INIT_HR(hr);
    SPXHANDLE async_handle = SPXHANDLE_INVALID;
    Utils::ScopeGuard guard{ [&]()
    {
        SPX_REPORT_ON_FAIL(recognizer_async_handle_release(async_handle));
    } };
    hr = operationFn(handle, std::forward<Args&&>(args)..., &async_handle);
    SPX_RETURN_ON_FAIL(hr);
    hr = waitFn(async_handle, UINT32_MAX, resultHandle);
    SPX_RETURN_ON_FAIL(hr);
    SPX_RETURN_HR(hr);
}

} } } }

