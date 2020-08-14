//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// WebSocket.h: A web socket C++ implementation using the Azure C shared library
//

#pragma once

#include <queue>
#include <ispxinterfaces.h>
#include <azure_c_shared_utility_includes.h>
#include <i_web_socket.h>
#include <i_telemetry.h>
#include <no_op_telemetry.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    class WebSocket : public IWebSocket, public std::enable_shared_from_this<WebSocket>
    {
    public:
        virtual ~WebSocket();

        static std::shared_ptr<WebSocket> Create(
            std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const std::chrono::milliseconds& pollingIntervalMs)
        {
            return std::shared_ptr<WebSocket>(new WebSocket(threadService, affinity, pollingIntervalMs, NoOpTelemetry::Instance()));
        }

        static std::shared_ptr<WebSocket> Create(
            std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const std::chrono::milliseconds& pollingIntervalMs,
            ITelemetry& telemetry)
        {
            return std::shared_ptr<WebSocket>(new WebSocket(threadService, affinity, pollingIntervalMs, telemetry));
        }

        virtual void Connect(const Impl::HttpEndpointInfo & webSocketEndpoint, const std::string& connectionId) override;
        virtual void Disconnect() override;

        void SendTextData(const std::string& text) override;
        void SendBinaryData(const uint8_t *data, const size_t size) override;
        void SendData(std::unique_ptr<IWebSocketMessage> message) override;

        virtual WebSocketState GetState() const override { return m_state.load(); }

        Impl::event<WebSocketState, WebSocketState> OnStateChanged;

    protected:
        /// <summary>
        /// Parses HTTP headers from the specified buffer. It interprets the first : as the separator between the header
        /// name, and the value
        /// </summary>
        /// <param name="buffer">The buffer of data to parse from</param>
        /// <param name="size">The number of bytes in the header</param>
        /// <param name="headers">The headers map to populate</param>
        /// <returns>How many bytes the header data was. This can be used as an offset into the buffer to determine
        /// where the HTTP payload starts</returns>
        static size_t ParseHttpHeaders(const uint8_t* buffer, size_t size, std::unordered_map<std::string, std::string>& headers);

        WebSocket(
            std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const std::chrono::milliseconds& pollingIntervalMs,
            ITelemetry& telemetry);

        void ChangeState(WebSocketState state)
        {
            WebSocketState previous = m_state.exchange(state);
            if (previous != state)
            {
                HandleWebSocketStateChanged(previous, state);
            }
        }

        void ChangeState(WebSocketState from, WebSocketState next)
        {
            WebSocketState compare = from;
            bool success = m_state.compare_exchange_strong(compare, next);
            if (!success)
            {
                // for debugging
                LogError("Failed to change state for WebSocket %p. From: %d, To: %d, Current: %d",
                    this, from, next, compare);

                // force it anyway
                from = m_state.exchange(next);
            }

            HandleWebSocketStateChanged(from, next);
        }

        virtual int Connect();

        virtual void QueueMessage(std::unique_ptr<IWebSocketMessage> message);
        virtual int SendMessage(std::unique_ptr<IWebSocketMessage> message);

        virtual void HandleConnected();
        virtual void HandleDisconnected(WebSocketDisconnectReason reason, const std::string& cause);
        virtual void HandleTextData(const std::string& data);
        virtual void HandleBinaryData(const uint8_t* data, const size_t size);
        virtual void HandleError(WebSocketError reason, int errorCode, const std::string& errorMessage);
        virtual void HandleWebSocketStateChanged(WebSocketState oldState, WebSocketState newState);
        virtual void HandleWebSocketFrameSent(IWebSocketMessage* message, WS_SEND_FRAME_RESULT result);

    private:
        DISABLE_DEFAULT_CTORS(WebSocket);

        static void WorkLoop(std::weak_ptr<WebSocket> ptr);
        void DoWork();
        
        void OnWebSocketOpened(WS_OPEN_RESULT_DETAILED open_result_detailed);
        void OnWebSocketFrameReceived(unsigned char frame_type, const unsigned char* buffer, size_t size);
        void OnWebSocketPeerClosed(uint16_t* closeCode, const unsigned char* extraData, size_t extraDataLength);
        void OnWebSocketError(WS_ERROR errorCode);
        void OnWebSocketClosed();

        uint64_t GetTimestamp() const;

    protected:
        std::atomic_bool m_valid;
        std::atomic_bool m_open;
        ITelemetry& m_telemetry;
        std::string m_connectionId;

    private:
        std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> m_threadService;
        const Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity m_affinity;
        const std::chrono::milliseconds m_pollingIntervalMs;
        uint64_t m_creationTime;
        uint64_t m_connectionTime;

        Impl::HttpEndpointInfo::Internals m_webSocketEndpoint;
        UWS_CLIENT_HANDLE m_WSHandle;
        std::atomic<WebSocketState> m_state;
        std::queue<std::unique_ptr<IWebSocketMessage>> m_queue;
        std::mutex m_queue_lock;
};

} } } }
