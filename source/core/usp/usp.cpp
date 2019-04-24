//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "usp.h"
#include "uspinternal.h"
#include "uspcommon.h"
#include "audio_chunk.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

ConnectionPtr Client::Connect()
{
    return ConnectionPtr(new Connection(*this));
}

Connection::~Connection()
{
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

void Connection::WriteAudio(const ::Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk)
{
    m_impl->QueueAudioSegment(audioChunk);
}

void Connection::FlushAudio()
{
    m_impl->QueueAudioEnd();
}

void Connection::SendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size, MessageType messageType)
{
    m_impl->QueueMessage(messagePath, buffer, size, messageType);
}

void Connection::WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency)
{
    m_impl->WriteTelemetryLatency(latencyInTicks, isPhraseLatency);
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

}}}}
