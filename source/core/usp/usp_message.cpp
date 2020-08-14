//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <exception>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include <string_utils.h>
#include <time_utils.h>
#include <i_telemetry.h>
#include <azure_c_shared_utility_includes.h>
#include "usp_message.h"
#include "usp_constants.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    using namespace PAL;

    /// <summary>
    /// Helper function to retrieve values from an unordered map without modifying it nor throwing exceptions
    /// </summary>
    /// <param name="map">The map to retrieve values from</param>
    /// <param name="name">The value to retrieve</param>
    /// <returns>The corresponding value, or an empty string if there is none</returns>
    static std::string GetValue(const std::unordered_map<std::string, std::string>& map, const std::string& name)
    {
        auto found = map.find(name);
        if (found == map.end())
        {
            return std::string{};
        }

        return found->second;
    }

    static void SetValue(std::unordered_map<std::string, std::string>& map, const std::string& name, const std::string& value)
    {
        auto trimmed = StringUtils::Trim(value);
        if (trimmed.empty())
        {
            map.erase(name);
        }
        else
        {
            map[name] = value;
        }
    }

    static size_t SerializeHeader(const std::string& name, const std::string& value, char* buffer, const size_t bufferSize)
    {
        size_t offset = 0;
        if (value.empty())
        {
            return offset;
        }

        offset += sprintf_s(buffer + offset, bufferSize - offset, "%s", name.c_str());
        offset += sprintf_s(buffer + offset, bufferSize - offset, ":");
        offset += sprintf_s(buffer + offset, bufferSize - offset, "%s", value.c_str());
        offset += sprintf_s(buffer + offset, bufferSize - offset, "\r\n");

        return offset;
    }



    Message& Message::Path(const std::string& path)
    {
        SetValue(m_headers, Constants::HEADER_PATH, path);
        return *this;
    }

    std::string Message::Path() const
    {
        return GetValue(m_headers, Constants::HEADER_PATH);
    }

    Message& Message::RequestId(const std::string& requestId)
    {
        SetValue(m_headers, Constants::HEADER_REQUEST_ID, requestId);
        return *this;
    }

    std::string Message::RequestId() const
    {
        return GetValue(m_headers, Constants::HEADER_REQUEST_ID);
    }

    Message& Message::Timestamp(const std::string& timestamp)
    {
        std::string trimmed = StringUtils::Trim(timestamp);
        if (trimmed.empty()
            || !PAL::TryParseUtcTimeString(trimmed, Constants::FORMAT_ISO_8601_TIMESTAMP, m_timestamp))
        {
            throw std::invalid_argument{ "Timestamp string does match the expected format" };
        }

        m_headers[Constants::HEADER_TIMESTAMP] = trimmed;
        return *this;
    }

    Message& Message::Timestamp(const std::chrono::system_clock::time_point timestamp)
    {
        m_timestamp = timestamp;
        m_headers[Constants::HEADER_TIMESTAMP] = PAL::GetTimeInString(timestamp, 3);
        return *this;
    }

    std::chrono::system_clock::time_point Message::Timestamp() const
    {
        return m_timestamp;
    }

    std::string Message::UtcTimestampAsString() const
    {
        return GetValue(m_headers, Constants::HEADER_TIMESTAMP);
    }

    Message& Message::ContentType(const std::string& contentType)
    {
        SetValue(m_headers, Constants::HEADER_CONTENT_TYPE, contentType);
        return *this;
    }

    std::string Message::ContentType() const
    {
        return GetValue(m_headers, Constants::HEADER_CONTENT_TYPE);
    }

    Message& Message::SetHeader(const std::string& name, const std::string& value)
    {
        SetValue(m_headers, name, value);
        return *this;
    }

    std::string Message::GetHeader(const std::string& name) const
    {
        return GetValue(m_headers, name);
    }

    bool Message::IsBinary() const
    {
        return m_isBinary;
    }

    bool Message::HasData() const
    {
        return GetDataSize() > 0;
    }

    size_t Message::Size() const
    {
        // Start at 4 bytes because:
        // - [2] We'll always have the \r\n separating the header from the data
        // - [1] Need to leave room for the trailing \0 added by sprintf_s while writing the headers
        // - [1] Need to leave room for a potential trailing \0 while writing the data
        size_t size = 4;

        // Binary requests start with 2 bytes that indicate the size of the header
        if (IsBinary())
        {
            size += Constants::HEADER_SIZE_IN_BYTES;
        }

        // Compute the size of the header. Each header is:
        // <name>:<value>\r\n
        for (const auto& entry : m_headers)
        {
            size += entry.first.size();
            size += entry.second.size();
            size += 3; // ':' separator, and \r\n at the end
        }

        size += GetDataSize();
        return size;
    }

    Message& Message::MessageType(USP::MessageType messageType)
    {
        m_msgType = messageType;
        return *this;
    }

    USP::MessageType Message::MessageType() const
    {
        return m_msgType;
    }

    Message& Message::MetricMessageType(USP::MetricMessageType metricType)
    {
        m_metricType = metricType;
        return *this;
    }

    USP::MetricMessageType Message::MetricMessageType() const
    {
        return m_metricType;
    }

    uint8_t Message::FrameType() const
    {
        return IsBinary()
            ? WS_FRAME_TYPE_BINARY
            : WS_FRAME_TYPE_TEXT;
    }

    size_t Message::Serialize(std::shared_ptr<uint8_t>& buffer)
    {
        size_t offset = 0;

        size_t bufferSize = Size();
        buffer = std::shared_ptr<uint8_t>(new uint8_t[bufferSize], [](uint8_t* p) { delete[] p; });

        // The first two bytes in a binary are the size of the header. Skip over these for now
        // and we'll write them after we're done writing out the headers
        if (IsBinary())
        {
            offset += 2;
        }

        char* ptr = reinterpret_cast<char*>(buffer.get());

        std::string requiredHeaders[] =
        {
            Constants::HEADER_TIMESTAMP,
            Constants::HEADER_PATH,
            Constants::HEADER_CONTENT_TYPE,
            Constants::HEADER_REQUEST_ID
        };

        // for consistency we write out the required headers first in a particular order
        for (const auto& name : requiredHeaders)
        {
            std::string value = GetValue(m_headers, name);
            offset += SerializeHeader(name, value, ptr + offset, bufferSize - offset);
        }

        // write out any remaining headers
        for (auto& header : m_headers)
        {
            if (std::find(std::begin(requiredHeaders), std::end(requiredHeaders), header.first) != std::end(requiredHeaders))
            {
                continue;
            }

            offset += SerializeHeader(header.first, header.second, ptr + offset, bufferSize - offset);
        }

        // if this is a binary message, write the header size in the first two bytes
        if (IsBinary())
        {
            // NOTE: the header size does not include the last \r\n separating the header from the
            //       body so we need to subtract 2 here to account for that
            size_t headerSize = offset - 2;

            for (size_t i = 0; i < Constants::HEADER_SIZE_IN_BYTES; i++)
            {
                size_t shift = 8 * (Constants::HEADER_SIZE_IN_BYTES - i - 1);
                buffer.get()[i] = (uint8_t)((headerSize >> shift) & 0xff);
            }
        }
        else
        {
            // write out the \r\n separating the headers from the body
            offset += sprintf_s(ptr + offset, bufferSize - offset, "\r\n");
        }

        // write out the body
        offset += WriteData(buffer.get() + offset, bufferSize - offset);

        return offset;
    }

    std::future<bool> Message::MessageSent()
    {
        return m_messageSent.get_future();
    }

    void Message::MessageSent(bool success)
    {
        m_messageSent.set_value(success);
    }

    Message::Message(bool isBinary, const std::string& path, USP::MessageType messageType, const std::string& requestId) :
        m_isBinary(isBinary),
        m_timestamp(),
        m_headers(),
        m_msgType(messageType),
        m_metricType(USP::MetricMessageType::METRIC_MESSAGE_TYPE_INVALID),
        m_messageSent()
    {
        Timestamp(std::chrono::system_clock::now());

        std::string trimmed = StringUtils::Trim(path);
        if (!trimmed.empty())
        {
            Path(path);
        }

        trimmed = StringUtils::Trim(requestId);
        if (!trimmed.empty())
        {
            RequestId(trimmed);
        }
    }

}}}}
