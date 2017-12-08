#include "stdafx.h"
#include "session_event_args.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxSessionEventArgs::CSpxSessionEventArgs(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) :
    m_sessionId(sessionId)
{
}

const std::wstring& CSpxSessionEventArgs::GetSessionId()
{
    return m_sessionId;
}


}; // CARBON_IMPL_NAMESPACE()
