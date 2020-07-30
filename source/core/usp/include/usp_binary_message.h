//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <memory>
#include <vector>
#include "usp_message.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    /// <summary>
    /// Represents a USP binary message
    /// </summary>
    class BinaryMessage : public Message
    {
    public:
        /// <summary>
        /// Creates a new binary message with no data
        /// </summary>
        BinaryMessage();

        /// <summary>
        /// Creates a new binary message with the specified buffer size
        /// </summary>
        /// <param name="bytes">The number of bytes in the buffer</param>
        /// <param name="messageType">The type of the message</param>
        /// <param name="requestId">(Optional) The request ID to </param>
        BinaryMessage(size_t bytes, const std::string& path, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Creates a new binary message taking ownership of the specified buffer
        /// </summary>
        /// <param name="buffer">The buffer of data</param>
        BinaryMessage(std::shared_ptr<uint8_t> buffer, size_t bufferSize, const std::string& path, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Creates a new binary message taking ownership of the specified buffer
        /// </summary>
        /// <param name="buffer">The buffer of data</param>
        BinaryMessage(std::vector<uint8_t>&& buffer, const std::string& path, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Move constructor
        /// </summary>
        BinaryMessage(BinaryMessage&&) = default;

        /// <summary>
        /// Move assignment operator
        /// </summary>
        BinaryMessage& operator=(BinaryMessage&&) = default;

        /// <summary>
        /// Sets the data for this binary message
        /// </summary>
        /// <param name="buffer">The data buffer to take ownership of</param>
        /// <param name="bufferSize">The size of the data buffer</param>
        /// <returns>Reference to this instance</returns>
        BinaryMessage& Data(std::shared_ptr<uint8_t> buffer, const size_t bufferSize);

        /// <summary>
        /// Gets the pointer to the binary data buffer
        /// </summary>
        /// <returns>Pointer to binary data</returns>
        uint8_t* Data() const;

        /// <summary>
        /// Gets the size of the binary data buffer
        /// </summary>
        /// <returns>Buffer size in bytes</returns>
        size_t DataSize() const;

        /// <summary>
        /// Serializes this USP message
        /// </summary>
        /// <param name="buffer">The buffer instantiate and serialize to</param>
        /// <returns>The number of bytes written</returns>
        size_t Serialize(std::shared_ptr<uint8_t>& buffer) override;

    protected:
        virtual size_t GetDataSize() const override;
        virtual size_t WriteData(uint8_t* buffer, const size_t bufferSize) const override;

        size_t m_dataSize;
        size_t m_dataOffset;
        std::shared_ptr<uint8_t> m_data;
        std::vector<uint8_t> m_dataVector;
    };

}}}}
