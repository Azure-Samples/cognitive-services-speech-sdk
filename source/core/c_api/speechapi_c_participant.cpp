//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_participant.cpp: Public API declarations for conversation transcriber participant related C methods and enumerations
//

#include "stdafx.h"
#include <speechapi_c_common.h>
#include "create_object_helpers.h"
#include "handle_helpers.h"
#include "handle_table.h"
#include "platform.h"
#include "site_helpers.h"
#include "string_utils.h"
#include "spxdebug.h"
#include <assert.h>

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace std;


SPXAPI participant_create_handle(SPXPARTICIPANTHANDLE* hparticipant, const char* userId, const char* preferred_language, const char* voice_signature)
{
    // all the arguments after userId can be optional
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, userId == nullptr || !(*userId));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hparticipant == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *hparticipant = SPXHANDLE_INVALID;

        auto participant = SpxCreateObjectWithSite<ISpxParticipant>("CSpxParticipant", SpxGetRootSite());

        auto user = SpxQueryInterface<ISpxUser>(participant);
        SPX_IFTRUE_THROW_HR(user == nullptr, SPXERR_INVALID_ARG);
        user->InitFromUserId(userId);

        if (preferred_language)
        {
            participant->SetPreferredLanguage(preferred_language);
        }

        if (voice_signature)
        {
            string voice{ voice_signature };
            participant->SetVoiceSignature(move(voice));
        }

        auto participanthandles = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        *hparticipant = participanthandles->TrackHandle(participant);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI participant_release_handle(SPXPARTICIPANTHANDLE hparticipant)
{
    return Handle_Close<SPXPARTICIPANTHANDLE, ISpxParticipant>(hparticipant);
}

SPXAPI participant_set_preferred_langugage(SPXPARTICIPANTHANDLE hparticipant, const char* preferred_language)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, preferred_language == nullptr || !(*preferred_language));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto participants = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        auto participant_ptr = (*participants)[hparticipant];

        SPX_IFTRUE_THROW_HR(participant_ptr == nullptr, SPXERR_INVALID_ARG);
        auto participant = SpxQueryInterface<ISpxParticipant>(participant_ptr);
        if (participant && preferred_language)
        {
            participant->SetPreferredLanguage(preferred_language);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI participant_set_voice_signature(SPXPARTICIPANTHANDLE hparticipant, const char* voice_signature)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, voice_signature == nullptr || !(*voice_signature));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto participants = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        auto participant_ptr = (*participants)[hparticipant];

        SPX_IFTRUE_THROW_HR(participant_ptr == nullptr, SPXERR_INVALID_ARG);
        auto participant = SpxQueryInterface<ISpxParticipant>(participant_ptr);

        if (participant)
        {
            participant->SetVoiceSignature({ voice_signature });
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI participant_get_property_bag(SPXPARTICIPANTHANDLE hparticipant, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);

    *hpropbag = SPXHANDLE_INVALID;
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto participant_handles = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        auto participant = (*participant_handles)[hparticipant];
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(participant);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        if (baghandle)
        {
            *hpropbag = baghandle->TrackHandle(namedProperties);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
