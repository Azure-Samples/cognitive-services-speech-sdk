//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_session.cpp: Public API definitions for Session related C methods
//

#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "string_utils.h"
#include "service_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI session_from_recognizer(SPXRECOHANDLE hreco, SPXSESSIONHANDLE* phsession)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phsession == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phsession = SPXHANDLE_INVALID;

        auto recoHandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recoHandles)[hreco];

        auto precognizer = SpxQueryInterface<ISpxSessionFromRecognizer>(recognizer);
        auto psession = precognizer->GetDefaultSession();

        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        *phsession = sessionHandles->TrackHandle(psession);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) session_handle_is_valid(SPXSESSIONHANDLE hsession)
{
    return Handle_IsValid<SPXSESSIONHANDLE, ISpxSession>(hsession);
}

SPXAPI session_handle_release(SPXSESSIONHANDLE hsession)
{
    return Handle_Close<SPXSESSIONHANDLE, ISpxSession>(hsession);
}

SPXAPI session_get_property_bag(SPXSESSIONHANDLE hsession, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXRECOHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryService<ISpxNamedProperties>(session);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);

    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
