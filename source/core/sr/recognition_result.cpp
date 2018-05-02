#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognitionResult::CSpxRecognitionResult():
    m_type { ResultType::Unknown}
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

Reason CSpxRecognitionResult::GetReason()
{
    return m_reason;
}

ResultType CSpxRecognitionResult::GetType()
{
    return m_type;
}

void CSpxRecognitionResult::InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, ResultType type)
{
    m_reason = Reason::IntermediateResult;
    m_type = type;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text;

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls", __FUNCTION__, m_resultId.c_str());
}

void CSpxRecognitionResult::InitFinalResult(const wchar_t* resultId, const wchar_t* text, ResultType type)
{
    m_reason = Reason::Recognized;
    m_type = type;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text == nullptr
        ? L""
        : text;

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls", __FUNCTION__, m_resultId.c_str());
}

void CSpxRecognitionResult::InitNoMatch(ResultType type)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::NoMatch;
    m_type = type;
}

void CSpxRecognitionResult::InitError(const wchar_t* text, ResultType type)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::Canceled;
    m_type = type;
    if (text != nullptr) 
    {
        SetStringValue(g_RESULT_ErrorDetails, text);
    }
}

wstring CSpxRecognitionResult::GetIntentId()
{
    return m_intentId;
}

void CSpxRecognitionResult::InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload)
{
    m_intentId = (intentId != nullptr) ? intentId : L"";

    SetStringValue(g_RESULT_LanguageUnderstandingJson, jsonPayload);
}

const map<wstring, wstring>& CSpxRecognitionResult::GetTranslationText()
{
    return m_translations;
}

TranslationStatus CSpxRecognitionResult::GetTranslationStatus() const
{
    return m_translationStatus;
}

const wstring& CSpxRecognitionResult::GetTranslationFailureReason() const
{
    return m_translationFailureReason;
}

void CSpxRecognitionResult::InitTranslationTextResult(TranslationStatus status, const map<wstring, wstring>& translations, const wstring& failureReason)
{
    m_translations = translations;
    m_translationStatus = status;
    m_translationFailureReason = failureReason;
}

SynthesisStatus CSpxRecognitionResult::GetSynthesisStatus()
{
    return m_synthesisStatus;
}

const wstring& CSpxRecognitionResult::GetSynthesisFailureReason()
{
    return m_synthesisFailureReason;
}

const uint8_t* CSpxRecognitionResult::GetAudio() const
{
    return m_audioBuffer;
}

size_t CSpxRecognitionResult::GetLength() const
{
    return m_audioLength;
}

// ISpxTranslationSynthesisResultInit
void CSpxRecognitionResult::InitTranslationSynthesisResult(SynthesisStatus status, const uint8_t* audioData, size_t audioLength, const wstring& failureReason)
{
    m_synthesisStatus = status;
    m_audioBuffer = audioData;
    m_audioLength = audioLength;
    m_synthesisFailureReason = failureReason;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
