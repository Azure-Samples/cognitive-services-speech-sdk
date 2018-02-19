#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"


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


} // CARBON_IMPL_NAMESPACE
