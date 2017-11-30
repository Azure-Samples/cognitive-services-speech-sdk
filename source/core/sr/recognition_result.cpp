#include "stdafx.h"
#include "recognition_result.h"


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
    auto hackResultId = std::wstring(L"{7B252810-CDFF-442E-9A7A-483B09019CEF}");
    return hackResultId; // TODO: RobCh: SPXERR_NOT_IMPL
}

std::wstring CSpxRecognitionResult::GetText()
{
    auto hackText = std::wstring(L"This is a test of the emergency broadcast system.");
    return hackText; // TODO: RobCh: SPXERR_NOT_IMPL
}

Reason CSpxRecognitionResult::GetReason()
{
    auto hackReason = Reason::Recognized;
    return hackReason; // TODO: RobCh: SPXERR_NOT_IMPL
}


}; // CARBON_IMPL_NAMESPACE()
