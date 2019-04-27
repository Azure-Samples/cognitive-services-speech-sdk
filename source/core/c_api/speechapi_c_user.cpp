//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_user.cpp: Public API declarations for conversation transcriber user related C methods.
//

#include "stdafx.h"
#include "create_object_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include <assert.h>
#include <cstring>

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI user_create_from_id(const char* user_id, SPXUSERHANDLE* huser)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, user_id == nullptr || !(*user_id));
    SPXAPI_INIT_HR_TRY(hr)
    {
        *huser = SPXHANDLE_INVALID;

        auto user = SpxCreateObjectWithSite<ISpxUser>("CSpxUser", SpxGetRootSite());
        user->InitFromUserId(user_id);

        auto userhandles = CSpxSharedPtrHandleTableManager::Get<ISpxUser, SPXUSERHANDLE>();
        *huser = userhandles->TrackHandle(user);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI user_release_handle(SPXUSERHANDLE huser)
{
    return Handle_Close<SPXUSERHANDLE, ISpxUser>(huser);
}

SPXAPI user_get_id(SPXUSERHANDLE huser, char* user_id, size_t user_id_size)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto users = CSpxSharedPtrHandleTableManager::Get<ISpxUser, SPXUSERHANDLE>();
        auto user_ptr = (*users)[huser];

        SPX_IFTRUE_THROW_HR(user_ptr == nullptr, SPXERR_INVALID_ARG);
        auto user = SpxQueryInterface<ISpxUser>(user_ptr);

        auto id = user->GetId();
        SPX_IFTRUE_THROW_HR(id.length() >= user_id_size, SPXERR_INVALID_ARG);
        std::memcpy(user_id, id.c_str(), id.length() + 1);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
