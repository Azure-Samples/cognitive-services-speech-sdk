//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include "stdafx.h"
#include <handle_table.h>
#include <service_helpers.h>
#include <interface_helpers.h>
#include "speechapi_cxx_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

namespace Details {
    template<typename T, typename U, typename F>
    SPXHR GetTargetObject(SPXHANDLE handle, SPXHANDLE* targetHandle, F fn)
    {
        SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, targetHandle == nullptr);
        SPXAPI_INIT_HR_TRY(hr)
        {
            auto obj = CSpxSharedPtrHandleTableManager::GetPtr<T, SPXHANDLE>(handle);
            auto targetObject = fn(obj);
            *targetHandle = CSpxSharedPtrHandleTableManager::TrackHandle<U, SPXHANDLE>(targetObject);
        }
        SPXAPI_CATCH_AND_RETURN_HR(hr);
    }
}

template<typename T, typename U>
SPXHR GetTargetObjectByInterface(SPXHANDLE handle, SPXHANDLE* targetHandle)
{
    using TPtr = std::shared_ptr<T>;
    return Details::GetTargetObject<T, U>(handle, targetHandle, [](TPtr& ptr)
    {
        return SpxQueryInterface<U>(ptr);
    });
}

template<typename T, typename U>
SPXHR GetTargetObjectByService(SPXHANDLE handle, SPXHANDLE* targetHandle)
{
    using TPtr = std::shared_ptr<T>;
    return Details::GetTargetObject<T, U>(handle, targetHandle, [](TPtr& ptr)
    {
        return SpxQueryService<U>(ptr);

    });
}

} } } }