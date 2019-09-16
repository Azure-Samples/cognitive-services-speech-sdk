//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_conversation_transcriber.cpp: Public API definitions for ConversationTranscriber related C methods
//

#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "string_utils.h"
#include <cstring>


using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI conversation_transcriber_update_participant_by_user_id(SPXRECOHANDLE hreco, bool add, const char* userId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, userId == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto conversationTranscriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);
        SPX_IFTRUE_THROW_HR(conversationTranscriber == nullptr, SPXERR_INVALID_ARG);

        conversationTranscriber->UpdateParticipant(add, userId);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_transcriber_update_participant_by_user(SPXRECOHANDLE hreco, bool add, SPXUSERHANDLE huser)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, huser == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto conversationTranscriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);
        SPX_IFTRUE_THROW_HR(conversationTranscriber == nullptr, SPXERR_INVALID_ARG);

        auto userhandles = CSpxSharedPtrHandleTableManager::Get<ISpxUser, SPXPARTICIPANTHANDLE>();

        auto user = SpxQueryInterface<ISpxUser>((*userhandles)[huser]);
        SPX_IFTRUE_THROW_HR(user == nullptr, SPXERR_INVALID_ARG);

        conversationTranscriber->UpdateParticipant(add, user->GetId());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_transcriber_update_participant(SPXRECOHANDLE hreco, bool add, SPXPARTICIPANTHANDLE hparticipant)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hparticipant == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto conversationTranscriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);
        SPX_IFTRUE_THROW_HR(conversationTranscriber == nullptr, SPXERR_INVALID_ARG);

        auto participant_handels = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        auto participantObj = (*participant_handels)[hparticipant];
        auto participant = SpxQueryInterface<ISpxParticipant>(participantObj);
        SPX_IFTRUE_THROW_HR(participant == nullptr, SPXERR_INVALID_ARG);

        auto user = SpxQueryInterface<ISpxUser>(participantObj);
        SPX_IFTRUE_THROW_HR(user == nullptr, SPXERR_INVALID_ARG);

        conversationTranscriber->UpdateParticipant(add, user->GetId(), participant);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_transcriber_set_conversation_id(SPXRECOHANDLE hreco, const char* id)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, id == nullptr || !(*id));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto conversationTranscriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);
        SPX_IFTRUE_THROW_HR(conversationTranscriber == nullptr, SPXERR_INVALID_ARG);

        conversationTranscriber->SetConversationId(id);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_transcriber_get_conversation_id(SPXRECOHANDLE hreco, char* id, size_t id_size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, id == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto conversationTranscriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);
        SPX_IFTRUE_THROW_HR(conversationTranscriber == nullptr, SPXERR_INVALID_ARG);

        std::string idStr;
        conversationTranscriber->GetConversationId(idStr);
        SPX_IFTRUE_THROW_HR(idStr.length() >= id_size, SPXERR_INVALID_ARG);
        std::memcpy(id, idStr.c_str(), idStr.length() + 1);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_transcriber_end_conversation(SPXRECOHANDLE hreco, bool destroy)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto conversationTranscriber = SpxQueryInterface<ISpxConversationTranscriber>(recognizer);
        SPX_IFTRUE_THROW_HR(conversationTranscriber == nullptr, SPXERR_INVALID_ARG);

        conversationTranscriber->EndConversation(destroy);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
