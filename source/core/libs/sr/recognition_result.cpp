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
    throw nullptr;
}

std::wstring CSpxRecognitionResult::GetText()
{
    throw nullptr;
}


}; // CARBON_IMPL_NAMESPACE()
