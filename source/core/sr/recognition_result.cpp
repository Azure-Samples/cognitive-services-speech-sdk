#include "stdafx.h"
#include "recognition_result.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognitionResult::CSpxRecognitionResult(const wchar_t* resultId, const wchar_t* text)
{
    SPX_DBG_TRACE_FUNCTION();

    if (resultId == nullptr)
    {
        // TODO: RobCh: Next: Generate a real result id here
        resultId = L"ffff-ffff-ffff-result-id-goes-here-ffff";
    }

    m_reason = Reason::Recognized;
    m_resultId = resultId;
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


}; // CARBON_IMPL_NAMESPACE()
