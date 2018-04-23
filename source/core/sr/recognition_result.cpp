#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"
#include "named_properties_constants.h"


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

std::wstring CSpxRecognitionResult::GetResultId()
{
    return m_resultId;
}

std::wstring CSpxRecognitionResult::GetText()
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

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%S", __FUNCTION__, m_resultId.c_str());
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

    SPX_DBG_TRACE_VERBOSE("%s: resultId=%S", __FUNCTION__, m_resultId.c_str());
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
    m_text = (text == nullptr)
         ? L""
         : text;
}

std::wstring CSpxRecognitionResult::GetIntentId()
{
    return m_intentId;
}

void CSpxRecognitionResult::InitIntentResult(const wchar_t* intentId, const wchar_t* jsonPayload)
{
    m_intentId = intentId != nullptr
        ? intentId
        : L"";

    SetStringValue(g_RESULT_LanguageUnderstandingJson, jsonPayload);
}

const std::map<std::wstring, std::wstring>& CSpxRecognitionResult::GetTranslationText()
{
    return m_translations;
}


void CSpxRecognitionResult::InitTranslationTextResult(ISpxTranslationStatus status, const std::map<std::wstring, std::wstring>& translations)
{
    m_translations = translations;
    m_translationStatus = status;
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
void CSpxRecognitionResult::InitTranslationSynthesisResult(const uint8_t* audioData, size_t audioLength)
{
    m_audioBuffer = audioData;
    m_audioLength = audioLength;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
