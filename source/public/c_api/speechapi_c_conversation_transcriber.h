//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_conversation_transcriber.h: Public API declarations for conversation trancriber related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI conversation_transcriber_end_conversation(SPXRECOHANDLE hreco);
SPXAPI conversation_transcriber_update_participant_by_user_id(SPXRECOHANDLE hreco, bool add, const char* userId);
SPXAPI conversation_transcriber_update_participant_by_user(SPXRECOHANDLE hreco, bool add, SPXUSERHANDLE huser);
SPXAPI conversation_transcriber_update_participant(SPXRECOHANDLE hreco, bool add, SPXPARTICIPANTHANDLE hparticipant);
SPXAPI conversation_transcriber_set_conversation_id(SPXRECOHANDLE hreco, const char* id);
SPXAPI conversation_transcriber_get_conversation_id(SPXRECOHANDLE hreco, char* id, size_t size);
