//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_synthesizer.h: Public API declarations for Synthesizer related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) synthesizer_handle_is_valid(SPXSYNTHHANDLE hsynth);
SPXAPI synthesizer_handle_release(SPXSYNTHHANDLE hsynth);

SPXAPI_(bool) synthesizer_async_handle_is_valid(SPXASYNCHANDLE hasync);
SPXAPI synthesizer_async_handle_release(SPXASYNCHANDLE hasync);

SPXAPI_(bool) synthesizer_result_handle_is_valid(SPXRESULTHANDLE hresult);
SPXAPI synthesizer_result_handle_release(SPXRESULTHANDLE hresult);

SPXAPI_(bool) synthesizer_event_handle_is_valid(SPXEVENTHANDLE hevent);
SPXAPI synthesizer_event_handle_release(SPXEVENTHANDLE hevent);

SPXAPI synthesizer_enable(SPXSYNTHHANDLE hsynth);
SPXAPI synthesizer_disable(SPXSYNTHHANDLE hsynth);
SPXAPI synthesizer_is_enabled(SPXSYNTHHANDLE hsynth, bool* pfEnabled);

SPXAPI synthesizer_get_property_bag(SPXSYNTHHANDLE hsynth, SPXPROPERTYBAGHANDLE* hpropbag);

SPXAPI synthesizer_speak_text(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXRESULTHANDLE* phresult);
SPXAPI synthesizer_speak_ssml(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXRESULTHANDLE* phresult);
SPXAPI synthesizer_speak_text_async(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXASYNCHANDLE* phasync);
SPXAPI synthesizer_speak_ssml_async(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXASYNCHANDLE* phasync);
SPXAPI synthesizer_start_speaking_text(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXRESULTHANDLE* phresult);
SPXAPI synthesizer_start_speaking_ssml(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXRESULTHANDLE* phresult);
SPXAPI synthesizer_start_speaking_text_async(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXASYNCHANDLE* phasync);
SPXAPI synthesizer_start_speaking_ssml_async(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXASYNCHANDLE* phasync);
SPXAPI synthesizer_speak_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult);

typedef void(*PSYNTHESIS_CALLBACK_FUNC)(SPXSYNTHHANDLE hsynth, SPXEVENTHANDLE hevent, void* pvContext);
SPXAPI synthesizer_started_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI synthesizer_synthesizing_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI synthesizer_completed_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI synthesizer_canceled_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI synthesizer_word_boundary_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext);

SPXAPI synthesizer_synthesis_event_get_result(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult);
SPXAPI synthesizer_word_boundary_event_get_values(SPXEVENTHANDLE hevent, uint64_t* pAudioOffset, uint32_t* pTextOffset, uint32_t* pWordLength);
