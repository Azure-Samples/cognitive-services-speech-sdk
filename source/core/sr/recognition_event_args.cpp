#include "stdafx.h"
#include "recognition_event_args.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxRecognitionEventArgs::CSpxRecognitionEventArgs()
{
}

const std::wstring& CSpxRecognitionEventArgs::GetSessionId()
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() == 0, SPXERR_UNINITIALIZED);
    return m_sessionId;
}

std::shared_ptr<ISpxRecognitionResult> CSpxRecognitionEventArgs::GetResult()
{
    SPX_IFTRUE_THROW_HR(m_result.get() == nullptr, SPXERR_UNINITIALIZED);
    return m_result;
}

void CSpxRecognitionEventArgs::Init(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() != 0, SPXERR_ALREADY_INITIALIZED);
    SPX_IFTRUE_THROW_HR(m_result.get() != nullptr, SPXERR_ALREADY_INITIALIZED);

    m_sessionId = sessionId;
    m_result = result;
}


} // CARBON_IMPL_NAMESPACE
