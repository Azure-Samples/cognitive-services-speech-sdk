#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"
#include "named_properties_constants.h"

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognitionResult::CSpxRecognitionResult()
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

void CSpxRecognitionResult::InitIntermediateResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type)
{
    m_reason = Reason::IntermediateResult;
    m_type = type;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text;

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%ls", __FUNCTION__, m_resultId.c_str());
}

void CSpxRecognitionResult::InitFinalResult(const wchar_t* resultId, const wchar_t* text, enum ResultType type)
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

void CSpxRecognitionResult::InitNoMatch(enum ResultType type)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::NoMatch;
    m_type = type;
}

void CSpxRecognitionResult::InitError(const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::Canceled;
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

TranslationTextStatus CSpxRecognitionResult::GetTextStatus() const
{
    return m_translationTextStatus;
}

const wstring& CSpxRecognitionResult::GetTextFailureReason() const
{
    return m_translationTextFailureReason;
}

void CSpxRecognitionResult::InitTranslationTextResult(TranslationTextStatus status, const map<wstring, wstring>& translations, const wstring& failureReason)
{
    m_translations = translations;
    m_translationTextStatus = status;
    m_translationTextFailureReason = failureReason;
}

TranslationSynthesisStatus CSpxRecognitionResult::GetSynthesisStatus()
{
    return m_translationSynthesisStatus;
}

const wstring& CSpxRecognitionResult::GetSynthesisFailureReason()
{
    return m_translationSynthesisFailureReason;
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
void CSpxRecognitionResult::InitTranslationSynthesisResult(TranslationSynthesisStatus status, const uint8_t* audioData, size_t audioLength, const wstring& failureReason)
{
    m_translationSynthesisStatus = status;
    m_audioBuffer = audioData;
    m_audioLength = audioLength;
    m_translationSynthesisFailureReason = failureReason;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
