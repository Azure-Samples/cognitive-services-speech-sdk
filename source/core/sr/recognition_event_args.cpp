#include "stdafx.h"
#include "recognition_event_args.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognitionEventArgs::CSpxRecognitionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) :
    m_sessionId(sessionId),
    m_result(result)
{
}

const std::wstring& CSpxRecognitionEventArgs::GetSessionId()
{
    return m_sessionId;
}

std::shared_ptr<ISpxRecognitionResult> CSpxRecognitionEventArgs::GetResult()
{
    return m_result;
}    


}; // CARBON_IMPL_NAMESPACE()
