//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "usp_message.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    /// <summary>
    /// Represents a USP text message
    /// </summary>
    class TextMessage : public Message
    {
    public:
        /// <summary>
        /// Creates a new instance
        /// </summary>
        TextMessage();

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="data">The message data to take ownership of</param>
        /// <param name="path">The message path</param>
        /// <param name="messageType">The USP message type</param>
        /// <param name="requestId">(Optional) The request ID</param>
        TextMessage(std::string&& data, const std::string& path, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="data">The message data</param>
        /// <param name="path">The message path</param>
        /// <param name="messageType">The USP message type</param>
        /// <param name="requestId">(Optional) The request ID</param>
        TextMessage(const std::string& data, const std::string& path, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="data">The message data to take ownership of</param>
        /// <param name="path">The message path</param>
        /// <param name="contentType">The content type of the data</param>
        /// <param name="messageType">The USP message type</param>
        /// <param name="requestId">(Optional) The request ID</param>
        TextMessage(std::string&& data, const std::string& path, const std::string& contentType, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="data">The message data</param>
        /// <param name="path">The message path</param>
        /// <param name="contentType">The content type of the data</param>
        /// <param name="messageType">The USP message type</param>
        /// <param name="requestId">(Optional) The request ID</param>
        TextMessage(const std::string& data, const std::string& path, const std::string& contentType, USP::MessageType messageType, const std::string& requestId = "");

        /// <summary>
        /// Move constructor
        /// </summary>
        TextMessage(TextMessage&&) = default;

        /// <summary>
        /// Move assignment operator
        /// </summary>
        TextMessage& operator=(TextMessage&&) = default;

        /// <summary>
        /// Sets the text data content
        /// </summary>
        /// <param name="data">The text data</param>
        /// <returns>Reference to this instance</returns>
        TextMessage& Data(const std::string& data);

        /// <summary>
        /// Gets the text data content
        /// </summary>
        /// <returns>The string data content</returns>
        const std::string& Data() const;

    protected:
        virtual size_t GetDataSize() const override;
        virtual size_t WriteData(uint8_t* buffer, const size_t bufferSize) const override;

        std::string m_data;
    };

}}}}
