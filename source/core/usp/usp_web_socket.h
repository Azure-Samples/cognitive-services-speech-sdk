//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usb_web_socket.h: defines the USP web socket connection.
//

#pragma once

#include <memory>
#include <map>
#include <atomic>
#include <web_socket.h>
#include <audio_chunk.h>
#include <i_telemetry.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    typedef std::unordered_map<std::string, std::string> UspHeaders;

    class UspWebSocket : public WebSocket
    {
    public:
        virtual ~UspWebSocket();

        static std::shared_ptr<UspWebSocket> Create(
            std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const std::chrono::milliseconds& pollingIntervalMs,
            ITelemetry& telemetry);

        std::future<bool> SendData(const std::string& path, const uint8_t* buffer, size_t bufferSize, const std::string& requestId = "", bool binary = false);

        void SendAudioData(const std::string& path, const Impl::DataChunkPtr& audioChunk, const std::string& requestId = "", bool newStream = false);

        void SendTelemetryData(const uint8_t* buffer, size_t bufferSize, const std::string& requestId);

        Impl::event<const UspHeaders&, const std::string&> OnUspTextData;
        Impl::event<const UspHeaders&, const uint8_t*, size_t> OnUspBinaryData;

    protected:
        UspWebSocket(std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const std::chrono::milliseconds& pollingIntervalMs,
            ITelemetry& telemetry);

        int SendPacket(std::unique_ptr<TransportPacket> packet) override;

        void HandleTextData(const std::string& data) override;
        void HandleBinaryData(const uint8_t* data, const size_t size) override;
        void HandleDisconnected(WebSocketDisconnectReason reason, const std::string& cause, bool serverRequested) override;

    private:
        DISABLE_DEFAULT_CTORS(UspWebSocket);

        /// <summary>
        /// Writes out the USP Audio header to the specified buffer
        /// </summary>
        /// <param name="buffer">The buffer to write to</param>
        /// <param name="bufferSize">The maximum size of the buffer</param>
        /// <param name="requestId">The request identifier</param>
        /// <param name="pstTimeStamp">The PST timestamp</param>
        /// <param name="userId">The user ID</param>
        /// <param name="contentType">The audio content type. This can be nullptr</param>
        /// <returns>The number of bytes written, or a value less than 0 in the case of errors</returns>
        int CreateAudioDataHeader(
            char* buffer,
            size_t bufferSize,
            const std::string& requestId,
            const std::string& pstTimeStamp,
            const std::string userId,
            const char * contentType);

        // Silence warnings on Android builds
        using WebSocket::SendTextData;
        using WebSocket::SendBinaryData;

    private:
        std::atomic_bool m_chunkSent;
        std::atomic<std::uint32_t> m_streamId;
    };

} } } }
