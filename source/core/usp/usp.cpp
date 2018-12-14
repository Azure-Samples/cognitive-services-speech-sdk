//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "usp.h"
#include "uspinternal.h"
#include "uspcommon.h"

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

void Connection::WriteAudio(const uint8_t* buffer, size_t size)
{
    m_impl->QueueAudioSegment(buffer, size);
}

void Connection::FlushAudio()
{
    m_impl->QueueAudioEnd();
}

void Connection::SendMessage(const std::string& messagePath, const uint8_t* buffer, size_t size, MessageType messageType)
{
    m_impl->QueueMessage(messagePath, buffer, size, messageType);
}

Client& Client::SetProxyServerInfo(const std::string &proxyHost, int proxyPort, const std::string &proxyUsername, const std::string &proxyPassword)
{
#ifdef _MSC_VER
#define strdup _strdup
#endif
    m_proxyServerInfo = std::shared_ptr<ProxyServerInfo>(new ProxyServerInfo{strdup(proxyHost.c_str()), proxyPort, strdup(proxyUsername.c_str()), strdup(proxyPassword.c_str())}, 
    [=](ProxyServerInfo *info) 
    {
        if (info->host)
        {
            free((void*)info->host);
        }
        if (info->password)
        {
            free((void*)info->password);
        }
        if (info->username)
        {
            free((void*)info->username);
        }
        delete info;
    });
    return *this;
}

}}}}
