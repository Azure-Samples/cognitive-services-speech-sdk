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
#include <i_telemetry.h>
#include "usp_message.h"
#include "usp_text_message.h"
#include "usp_binary_message.h"
#include <ispxinterfaces.h>

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

        void SendAudioData(const std::string& path, const Impl::DataChunkPtr& audioChunk, const std::string& requestId = "", bool newStream = false);

        void SendTelemetryData(std::string&& data, const std::string& requestId);

        Impl::event<const UspHeaders&, const std::string&> OnUspTextData;
        Impl::event<const UspHeaders&, const uint8_t*, size_t> OnUspBinaryData;

    protected:
        UspWebSocket(std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const std::chrono::milliseconds& pollingIntervalMs,
            ITelemetry& telemetry);

        int SendMessage(std::unique_ptr<IWebSocketMessage> message) override;

        void HandleTextData(const std::string& data) override;
        void HandleBinaryData(const uint8_t* data, const size_t size) override;
        void HandleDisconnected(WebSocketDisconnectReason reason, const std::string& cause) override;

    private:
        DISABLE_DEFAULT_CTORS(UspWebSocket);

        // Silence warnings on Android builds
        using WebSocket::SendTextData;
        using WebSocket::SendBinaryData;

    private:
        std::atomic_bool m_chunkSent;
        std::atomic<std::uint32_t> m_streamId;
    };

} } } }
