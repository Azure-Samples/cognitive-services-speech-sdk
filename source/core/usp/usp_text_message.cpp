//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <cstring>
#include <stdexcept>

#include "usp_text_message.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    TextMessage::TextMessage()
        : Message(false, "", USP::MessageType::Unknown, ""), m_data()
    {}

    TextMessage::TextMessage(std::string&& data, const std::string& path, USP::MessageType messageType, const std::string& requestId)
        : Message(false, path, messageType, requestId), m_data(std::move(data))
    {
    }

    TextMessage::TextMessage(const std::string& data, const std::string& path, USP::MessageType messageType, const std::string& requestId)
        : Message(false, path, messageType, requestId), m_data(data)
    {
    }

    TextMessage::TextMessage(std::string&& data, const std::string& path, const std::string& contentType, USP::MessageType messageType, const std::string& requestId)
        : Message(false, path, messageType, requestId), m_data(std::move(data))
    {
        ContentType(contentType);
    }

    TextMessage::TextMessage(const std::string& data, const std::string& path, const std::string& contentType, USP::MessageType messageType, const std::string& requestId)
        : Message(false, path, messageType, requestId), m_data(data)
    {
        ContentType(contentType);
    }

    TextMessage& TextMessage::Data(const std::string & body)
    {
        m_data = body;
        return *this;
    }

    const std::string& TextMessage::Data() const
    {
        return m_data;
    }

    size_t TextMessage::GetDataSize() const
    {
        return m_data.size();
    }

    size_t TextMessage::WriteData(uint8_t* buffer, const size_t bufferSize) const
    {
        size_t bytesToWrite = GetDataSize();
        if (bufferSize < bytesToWrite)
        {
            bytesToWrite = bufferSize;
        }

        std::memcpy(buffer, m_data.c_str(), bytesToWrite);
        return bytesToWrite;
    }

}}}}
