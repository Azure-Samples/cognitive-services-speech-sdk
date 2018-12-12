#include "stdafx.h"
#include "connection.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxConnection::CSpxConnection()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxConnection::~CSpxConnection()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxConnection::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
    m_recognizer = GetSite()->GetRecognizer();
}

void CSpxConnection::Term()
{
    SPX_DBG_TRACE_FUNCTION();
}

std::shared_ptr<ISpxRecognizer> CSpxConnection::GetRecognizer()
{
    return m_recognizer;
}

void CSpxConnection::Open()
{
    // Todo: Add connection setup.

}

} } } } // Microsoft::CognitiveServices::Speech::Impl
