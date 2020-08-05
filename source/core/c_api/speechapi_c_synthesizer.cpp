//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_synthesizer.cpp: Public API definitions for Synthesizer related C methods

#include "stdafx.h"
#include "common.h"
#include "service_helpers.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "platform.h"
#include "string_utils.h"
#include "async_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech;

static_assert((int)SpeechSynthesisOutputFormat_Raw8Khz8BitMonoMULaw == (int)SpeechSynthesisOutputFormat::Raw8Khz8BitMonoMULaw, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Riff16Khz16KbpsMonoSiren == (int)SpeechSynthesisOutputFormat::Riff16Khz16KbpsMonoSiren, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio16Khz16KbpsMonoSiren == (int)SpeechSynthesisOutputFormat::Audio16Khz16KbpsMonoSiren, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio16Khz32KBitRateMonoMp3 == (int)SpeechSynthesisOutputFormat::Audio16Khz32KBitRateMonoMp3, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio16Khz128KBitRateMonoMp3 == (int)SpeechSynthesisOutputFormat::Audio16Khz128KBitRateMonoMp3, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio16Khz64KBitRateMonoMp3 == (int)SpeechSynthesisOutputFormat::Audio16Khz64KBitRateMonoMp3, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio24Khz48KBitRateMonoMp3 == (int)SpeechSynthesisOutputFormat::Audio24Khz48KBitRateMonoMp3, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio24Khz96KBitRateMonoMp3 == (int)SpeechSynthesisOutputFormat::Audio24Khz96KBitRateMonoMp3, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Audio24Khz160KBitRateMonoMp3 == (int)SpeechSynthesisOutputFormat::Audio24Khz160KBitRateMonoMp3, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Raw16Khz16BitMonoTrueSilk == (int)SpeechSynthesisOutputFormat::Raw16Khz16BitMonoTrueSilk, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Riff16Khz16BitMonoPcm == (int)SpeechSynthesisOutputFormat::Riff16Khz16BitMonoPcm, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Riff8Khz16BitMonoPcm == (int)SpeechSynthesisOutputFormat::Riff8Khz16BitMonoPcm, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Riff24Khz16BitMonoPcm == (int)SpeechSynthesisOutputFormat::Riff24Khz16BitMonoPcm, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Riff8Khz8BitMonoMULaw == (int)SpeechSynthesisOutputFormat::Riff8Khz8BitMonoMULaw, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Raw16Khz16BitMonoPcm == (int)SpeechSynthesisOutputFormat::Raw16Khz16BitMonoPcm, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Raw24Khz16BitMonoPcm == (int)SpeechSynthesisOutputFormat::Raw24Khz16BitMonoPcm, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");
static_assert((int)SpeechSynthesisOutputFormat_Raw8Khz16BitMonoPcm == (int)SpeechSynthesisOutputFormat::Raw8Khz16BitMonoPcm, "SpeechSynthesisOutputFormat_* enum values == SpeechSynthesisOutputFormat::* enum values");

static_assert((int)StreamStatus_Unknown == (int)StreamStatus::Unknown, "StreamStatus_* enum values == StreamStatus::* enum values");
static_assert((int)StreamStatus_NoData == (int)StreamStatus::NoData, "StreamStatus_* enum values == StreamStatus::* enum values");
static_assert((int)StreamStatus_PartialData == (int)StreamStatus::PartialData, "StreamStatus_* enum values == StreamStatus::* enum values");
static_assert((int)StreamStatus_AllData == (int)StreamStatus::AllData, "StreamStatus_* enum values == StreamStatus::* enum values");
static_assert((int)StreamStatus_Canceled == (int)StreamStatus::Canceled, "StreamStatus_* enum values == StreamStatus::* enum values");

SPXAPI_(bool) synthesizer_handle_is_valid(SPXSYNTHHANDLE hsynth)
{
    return Handle_IsValid<SPXSYNTHHANDLE, ISpxSynthesizer>(hsynth);
}

SPXAPI synthesizer_handle_release(SPXSYNTHHANDLE hsynth)
{
    return Handle_Close<SPXSYNTHHANDLE, ISpxSynthesizer>(hsynth);
}

SPXAPI_(bool) synthesizer_async_handle_is_valid(SPXASYNCHANDLE hasync)
{
    return Handle_IsValid<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync)
        ? Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<void>>(hasync)
        : Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(hasync);
}

SPXAPI synthesizer_async_handle_release(SPXASYNCHANDLE hasync)
{
    return Handle_Close<SPXASYNCHANDLE, CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(hasync);
}

SPXAPI_(bool) synthesizer_result_handle_is_valid(SPXRESULTHANDLE hresult)
{
    return Handle_IsValid<SPXRESULTHANDLE, ISpxSynthesisResult>(hresult);
}

SPXAPI synthesizer_result_handle_release(SPXRESULTHANDLE hresult)
{
    return Handle_Close<SPXRESULTHANDLE, ISpxSynthesisResult>(hresult);
}

SPXAPI_(bool) synthesizer_event_handle_is_valid(SPXEVENTHANDLE hevent)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxSynthesisEventArgs>(hevent) ||
        Handle_IsValid<SPXEVENTHANDLE, ISpxWordBoundaryEventArgs>(hevent);
}

SPXAPI synthesizer_event_handle_release(SPXEVENTHANDLE hevent)
{
    if (Handle_IsValid<SPXEVENTHANDLE, ISpxSynthesisEventArgs>(hevent))
    {
        return Handle_Close<SPXEVENTHANDLE, ISpxSynthesisEventArgs>(hevent);
    }
    else
    {
        return Handle_Close<SPXEVENTHANDLE, ISpxWordBoundaryEventArgs>(hevent);
    }
}

SPXAPI synthesizer_enable(SPXSYNTHHANDLE hsynth)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];
        synthesizer->Enable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_disable(SPXSYNTHHANDLE hsynth)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];
        synthesizer->Disable();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_is_enabled(SPXSYNTHHANDLE hsynth, bool* pfEnabled)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pfEnabled == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];
        *pfEnabled = synthesizer->IsEnabled();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_get_property_bag(SPXSYNTHHANDLE hsynth, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !synthesizer_handle_is_valid(hsynth));
    return GetTargetObjectByService<ISpxSynthesizer, ISpxNamedProperties>(hsynth, hpropbag);
}

SPXAPI synthesizer_speak_text(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, text == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto result = synthesizer->Speak(std::string(text, textLength), false);
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_speak_ssml(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ssml == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto result = synthesizer->Speak(std::string(ssml, ssmlLength), true);
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_speak_text_async(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, text == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto asyncop = synthesizer->SpeakAsync(std::string(text, textLength), false);
        auto ptr = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_speak_ssml_async(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ssml == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto asyncop = synthesizer->SpeakAsync(std::string(ssml, ssmlLength), true);
        auto ptr = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_start_speaking_text(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, text == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto result = synthesizer->StartSpeaking(std::string(text, textLength), false);
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_start_speaking_ssml(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ssml == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto result = synthesizer->StartSpeaking(std::string(ssml, ssmlLength), true);
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_start_speaking_text_async(SPXSYNTHHANDLE hsynth, const char* text, uint32_t textLength, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, text == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto asyncop = synthesizer->StartSpeakingAsync(std::string(text, textLength), false);
        auto ptr = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_start_speaking_ssml_async(SPXSYNTHHANDLE hsynth, const char* ssml, uint32_t ssmlLength, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ssml == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto asyncop = synthesizer->StartSpeakingAsync(std::string(ssml, ssmlLength), true);
        auto ptr = std::make_shared<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_speak_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<std::shared_ptr<ISpxSynthesisResult>>, SPXASYNCHANDLE>();
        auto asyncop = (*asynchandles)[hasync];

        hr = SPXERR_TIMEOUT;
        auto completed = asyncop->WaitFor(milliseconds);
        if (completed)
        {
            auto result = asyncop->Future.get();
            if (result == nullptr)
            {
                hr = SPXERR_TIMEOUT;
            }
            else
            {
                auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
                *phresult = resulthandles->TrackHandle(result);
                hr = SPX_NOERROR;
            }
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_stop_speaking(SPXSYNTHHANDLE hsynth)
{

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        synthesizer->StopSpeaking();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_stop_speaking_async(SPXSYNTHHANDLE hsynth, SPXASYNCHANDLE* phasync)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phasync == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phasync = SPXHANDLE_INVALID;

        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto asyncop = synthesizer->StopSpeakingAsync();
        auto ptr = std::make_shared<CSpxAsyncOp<void>>(std::move(asyncop));

        auto asynchandles = CSpxSharedPtrHandleTableManager::Get<CSpxAsyncOp<void>, SPXASYNCHANDLE>();
        *phasync = asynchandles->TrackHandle(ptr);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_stop_async_wait_for(SPXASYNCHANDLE hasync, uint32_t milliseconds)
{
    return async_operation_wait_for(hasync, milliseconds);
}

SPXAPI synthesizer_started_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return synthesizer_set_event_callback(&ISpxSynthesizerEvents::SynthesisStarted, hsynth, pCallback, pvContext);
}

SPXAPI synthesizer_synthesizing_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return synthesizer_set_event_callback(&ISpxSynthesizerEvents::Synthesizing, hsynth, pCallback, pvContext);
}

SPXAPI synthesizer_completed_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return synthesizer_set_event_callback(&ISpxSynthesizerEvents::SynthesisCompleted, hsynth, pCallback, pvContext);
}

SPXAPI synthesizer_canceled_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return synthesizer_set_event_callback(&ISpxSynthesizerEvents::SynthesisCanceled, hsynth, pCallback, pvContext);
}

SPXAPI synthesizer_word_boundary_set_callback(SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    return synthesizer_word_boundary_set_event_callback(&ISpxSynthesizerEvents::WordBoundary, hsynth, pCallback, pvContext);
}

SPXAPI synthesizer_synthesis_event_get_result(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phresult == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisEventArgs, SPXEVENTHANDLE>();
        auto synthEvent = (*eventhandles)[hevent];
        auto result = synthEvent->GetResult();

        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        *phresult = resulthandles->TrackHandle(result);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synthesizer_word_boundary_event_get_values(SPXEVENTHANDLE hevent, uint64_t* pAudioOffset, uint32_t* pTextOffset, uint32_t* pWordLength)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pAudioOffset == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pTextOffset == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pWordLength == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxWordBoundaryEventArgs, SPXEVENTHANDLE>();
        auto wordBoundaryEvent = (*eventhandles)[hevent];
        *pAudioOffset = wordBoundaryEvent->GetAudioOffset();
        *pTextOffset = wordBoundaryEvent->GetTextOffset();
        *pWordLength = wordBoundaryEvent->GetWordLength();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
