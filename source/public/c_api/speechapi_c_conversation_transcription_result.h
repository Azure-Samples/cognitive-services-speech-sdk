//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_conversation_transcriber_result.h: Public API declarations for ConversationTranscriberResult related C methods and enumerations
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI conversation_transcription_result_get_user_id(SPXRESULTHANDLE hresult, char* pszUserId, uint32_t cchUserId);
SPXAPI conversation_transcription_result_get_utterance_id(SPXRESULTHANDLE hresult, char* pszUtteranceId, uint32_t cchUtteranceId);
