//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usb_web_socket.cpp: The implementation of the USP web socket connection.
//

#include "stdafx.h"
#include <sstream>
#include <http_utils.h>
#include <i_telemetry.h>
#include "usp_web_socket.h"
#include "uspcommon.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    using namespace std::chrono_literals;

    // TODO right now the code to write out the USP header data is split across many different places. This can be
    //      combined into one function using either an std::ostringstream, or iteratively sprintf_s using a "%s:%s\r\n"
    //      format string

    constexpr char g_keywordPTStimeStampName[] = "PTS";
    constexpr char g_keywordSpeakerIdName[] = "SpeakerId";
    constexpr char g_KeywordStreamId[] = "X-StreamId";
    constexpr char g_keywordRequestId[] = "X-RequestId";
    constexpr char g_keywordContentType[] = "Content-Type";
    constexpr char g_messageHeader[] = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n%s:%s\r\n\r\n";
    constexpr char g_messageHeaderSsml[] = "%s:%s\r\nPath:%s\r\nContent-Type:application/ssml+xml\r\n%s:%s\r\n\r\n";
    constexpr char g_messageHeaderWithoutRequestId[] = "%s:%s\r\nPath:%s\r\nContent-Type:application/json\r\n\r\n";

    constexpr char g_wavheaderFormat[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n%s:%s\r\n";
    // this is for audio and video data USP message
    constexpr char g_requestFormat[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
    // compared to g_requestFormat2, has one more field PTStimestamp or userId
    constexpr char g_requestFormat2[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n%s:%s\r\n";
    // compared to g_requestFormat3, has two more fields: PTStimestamp and userId
    constexpr char g_requestFormat3[] = "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n";

    constexpr char g_telemetryHeader[] = "%s:%s\r\nPath: telemetry\r\nContent-Type: application/json; charset=utf-8\r\n%s:%s\r\n\r\n";
    constexpr char g_timeStampHeaderName[] = "X-Timestamp";
    constexpr char g_audioWavName[] = "audio/x-wav";

    constexpr int WS_MESSAGE_HEADER_SIZE = 2;

    static size_t ParseHttpHeaders(UspHeaders& headers, const uint8_t* buffer, size_t size)
    {
        size_t ns, vs, offset;
        bool isDone;

        isDone = false;
        std::string name;
        std::string value;
        
        for (ns = offset = vs = 0; offset < size && !isDone; offset++)
        {
            switch (buffer[offset])
            {
                case ' ':
                    break;
                case ':':
                    // we only care about the first ':' separator, everything else is part of the value.
                    if (name.empty())
                    {
                        name = std::string(reinterpret_cast<const char *>(buffer + ns), offset - ns);
                        vs = offset + 1;
                    }
                    break;
                case '\r':
                    if (!name.empty())
                    {
                        value = std::string(reinterpret_cast<const char *>(buffer + vs), offset - vs);
                        headers[name] = value;

                        name.clear();
                        value.clear();
                    }
                    else
                    {
                        isDone = true;
                    }
                    break;
                case '\n':
                    vs = 0;
                    ns = offset + 1;
                    break;
            }
        }

        // skip the trailing '\n'
        if (isDone)
        {
            offset++;
        }

        return offset;
    }

    size_t GetContentTypeForAudioChunk(const Impl::DataChunkPtr& audioChunk, const char*& contentType)
    {
        size_t contentTypeSize = audioChunk->contentType.size();
        bool needsContentTypeHeader = audioChunk->isWavHeader || contentTypeSize > 0;
        contentType = nullptr;

        if (needsContentTypeHeader)
        {
            if (contentTypeSize == 0)
            {
                contentTypeSize = sizeof(g_audioWavName);
                contentType = g_audioWavName;
            }
            else
            {
                contentType = audioChunk->contentType.c_str();
                contentTypeSize++;
            }
        }
        return contentTypeSize;
    }

    UspWebSocket::UspWebSocket(std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
                               Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
                               const std::chrono::milliseconds& pollingIntervalMs,
                               ITelemetry& telemetry) :
        WebSocket(threadService, affinity, pollingIntervalMs, telemetry),
        m_chunkSent(false),
        m_streamId(0)
    {}

    UspWebSocket::~UspWebSocket()
    {
    }

    std::shared_ptr<UspWebSocket> UspWebSocket::Create(
        std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
        Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
        const std::chrono::milliseconds & pollingIntervalMs,
        ITelemetry& telemetry)
    {
        return std::shared_ptr<UspWebSocket>(new UspWebSocket(threadService, affinity, pollingIntervalMs, telemetry));
    }

    void UspWebSocket::SendData(const std::string & path, const uint8_t * buffer, size_t bufferSize, const std::string & requestId, bool binary)
    {
        bool includeRequestId = !requestId.empty();

        size_t payloadSize = sizeof(g_messageHeader) +
            path.size() +
            (includeRequestId ? sizeof(g_keywordRequestId) : 0) +
            (includeRequestId ? NO_DASH_UUID_LEN : 0) +
            sizeof(g_timeStampHeaderName) +
            TIME_STRING_MAX_SIZE +
            bufferSize;

        auto msg = std::make_unique<TransportPacket>(
            static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_DEVICECONTEXT),
            static_cast<unsigned char>(binary ? WS_FRAME_TYPE_BINARY : WS_FRAME_TYPE_TEXT),
            payloadSize);

        char timeString[TIME_STRING_MAX_SIZE];
        int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        if (timeStringLen < 0)
        {
            Impl::ThrowRuntimeError("Failed to get the ISO 8601 timestamp");
        }

        // add headers
        if (includeRequestId)
        {
            if (0 == strcmp(path.data(), "ssml"))
            {
                msg->length = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
                    payloadSize,
                    g_messageHeaderSsml,
                    g_timeStampHeaderName,
                    timeString,
                    path.c_str(),
                    g_keywordRequestId,
                    requestId.c_str());
            }
            else
            {
                msg->length = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
                    payloadSize,
                    g_messageHeader,
                    g_timeStampHeaderName,
                    timeString,
                    path.c_str(),
                    g_keywordRequestId,
                    requestId.c_str());
            }
        }
        else
        {
            msg->length = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
                payloadSize,
                g_messageHeaderWithoutRequestId,
                g_timeStampHeaderName,
                timeString,
                path.c_str());
        }

        // add body
        memcpy(msg->buffer.get() + msg->length, buffer, bufferSize);
        msg->length += bufferSize;

        QueuePacket(std::move(msg));
    }

    void UspWebSocket::SendAudioData(const std::string & path, const Impl::DataChunkPtr & audioChunk, const std::string & requestId, bool newStream)
    {
        if (requestId.empty())
        {
            Impl::ThrowInvalidArgumentException("requestId is empty.");
        }

        if (newStream)
        {
            m_streamId++;
        }

        size_t bufferSize = (size_t)audioChunk->size;
        auto buffer = audioChunk->data.get();

        uint8_t msgtype = METRIC_MESSAGE_TYPE_INVALID;
        if (bufferSize == 0)
        {
            msgtype = static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_AUDIO_LAST);

            bool previousWasChunkSent = m_chunkSent.exchange(false);
            if (!previousWasChunkSent)
            {
                return;
            }
        }
        else
        {
            bool previousWasChunkSent = m_chunkSent.exchange(true);
            if (!previousWasChunkSent)
            {
                if (bufferSize < 6)
                {
                    Impl::ThrowInvalidArgumentException("Bad payload");
                }

                if (memcmp(buffer, "RIFF", 4) && memcmp(buffer, "#!SILK", 6))
                {
                    // TODO: Should this throw an exception?
                    return;
                }

                msgtype = static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_AUDIO_START);
            }
        }

        std::string pstTimeStamp = audioChunk->capturedTime;
        std::string userId = audioChunk->userId;

        const char* contentType = nullptr;
        size_t contentTypeSize = GetContentTypeForAudioChunk(audioChunk, contentType);

        size_t payloadSize = sizeof(g_requestFormat3) +
            path.size() +
            sizeof(g_KeywordStreamId) +
            30 +
            sizeof(g_keywordRequestId) +
            sizeof(requestId) +
            sizeof(g_timeStampHeaderName) +
            TIME_STRING_MAX_SIZE +
            sizeof(g_keywordContentType) +
            contentTypeSize +
            pstTimeStamp.length() +
            userId.length() +
            bufferSize +
            WS_MESSAGE_HEADER_SIZE;

        auto msg = std::make_unique<TransportPacket>(msgtype, static_cast<unsigned char>(WS_FRAME_TYPE_BINARY), payloadSize);

        // fill the msg->buffer with the header content
        auto headerLen = CreateAudioDataHeader(
            reinterpret_cast<char *>(msg->buffer.get()) + WS_MESSAGE_HEADER_SIZE,
            payloadSize - WS_MESSAGE_HEADER_SIZE,
            requestId,
            pstTimeStamp,
            userId,
            contentType);

        if (headerLen < 0)
        {
            Impl::ThrowRuntimeError("Failed to create the USP data header");
        }

        // two bytes length
        msg->buffer[0] = (uint8_t)((headerLen >> 8) & 0xff);
        msg->buffer[1] = (uint8_t)((headerLen >> 0) & 0xff);
        msg->length = headerLen + WS_MESSAGE_HEADER_SIZE;

        // body
        std::memcpy(msg->buffer.get() + msg->length, buffer, bufferSize);
        msg->length += bufferSize;

        QueuePacket(std::move(msg));
    }

    void UspWebSocket::SendTelemetryData(const uint8_t * buffer, size_t bufferSize, const std::string& requestId)
    {
        // serialize headers.
        size_t headerLen;

        size_t payloadSize = sizeof(g_telemetryHeader) +
            sizeof(g_keywordRequestId) +
            NO_DASH_UUID_LEN + // size of requestId
            sizeof(g_timeStampHeaderName) +
            TIME_STRING_MAX_SIZE +
            bufferSize;

        auto msg = std::make_unique<TransportPacket>(static_cast<uint8_t>(MetricMessageType::METRIC_MESSAGE_TYPE_TELEMETRY), static_cast<unsigned char>(WS_FRAME_TYPE_TEXT), payloadSize);

        char timeString[TIME_STRING_MAX_SIZE];
        int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        if (timeStringLen < 0)
        {
            Impl::ThrowRuntimeError("There was a problem getting time string");
        }

        // serialize the entire telemetry message.
        headerLen = sprintf_s(reinterpret_cast<char*>(msg->buffer.get()),
            payloadSize,
            g_telemetryHeader,
            g_timeStampHeaderName,
            timeString,
            g_keywordRequestId,
            requestId.c_str());

        msg->length = headerLen;

        // body
        memcpy(msg->buffer.get() + msg->length, buffer, bufferSize);
        msg->length += bufferSize;

        // finally queue for sending
        QueuePacket(move(msg));
    }

    int UspWebSocket::SendPacket(std::unique_ptr<TransportPacket> packet)
    {
        // re-stamp X-timestamp header value
        char timeString[TIME_STRING_MAX_SIZE] = "";
        int timeStringLength = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        int offset = sizeof(g_timeStampHeaderName);
        if (packet->wstype == WS_FRAME_TYPE_BINARY)
        {
            // Include the first 2 bytes for header length
            offset += 2;
        }
        memcpy(packet->buffer.get() + offset, timeString, timeStringLength);

        return WebSocket::SendPacket(move(packet));
    }

    void UspWebSocket::HandleTextData(const std::string & data)
    {
        UspHeaders headers;
        size_t offset = ParseHttpHeaders(headers, reinterpret_cast<const uint8_t*>(data.c_str()), data.length());

        if (offset == 0 || headers.size() == 0)
        {
            PROTOCOL_VIOLATION("Unable to parse response headers%s", "");
            MetricsTransportParsingError();
            return;
        }

        std::string body(data, offset, data.length() - offset);
        OnUspTextData(headers, body);

        // send original events too
        WebSocket::HandleTextData(body);
    }

    void UspWebSocket::HandleBinaryData(const uint8_t* data, const size_t size)
    {
        if (size < WS_MESSAGE_HEADER_SIZE)
        {
            PROTOCOL_VIOLATION("unable to read binary message length%s", "");
            return;
        }

        UspHeaders headers;
        uint16_t headerSize = 0;
        headerSize = (uint16_t)(data[0] << 8);
        headerSize |= (uint16_t)(data[1] << 0);

        size_t offset = ParseHttpHeaders(headers, data + WS_MESSAGE_HEADER_SIZE, headerSize);
        if (offset > 0)
        {
            offset += WS_MESSAGE_HEADER_SIZE;
        }

        if (offset == 0 || headers.size() == 0)
        {
            PROTOCOL_VIOLATION("Unable to parse response headers%s", "");
            MetricsTransportParsingError();
            return;
        }

        OnUspBinaryData(headers, data + offset, size - offset);

        // send the original events too
        WebSocket::HandleBinaryData(data + offset, size - offset);
    }

    void UspWebSocket::HandleDisconnected(WebSocketDisconnectReason reason, const std::string & cause, bool serverRequested)
    {
        if (serverRequested)
        {
            // For compatibility with what the old transport.cpp code did, raise this as an error instead of a disconnection
            HandleError(WebSocketError::REMOTE_CLOSED, (int)reason, cause);
        }
        else
        {
            WebSocket::HandleDisconnected(reason, cause, serverRequested);
        }
    }

    int UspWebSocket::CreateAudioDataHeader(char * buffer, size_t bufferSize, const std::string & requestId, const std::string & pstTimeStamp, const std::string userId, const char * contentType)
    {
        bool addContentTypeHeader = contentType != nullptr;

        char timeString[TIME_STRING_MAX_SIZE];
        int timeStringLen = GetISO8601Time(timeString, TIME_STRING_MAX_SIZE);
        if (timeStringLen < 0)
        {
            return -1;
        }

        uint32_t streamId = m_streamId.load();

        // TODO: this code is unnecessarily difficult to maintain. We are essentially writing out "%s:%s\r\n" entries
        //       and can simplify this logic by building up the headers iteratively rather than trying to cover all
        //       possible combinations of variables as is done here.

        size_t headerLen;
        std::string path = "audio";
        if (addContentTypeHeader)
        {
            headerLen = sprintf_s(buffer,
                bufferSize,
                g_wavheaderFormat,
                g_timeStampHeaderName,
                timeString,
                path.c_str(),
                g_KeywordStreamId,
                streamId,
                g_keywordRequestId,
                requestId.c_str(),
                g_keywordContentType,
                contentType);
        }
        else if (pstTimeStamp.empty() && userId.empty())
        {
            headerLen = sprintf_s(buffer,
                bufferSize,
                g_requestFormat,  // "%s:%s\r\nPath:%s\r\n%s:%d\r\n%s:%s\r\n";
                g_timeStampHeaderName,  //"X-Timestamp";
                timeString,
                path.c_str(),
                g_KeywordStreamId,  //"X-StreamId";
                streamId,  // %d, streamId is uint32_t
                g_keywordRequestId, //"X-RequestId";
                requestId.c_str());
        }
        else if (!pstTimeStamp.empty() && userId.empty())
        {
            headerLen = sprintf_s(buffer,
                bufferSize,
                g_requestFormat2,
                g_timeStampHeaderName,
                timeString,
                path.c_str(),
                g_KeywordStreamId,
                streamId,
                g_keywordRequestId,
                requestId.c_str(),
                g_keywordPTStimeStampName,
                pstTimeStamp.c_str());
        }
        else if (pstTimeStamp.empty() && !userId.empty())
        {
            headerLen = sprintf_s(buffer,
                bufferSize,
                g_requestFormat2,
                g_timeStampHeaderName,
                timeString,
                path.c_str(),
                g_KeywordStreamId,
                streamId,
                g_keywordRequestId,
                requestId.c_str(),
                g_keywordSpeakerIdName,
                userId.c_str());
        }
        // both are there
        else
        {
            headerLen = sprintf_s(buffer,
                bufferSize,
                g_requestFormat3,
                g_timeStampHeaderName,
                timeString,
                path.c_str(),
                g_KeywordStreamId,
                streamId,
                g_keywordRequestId,
                requestId.c_str(),
                g_keywordSpeakerIdName,
                userId.c_str(),
                g_keywordPTStimeStampName,
                pstTimeStamp.c_str());
        }

        return (int)headerLen;
    }

} } } }
