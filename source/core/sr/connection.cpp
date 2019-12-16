#include "stdafx.h"
#include "connection.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;

CSpxConnection::CSpxConnection()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxConnection::~CSpxConnection()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxConnection::Init(std::weak_ptr<ISpxRecognizer> recognizer, std::weak_ptr<ISpxMessageParamFromUser> setter)
{
    m_recognizer = recognizer;
    m_setMessageParamFromUser = setter;
}

std::shared_ptr<ISpxRecognizer> CSpxConnection::GetRecognizer()
{
    auto shared = m_recognizer.lock();
    if (shared == nullptr)
    {
        SPX_TRACE_WARNING("GetRecognizer from connection: recognizer is no longer valid");
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

void CSpxConnection::SetParameter(std::string&& path, std::string&& name, std::string&& value)
{
    auto shared = m_setMessageParamFromUser.lock();
    if (shared == nullptr)
    {
        ThrowRuntimeError("Could not get ISpxMessageParamFromUser.");
    }
    SPX_IFTRUE_THROW_HR(shared == nullptr, SPXERR_INVALID_RECOGNIZER);
    shared->SetParameter(move(path), move(name), move(value));
}

void CSpxConnection::SendNetworkMessage(std::string&& path, std::string&& payload)
{
    auto shared = m_setMessageParamFromUser.lock();
    if (shared == nullptr)
    {
        ThrowRuntimeError("Could not get ISpxMessageParamFromUser.");
    }
    shared->SendNetworkMessage(move(path), move(payload));
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
