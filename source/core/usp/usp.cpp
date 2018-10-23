//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "usp.h"
#include "uspinternal.h"

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

}}}}