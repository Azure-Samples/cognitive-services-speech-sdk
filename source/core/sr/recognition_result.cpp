#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"
#include "property_id_2_name_map.h"
#include "json.h"

using json = nlohmann::json;

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognitionResult::CSpxRecognitionResult():
    m_cancellationReason { REASON_CANCELED_NONE },
    m_cancellationErrorCode { CancellationErrorCode::NoError },
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

CancellationErrorCode CSpxRecognitionResult::GetCancellationErrorCode()
{
    return m_cancellationErrorCode;
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

void CSpxRecognitionResult::InitFinalResult(const wchar_t* resultId, ResultReason reason, NoMatchReason noMatchReason, CancellationReason cancellation, CancellationErrorCode errorCode, const wchar_t* text, uint64_t offset, uint64_t duration)
{
    SPX_DBG_TRACE_FUNCTION();

    m_reason = reason;
    m_cancellationReason = cancellation;
    m_cancellationErrorCode = errorCode;
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

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls reason=%d, cancellationReason=%d, text='%ls'", __FUNCTION__, m_resultId.c_str(), (int)m_reason, (int)m_cancellationReason, m_text.c_str());
}

void CSpxRecognitionResult::SetLatency(uint64_t latencyInTicks)
{
    SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_RecognitionLatency), std::to_string(latencyInTicks).c_str());
}

wstring CSpxRecognitionResult::GetIntentId()
{
    return m_intentId;
}

double CSpxRecognitionResult::GetConfidence()
{
    return m_confidence;
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

    SetStringValue(GetPropertyName(PropertyId::LanguageUnderstandingServiceResponse_JsonResult), jsonPayload ? PAL::ToString(jsonPayload).c_str() : "");
}

void CSpxRecognitionResult::InitKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, bool is_verified)
{
    SPX_DBG_TRACE_FUNCTION();

    m_reason = is_verified ? ResultReason::RecognizedKeyword : ResultReason::RecognizingKeyword;
    m_cancellationReason = REASON_CANCELED_NONE;
    m_cancellationErrorCode = CancellationErrorCode::NoError;
    m_noMatchReason = NO_MATCH_REASON_NONE;

    m_offset = offset;
    m_duration = duration;
    m_confidence = confidence;

    m_resultId = PAL::CreateGuidWithoutDashes();
    m_text = keyword;

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls", __FUNCTION__, m_resultId.c_str());
}

const map<wstring, wstring>& CSpxRecognitionResult::GetTranslationText()
{
    return m_translations;
}

void CSpxRecognitionResult::InitTranslationRecognitionResult(TranslationStatusCode status, const map<wstring, wstring>& translations, const wstring& failureReason)
{
    SPX_DBG_TRACE_FUNCTION();

    m_translations = translations;

    if (status == TranslationStatusCode::Success)
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
    else if (status == TranslationStatusCode::Error)
    {
        // Since speech recognition is successful but only translation returns an error, we do not create a recognition error event.
        // Instead, m_reason is not upgraded to TranslatingSpeech/TranslatedSpeech, but remains as RecognizingSpeech/RecognizedSpeech,
        // and the property SpeechServiceResponse_JsonErrorDetails is set with the translation error details.
        auto errorDetails = PAL::ToString(failureReason);
        SPX_DBG_TRACE_VERBOSE("%s: Recognition succeeded but translation has error. Error details: %s", __FUNCTION__, errorDetails.c_str());
        SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), errorDetails.c_str());
    }
    else
    {
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
}

const uint8_t* CSpxRecognitionResult::GetAudio() const
{
    return m_audioBuffer.data();
}

size_t CSpxRecognitionResult::GetLength() const
{
    return m_audioLength;
}

void CSpxRecognitionResult::InitTranslationSynthesisResult(const uint8_t* audioData, size_t audioLength)
{
    SPX_DBG_TRACE_FUNCTION();

    m_audioBuffer.assign(audioData, audioData + audioLength);
    m_audioLength = audioLength;

    m_reason = m_audioLength > 0
        ? ResultReason::SynthesizingAudio
        : ResultReason::SynthesizingAudioCompleted;
}

void CSpxRecognitionResult::SetStringValue(const char* name, const char* value)
{
    ISpxPropertyBagImpl::SetStringValue(name, value);

    if (PAL::stricmp(name, GetPropertyName(PropertyId::SpeechServiceResponse_JsonResult)) == 0)
    {
        InitPropertiesFromJsonResult(value);
    }
}

void CSpxRecognitionResult::InitPropertiesFromJsonResult(const char* value)
{
    if (value != nullptr && value[0] != '\0')
    {
        SPX_DBG_TRACE_VERBOSE("%s: json='%s'", __FUNCTION__, value);
        auto root = json::parse(value);
        auto nBest = root["NBest"];
        if (nBest.is_array())
        {
            auto firstBest = nBest[0];
            auto itn = firstBest["ITN"].get<string>();
            if (!itn.empty())
            {
                SetStringValue("ITN", itn.c_str());
            }

            auto lexical = firstBest["Lexical"].get<string>();
            if (!lexical.empty())
            {
                SetStringValue("Lexical", lexical.c_str());
            }
        }
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
