#include "stdafx.h"
#include "recognition_result.h"
#include "guid_utils.h"


namespace CARBON_IMPL_NAMESPACE() {

constexpr /*static*/ CSpxRecognitionResult::NoMatch_Type CSpxRecognitionResult::NoMatch;
constexpr /*static*/ CSpxRecognitionResult::IntermediateResult_Type CSpxRecognitionResult::IntermediateResult;

CSpxRecognitionResult::CSpxRecognitionResult(const wchar_t* resultId, const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_reason = Reason::Recognized;
    m_text = text;
}

CSpxRecognitionResult::CSpxRecognitionResult(const wchar_t* resultId, const wchar_t* text, IntermediateResult_Type)
{
    SPX_DBG_TRACE_FUNCTION();

    m_resultId = resultId == nullptr
        ? PAL::CreateGuid()
        : resultId;

    m_reason = Reason::IntermediateResult;
    m_text = text;
}

CSpxRecognitionResult::CSpxRecognitionResult(NoMatch_Type)
{
    SPX_DBG_TRACE_VERBOSE("%s NOMATCH", __FUNCTION__);
    m_reason = Reason::NoMatch;
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


} // CARBON_IMPL_NAMESPACE()
