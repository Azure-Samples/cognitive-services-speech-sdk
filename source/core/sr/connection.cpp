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

void CSpxConnection::Init(std::weak_ptr<ISpxRecognizer> recognizer)
{
    m_recognizer = recognizer;
}

std::shared_ptr<ISpxRecognizer> CSpxConnection::GetRecognizer()
{
    auto shared = m_recognizer.lock();
    if (shared == nullptr)
    {
        SPX_DBG_TRACE_INFO("GetRecognizer from connection: recognizer is no longer valid");
    }
    return shared;
}

void CSpxConnection::Open(bool forContinuousRecognition)
{
    auto shared = GetRecognizer();
    SPX_IFTRUE_THROW_HR(shared == nullptr, SPXERR_INVALID_RECOGNIZER);
    shared->OpenConnection(forContinuousRecognition);
}

void CSpxConnection::Close()
{
    auto shared = GetRecognizer();
    if (shared)
    {
        shared->CloseConnection();
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
