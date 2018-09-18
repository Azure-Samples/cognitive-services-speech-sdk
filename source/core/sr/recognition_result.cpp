#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"
#include "property_id_2_name_map.h"

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognitionResult::CSpxRecognitionResult():
    m_cancellationReason { REASON_CANCELED_NONE },
    m_noMatchReason { NO_MATCH_REASON_NONE }
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxRecognitionResult::~CSpxRecognitionResult()
{
    SPX_DBG_TRACE_FUNCTION();
}

wstring CSpxRecognitionResult::GetResultId()
{
    return m_resultId;
}

wstring CSpxRecognitionResult::GetText()
{
    return m_text;
}

ResultReason CSpxRecognitionResult::GetReason()
{
    return m_reason;
}

CancellationReason CSpxRecognitionResult::GetCancellationReason()
{
    return m_cancellationReason;
}

NoMatchReason CSpxRecognitionResult::GetNoMatchReason()
{
    return m_noMatchReason;
}

void CSpxRecognitionResult::InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, uint64_t offset, uint64_t duration)
{
    m_reason = ResultReason::RecognizingSpeech;
    m_cancellationReason = REASON_CANCELED_NONE;
    m_noMatchReason = NO_MATCH_REASON_NONE;

    m_offset = offset;
    m_duration = duration;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuidWithoutDashes()
        : resultId;

    m_text = text;

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls", __FUNCTION__, m_resultId.c_str());
}

void CSpxRecognitionResult::InitFinalResult(const wchar_t* resultId, ResultReason reason, NoMatchReason noMatchReason, CancellationReason cancellation, const wchar_t* text, uint64_t offset, uint64_t duration)
{
    SPX_DBG_TRACE_FUNCTION();

    m_reason = reason;
    m_cancellationReason = cancellation;
    m_noMatchReason = noMatchReason;

    m_offset = offset;
    m_duration = duration;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuidWithoutDashes()
        : resultId;

    m_text = (text == nullptr || reason == ResultReason::Canceled)
        ? L""
        : text;

    if (reason == ResultReason::Canceled)
    {
        auto errorDetails = (text == nullptr) ? "" : PAL::ToString(text);
        SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), errorDetails.c_str());
    }

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls", __FUNCTION__, m_resultId.c_str());
}

wstring CSpxRecognitionResult::GetIntentId()
{
    return m_intentId;
}

void CSpxRecognitionResult::InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload)
{
    SPX_DBG_TRACE_FUNCTION();

    m_intentId = (intentId != nullptr) ? intentId : L"";

    if (jsonPayload != nullptr && jsonPayload[0] != '\0')
    {
        switch (m_reason)
        {
        case ResultReason::RecognizingSpeech:
            m_reason = ResultReason::RecognizingIntent;
            break;
        case ResultReason::RecognizedSpeech:
            m_reason = ResultReason::RecognizedIntent;
            break;
        default:
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            break;
        }
    }

    SetStringValue(GetPropertyName(PropertyId::LanguageUnderstandingServiceResponse_JsonResult), jsonPayload ? PAL::ToString(std::wstring(jsonPayload)).c_str() : "");
}

const map<wstring, wstring>& CSpxRecognitionResult::GetTranslationText()
{
    return m_translations;
}

void CSpxRecognitionResult::InitTranslationTextResult(TranslationStatusCode status, const map<wstring, wstring>& translations, const wstring& failureReason)
{
    SPX_DBG_TRACE_FUNCTION();

    m_translations = translations;

    if (status != TranslationStatusCode::Error)
    {
        switch (m_reason)
        {
        case ResultReason::RecognizingSpeech:
            m_reason = ResultReason::TranslatingSpeech;
            break;
        case ResultReason::RecognizedSpeech:
            m_reason = ResultReason::TranslatedSpeech;
            break;
        case ResultReason::NoMatch:
            // no match is  also considered as success and passed through.
            break;
        default:
            SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
            break;
        }
    }

    if (status == TranslationStatusCode::Error)
    {
        SPX_DBG_TRACE_VERBOSE("%s: status=TranslationStatusCode::Error; switching result to ResultReason::Canceled", __FUNCTION__);
        auto errorDetails = PAL::ToString(failureReason);
        SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), errorDetails.c_str());
    }
}

const uint8_t* CSpxRecognitionResult::GetAudio() const
{
    return m_audioBuffer;
}

size_t CSpxRecognitionResult::GetLength() const
{
    return m_audioLength;
}

void CSpxRecognitionResult::InitTranslationSynthesisResult(SynthesisStatusCode status, const uint8_t* audioData, size_t audioLength, const wstring& failureReason)
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_DBG_ASSERT(audioLength == 0 || status == SynthesisStatusCode::Success);

    m_audioBuffer = audioData;
    m_audioLength = audioLength;

    m_reason = m_audioLength > 0
        ? ResultReason::SynthesizingAudio
        : ResultReason::SynthesizingAudioCompleted;

    if (status == SynthesisStatusCode::Error)
    {
        SPX_DBG_TRACE_VERBOSE("%s: status=SynthesisStatusCode::Error; switching result to ResultReason::Canceled", __FUNCTION__);
        m_reason = ResultReason::Canceled;
        m_cancellationReason = CancellationReason::Error;

        auto errorDetails = PAL::ToString(failureReason);
        SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), errorDetails.c_str());
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl

