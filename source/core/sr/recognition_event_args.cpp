#include "stdafx.h"
#include "recognition_event_args.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxRecognitionEventArgs::CSpxRecognitionEventArgs()
{
    m_offset = 0;
}

const std::wstring& CSpxRecognitionEventArgs::GetSessionId()
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() == 0, SPXERR_UNINITIALIZED);
    return m_sessionId;
}

const uint64_t& CSpxRecognitionEventArgs::GetOffset()
{
    return m_offset;
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

void CSpxRecognitionEventArgs::Init(const std::wstring& sessionId, uint64_t offset)
{
    SPX_IFTRUE_THROW_HR(m_sessionId.length() != 0, SPXERR_ALREADY_INITIALIZED);
    
    m_sessionId = sessionId;
    m_offset = offset;
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
