//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_conversation.cpp: Public API definitions for Conversation related C methods
//

#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "string_utils.h"
#include <cstring>

using namespace Microsoft::CognitiveServices::Speech::Impl;

static inline std::shared_ptr<ISpxConversation> GetConversation(SPXCONVERSATIONHANDLE hconv)
{
    return GetInstance<ISpxConversation>(hconv);
}

SPXAPI conversation_update_participant_by_user_id(SPXCONVERSATIONHANDLE hconv, bool add, const char* userId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, userId == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convhandles = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        auto conversation = (*convhandles)[hconv];

        SPX_IFTRUE_THROW_HR(conversation == nullptr, SPXERR_INVALID_ARG);

        conversation->UpdateParticipant(add, userId);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_update_participant_by_user(SPXCONVERSATIONHANDLE hconv, bool add, SPXUSERHANDLE huser)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, huser == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convhandles = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        auto conversation = (*convhandles)[hconv];

        SPX_IFTRUE_THROW_HR(conversation == nullptr, SPXERR_INVALID_ARG);

        auto userhandles = CSpxSharedPtrHandleTableManager::Get<ISpxUser, SPXPARTICIPANTHANDLE>();

        auto user = SpxQueryInterface<ISpxUser>((*userhandles)[huser]);
        SPX_IFTRUE_THROW_HR(user == nullptr, SPXERR_INVALID_ARG);

        conversation->UpdateParticipant(add, user->GetId());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_update_participant(SPXCONVERSATIONHANDLE hconv, bool add, SPXPARTICIPANTHANDLE hparticipant)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hparticipant == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convhandles = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        auto conversation = (*convhandles)[hconv];

        SPX_IFTRUE_THROW_HR(conversation == nullptr, SPXERR_INVALID_ARG);

        auto participant_handles = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        auto participantObj = (*participant_handles)[hparticipant];
        auto participant = SpxQueryInterface<ISpxParticipant>(participantObj);
        SPX_IFTRUE_THROW_HR(participant == nullptr, SPXERR_INVALID_ARG);

        auto user = SpxQueryInterface<ISpxUser>(participantObj);
        SPX_IFTRUE_THROW_HR(user == nullptr, SPXERR_INVALID_ARG);

        conversation->UpdateParticipant(add, user->GetId(), participant);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_get_conversation_id(SPXCONVERSATIONHANDLE hconv, char* id, size_t id_size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, id == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convhandles = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        auto conversation = (*convhandles)[hconv];

        SPX_IFTRUE_THROW_HR(conversation == nullptr, SPXERR_INVALID_ARG);

        std::string idStr = conversation->GetConversationId();
        SPX_IFTRUE_THROW_HR(idStr.length() >= id_size, SPXERR_INVALID_ARG);
        std::memcpy(id, idStr.c_str(), idStr.length() + 1);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_end_conversation(SPXCONVERSATIONHANDLE hconv)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->EndConversation();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_get_property_bag(SPXCONVERSATIONHANDLE hconv, SPXPROPERTYBAGHANDLE* phpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phpropbag == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phpropbag = SPXHANDLE_INVALID;
        auto conversations = CSpxSharedPtrHandleTableManager::Get<ISpxConversation, SPXCONVERSATIONHANDLE>();
        auto conversation = (*conversations)[hconv];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(conversation);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *phpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_start_conversation(SPXCONVERSATIONHANDLE hconv)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->StartConversation();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_delete_conversation(SPXCONVERSATIONHANDLE hconv)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->DeleteConversation();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_lock_conversation(SPXCONVERSATIONHANDLE hconv)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->SetLockConversation(true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_unlock_conversation(SPXCONVERSATIONHANDLE hconv)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->SetLockConversation(false);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_mute_all_participants(SPXCONVERSATIONHANDLE hconv)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->SetMuteAllParticipants(true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_unmute_all_participants(SPXCONVERSATIONHANDLE hconv)
{
     SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->SetMuteAllParticipants(false);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_mute_participant(SPXCONVERSATIONHANDLE hconv, const char * participantId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, participantId == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->SetMuteParticipant(true, participantId);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_unmute_participant(SPXCONVERSATIONHANDLE hconv, const char * participantId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, participantId == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetConversation(hconv)->SetMuteParticipant(false, participantId);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_release_handle(SPXHANDLE handle)
{
    if (handle == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXCONVERSATIONHANDLE, ISpxConversation>(handle))
    {
        return Handle_Close<SPXCONVERSATIONHANDLE, ISpxConversation>(handle);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}
