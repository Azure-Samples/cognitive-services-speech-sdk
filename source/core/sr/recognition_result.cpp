#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"
#include "named_properties_constants.h"


namespace CARBON_IMPL_NAMESPACE() {


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

void CSpxRecognitionResult::InitIntermediateResult(const wchar_t* resultId, const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::IntermediateResult;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text;
}

void CSpxRecognitionResult::InitFinalResult(const wchar_t* resultId, const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::Recognized;

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_text = text == nullptr
        ? L""
        : text;
}

void CSpxRecognitionResult::InitNoMatch()
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::NoMatch;
}

void CSpxRecognitionResult::InitError(const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();
    m_reason = Reason::Canceled;
    m_text = (text == nullptr) ? L"" : text;
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

    SetStringValue(g_RESULT_LuisJson, jsonPayload);
}


} // CARBON_IMPL_NAMESPACE
