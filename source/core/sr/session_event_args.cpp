#include "stdafx.h"
#include "session_event_args.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxSessionEventArgs::CSpxSessionEventArgs()
{
}

const std::wstring& CSpxSessionEventArgs::GetSessionId()
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() == 0, SPXERR_UNINITIALIZED);
    return m_sessionId;
}

void CSpxSessionEventArgs::Init(const std::wstring& sessionId)
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() != 0, SPXERR_ALREADY_INITIALIZED);
    m_sessionId = sessionId;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
