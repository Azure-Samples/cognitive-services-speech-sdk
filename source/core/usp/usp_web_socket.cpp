//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usb_web_socket.cpp: The implementation of the USP web socket connection.
//

#include "stdafx.h"
#include <sstream>
#include <http_utils.h>
#include <time_utils.h>
#include <i_telemetry.h>
#include "usp_web_socket.h"
#include "usp_constants.h"
#include "uspcommon.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    using namespace std::chrono_literals;
    constexpr int WS_MESSAGE_HEADER_SIZE = 2;


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

        auto message = std::make_unique<USP::BinaryMessage>(
            audioChunk->data, bufferSize, path, USP::MessageType::Unknown, requestId);

        std::string contentType = audioChunk->contentType;
        if (audioChunk->isWavHeader && contentType.empty())
        {
            contentType = Constants::CONTENT_TYPE_WAVE;
        }

        // set the rest of the required USP header values
        message->
            Path("audio") // TODO ralphe: Why is this hard coded?
            .SetHeader(Constants::HEADER_STREAM_ID, std::to_string(m_streamId.load()))
            .SetHeader(Constants::HEADER_CONTENT_TYPE, contentType)
            .SetHeader(Constants::HEADER_PRESENTATION_TIMESTAMP, audioChunk->capturedTime)
            .SetHeader(Constants::HEADER_SPEAKER_ID, audioChunk->userId);

        auto metricType = MetricMessageType::METRIC_MESSAGE_TYPE_INVALID;
        if (bufferSize == 0)
        {
            metricType = MetricMessageType::METRIC_MESSAGE_TYPE_AUDIO_LAST;

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

                if (memcmp(message->Data(), "RIFF", 4) && memcmp(message->Data(), "#!SILK", 6))
                {
                    // TODO: Should this throw an exception?
                    return;
                }

                metricType = MetricMessageType::METRIC_MESSAGE_TYPE_AUDIO_START;
            }
        }

        message->MetricMessageType(metricType);

        SendData(std::move(message));
    }

    void UspWebSocket::SendTelemetryData(std::string&& data, const std::string& requestId)
    {
        SendData(
            std::make_unique<USP::TextMessage>(
                data, "telemetry", Constants::CONTENT_TYPE_JSON, USP::MessageType::Unknown, requestId));
    }

    int UspWebSocket::SendMessage(std::unique_ptr<IWebSocketMessage> message)
    {
        // Update the timestamp
        auto uspMessage = dynamic_cast<USP::Message*>(message.get());
        if (uspMessage != nullptr)
        {
            uspMessage->Timestamp(std::chrono::system_clock::now());
        }

        return WebSocket::SendMessage(move(message));
    }

    void UspWebSocket::HandleTextData(const std::string & data)
    {
        UspHeaders headers;
        size_t offset = WebSocket::ParseHttpHeaders(reinterpret_cast<const uint8_t*>(data.c_str()), data.length(), headers);

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

        size_t offset = WebSocket::ParseHttpHeaders(data + WS_MESSAGE_HEADER_SIZE, headerSize, headers);
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
        if (reason == WebSocketDisconnectReason::Normal)
        {
            WebSocket::HandleDisconnected(reason, cause, serverRequested);
        }
        else
        {
            HandleError(WebSocketError::REMOTE_CLOSED, (int)reason, cause);
        }
    }

} } } }
