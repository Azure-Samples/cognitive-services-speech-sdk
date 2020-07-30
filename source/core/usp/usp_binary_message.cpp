//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <cstring>
#include <stdexcept>

#include "usp_binary_message.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    BinaryMessage::BinaryMessage()
        : Message(true, "", USP::MessageType::Unknown, ""), m_dataSize(0), m_dataOffset(0), m_data(), m_dataVector()
    {}

    BinaryMessage::BinaryMessage(size_t bytes, const std::string& path, USP::MessageType messageType, const std::string& requestId)
        : Message(true, path, messageType, requestId),
        m_dataSize(bytes),
        m_dataOffset(0),
        m_data(new uint8_t[bytes], [](uint8_t* p) { delete[] p; }),
        m_dataVector()
    {
    }

    BinaryMessage::BinaryMessage(std::shared_ptr<uint8_t> buffer, size_t bufferSize, const std::string& path, USP::MessageType messageType, const std::string& requestId)
        : Message(true, path, messageType, requestId), m_dataSize(bufferSize), m_dataOffset(0), m_data(buffer), m_dataVector()
    {}

    BinaryMessage::BinaryMessage(std::vector<uint8_t> && buffer, const std::string& path, USP::MessageType messageType, const std::string& requestId)
        : Message(true, path, messageType, requestId), m_dataSize(buffer.size()), m_dataOffset(0), m_data(), m_dataVector(std::move(buffer))
    {}

    BinaryMessage& BinaryMessage::Data(std::shared_ptr<uint8_t> buffer, const size_t bufferSize)
    {
        m_dataSize = bufferSize;
        m_dataOffset = 0;
        m_data = std::move(buffer);
        m_dataVector.clear();

        return *this;
    }

    uint8_t* BinaryMessage::Data() const
    {
        if (!m_dataVector.empty())
        {
            throw std::logic_error("Cannot retrieve a non-const pointer to your vector data");
        }

        return m_data.get() + m_dataOffset;
    }

    size_t BinaryMessage::DataSize() const
    {
        return m_dataSize;
    }

    size_t BinaryMessage::Serialize(std::shared_ptr<uint8_t>& buffer)
    {
        // to avoid having an unnecessary duplicate after serialization, update the buffer to be
        // the fully serialized message and use the offset into the message to return the Data()
        // pointer afterwards
        size_t totalBytes = USP::Message::Serialize(buffer);

        m_dataOffset = totalBytes - m_dataSize;
        m_data = buffer;

        return totalBytes;
    }

    size_t BinaryMessage::GetDataSize() const
    {
        return m_dataSize;
    }

    size_t BinaryMessage::WriteData(uint8_t* buffer, const size_t bufferSize) const
    {
        size_t bytesToWrite = GetDataSize();
        if (bufferSize < bytesToWrite)
        {
            bytesToWrite = bufferSize;
        }

        std::memcpy(buffer, Data(), bytesToWrite);
        return bytesToWrite;
    }

}}}}
