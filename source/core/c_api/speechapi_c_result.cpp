//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_result.cpp: Public API definitions for Result related C methods
//

#include "stdafx.h"
#include "string_utils.h"

#include "ispxinterfaces.h" // for SpxQueryInterface

using namespace Microsoft::CognitiveServices::Speech::Impl;

static_assert((int)ResultReason_NoMatch == (int)ResultReason::NoMatch, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_Canceled == (int)ResultReason::Canceled, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_RecognizingSpeech == (int)ResultReason::RecognizingSpeech, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_RecognizedSpeech == (int)ResultReason::RecognizedSpeech, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_RecognizingIntent == (int)ResultReason::RecognizingIntent, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_RecognizedIntent == (int)ResultReason::RecognizedIntent, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_TranslatingSpeech == (int)ResultReason::TranslatingSpeech, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_TranslatedSpeech == (int)ResultReason::TranslatedSpeech, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_SynthesizingAudio == (int)ResultReason::SynthesizingAudio, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_SynthesizingAudioComplete == (int)ResultReason::SynthesizingAudioCompleted, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_RecognizingKeyword == (int)ResultReason::RecognizingKeyword, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_RecognizedKeyword == (int)ResultReason::RecognizedKeyword, "ResultReason_* enum values == ResultReason::* enum values");
static_assert((int)ResultReason_SynthesizingAudioStart == (int)ResultReason::SynthesizingAudioStarted, "ResultReason_* enum values == ResultReason::* enum values");

static_assert((int)CancellationReason_Error == (int)CancellationReason::Error, "CancellationReason_* enum values == CancellationReason::* enum values");
static_assert((int)CancellationReason_EndOfStream == (int)CancellationReason::EndOfStream, "CancellationReason_* enum values == CancellationReason::* enum values");

static_assert((int)CancellationErrorCode_NoError == (int)CancellationErrorCode::NoError, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_AuthenticationFailure == (int)CancellationErrorCode::AuthenticationFailure, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_BadRequest == (int)CancellationErrorCode::BadRequest, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_TooManyRequests == (int)CancellationErrorCode::TooManyRequests, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_Forbidden == (int)CancellationErrorCode::Forbidden, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_ConnectionFailure == (int)CancellationErrorCode::ConnectionFailure, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_ServiceTimeout == (int)CancellationErrorCode::ServiceTimeout, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_ServiceError == (int)CancellationErrorCode::ServiceError, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_ServiceUnavailable == (int)CancellationErrorCode::ServiceUnavailable, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");
static_assert((int)CancellationErrorCode_RuntimeError == (int)CancellationErrorCode::RuntimeError, "CancellationErrorCode_* enum values == CancellationErrorCode::* enum values");

static_assert((int)NoMatchReason_NotRecognized == (int)NoMatchReason::NotRecognized, "NoMatchReason_* enum values == NoMatchReason::* enum values");
static_assert((int)NoMatchReason_InitialSilenceTimeout == (int)NoMatchReason::InitialSilenceTimeout, "NoMatchReason_* enum values == NoMatchReason::* enum values");
static_assert((int)NoMatchReason_InitialBabbleTimeout == (int)NoMatchReason::InitialBabbleTimeout, "NoMatchReason_* enum values == NoMatchReason::* enum values");
static_assert((int)NoMatchReason_KeywordNotRecognized == (int)NoMatchReason::KeywordNotRecognized, "NoMatchReason_* enum values == NoMatchReason::* enum values");

SPXAPI result_get_result_id(SPXRESULTHANDLE hresult, char* pszResultId, uint32_t cchResultId)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchResultId == 0);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = PAL::ToString(result->GetResultId());
        auto pszActual = strActual.c_str();
        PAL::strcpy(pszResultId, cchResultId, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_reason(SPXRESULTHANDLE hresult, Result_Reason* reason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reason == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *reason = (Result_Reason)result->GetReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_reason_canceled(SPXRESULTHANDLE hresult, Result_CancellationReason* reason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reason == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *reason = (Result_CancellationReason)result->GetCancellationReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_canceled_error_code(SPXRESULTHANDLE hresult, Result_CancellationErrorCode* errorCode)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, errorCode == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *errorCode = (Result_CancellationErrorCode)result->GetCancellationErrorCode();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_no_match_reason(SPXRESULTHANDLE hresult, Result_NoMatchReason* reason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reason == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *reason = (Result_NoMatchReason)result->GetNoMatchReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_text(SPXRESULTHANDLE hresult, char* pszText, uint32_t cchText)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, cchText == 0);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto strActual = PAL::ToString(result->GetText());
        auto pszActual = strActual.c_str();
        PAL::strcpy(pszText, cchText, pszActual, strActual.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_offset(SPXRESULTHANDLE hresult, uint64_t* offset)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, offset == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *offset = result->GetOffset();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_duration(SPXRESULTHANDLE hresult, uint64_t* duration)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, duration == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *duration = result->GetDuration();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI result_get_property_bag(SPXRESULTHANDLE hresult, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_result_id(SPXRESULTHANDLE hresult, char* resultId, uint32_t resultIdLength)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, resultId == 0);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];

        auto resultIdStr = PAL::ToString(result->GetResultId());
        PAL::strcpy(resultId, resultIdLength, resultIdStr.c_str(), resultIdStr.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_reason(SPXRESULTHANDLE hresult, Result_Reason* reason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reason == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];
        *reason = (Result_Reason)result->GetReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_reason_canceled(SPXRESULTHANDLE hresult, Result_CancellationReason* reason)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, reason == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *reason = (Result_CancellationReason)result->GetCancellationReason();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_canceled_error_code(SPXRESULTHANDLE hresult, Result_CancellationErrorCode* errorCode)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, errorCode == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resulthandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resulthandles)[hresult];
        *errorCode = (Result_CancellationErrorCode)result->GetCancellationErrorCode();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_audio_length(SPXRESULTHANDLE hresult, uint32_t* length)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, length == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];
        *length = result->GetAudioLength();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_audio_data(SPXRESULTHANDLE hresult, uint8_t* buffer, uint32_t bufferSize, uint32_t* filledSize)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, filledSize == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];
        auto audiodata = result->GetAudioData();

        uint32_t audioSize = (uint32_t)(audiodata->size());
        *filledSize = audioSize < bufferSize ? audioSize : bufferSize;
        memcpy(buffer, audiodata->data(), *filledSize);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_audio_format(SPXRESULTHANDLE hresult, SPXAUDIOSTREAMFORMATHANDLE* hformat)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hformat == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];

        auto formatSize = result->GetFormat(nullptr, 0);
        auto format = SpxAllocWAVEFORMATEX(formatSize);
        result->GetFormat(format.get(), formatSize);

        *hformat = CSpxSharedPtrHandleTableManager::TrackHandle<SPXWAVEFORMATEX, SPXAUDIOSTREAMFORMATHANDLE>(format);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI synth_result_get_property_bag(SPXRESULTHANDLE hresult, SPXPROPERTYBAGHANDLE* hpropbag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hpropbag == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto resultshandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisResult, SPXRESULTHANDLE>();
        auto result = (*resultshandles)[hresult];
        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(result);

        auto baghandle = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *hpropbag = baghandle->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
