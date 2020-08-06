//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "usp.h"
#include "uspinternal.h"
#include "uspcommon.h"
#include <ispxinterfaces.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

using namespace std;

Connection::~Connection()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    m_impl->Shutdown();
}

Connection::Connection(const Client& config) :
    m_impl(new Impl { config })
{
    m_impl->Connect();
}

bool Connection::IsConnected()
{
    return m_impl->IsConnected();
}

std::string Connection::GetConnectionUrl()
{
    return m_impl->GetConnectionUrl();
}

void Connection::WriteAudio(const ::Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk)
{
    m_impl->QueueAudioSegment(audioChunk);
}

void Connection::FlushAudio()
{
    m_impl->QueueAudioEnd();
}

void Connection::SendMessage(std::unique_ptr<USP::Message> message)
{
    m_impl->QueueMessage(std::move(message));
}

void Connection::WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency)
{
    m_impl->WriteTelemetryLatency(latencyInTicks, isPhraseLatency);
}


ConnectionPtr Client::Connect()
{
    return ConnectionPtr(new Connection(*this));
}

Client& Client::SetProxyServerInfo(const char *proxyHost, int proxyPort, const char *proxyUsername, const char *proxyPassword)
{
    m_proxyServerInfo = std::make_shared<ProxyServerInfo>();
    if (proxyHost)
    {
        m_proxyServerInfo->host = proxyHost;
    }
    m_proxyServerInfo->port = proxyPort;
    if (proxyUsername)
    {
        m_proxyServerInfo->username = proxyUsername;
    }
    if (proxyPassword)
    {
        m_proxyServerInfo->password = proxyPassword;
    }
    return *this;
}

Client& Client::SetQueryParameter(const std::string& name, const std::string& value)
{
    m_queryParameters[name] = value;
    return *this;
}

}}}}
