//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// WebSocket.h: A web socket C++ implementation using the Azure C shared library
//

#pragma once

#include <queue>
#include <azure_c_shared_utility_includes.h>
#include "i_web_socket.h"
#include "../usp/dnscache.h"

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    struct TransportPacket;

    class WebSocket : public IWebSocket, public enable_shared_from_this<WebSocket>
    {
    public:
        ~WebSocket();

        static shared_ptr<WebSocket> Create(
            shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const chrono::milliseconds& pollingIntervalMs)
        {
            return shared_ptr<WebSocket>(new WebSocket(threadService, affinity, pollingIntervalMs));
        }

        void Connect(const Impl::HttpEndpointInfo& params) override;
        virtual void Disconnect() override;

        void SendTextData(const string& text) override;
        void SendBinaryData(const uint8_t *data, const size_t size) override;

        virtual WebSocketState GetState() const override { return m_state.load(); }

        Impl::event<WebSocketState, WebSocketState> OnStateChanged;

    protected:
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

        virtual void QueuePacket(unique_ptr<TransportPacket> packet);
        virtual int SendPacket(unique_ptr<TransportPacket> packet);

        virtual void HandleConnected();
        virtual void HandleDisconnected(WebSocketDisconnectReason reason, const string& cause, bool serverRequested);
        virtual void HandleTextData(const string& data);
        virtual void HandleBinaryData(const uint8_t* data, const size_t size);
        virtual void HandleError(WebSocketError reason, int errorCode, const std::string& errorMessage);
        virtual void HandleWebSocketStateChanged(WebSocketState oldState, WebSocketState newState);
        virtual void HandleWebSocketFrameSent(WS_SEND_FRAME_RESULT result, uint8_t msgType, uint8_t wsType);

    private:
        WebSocket(
            shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const chrono::milliseconds& pollingIntervalMs);

        DISABLE_DEFAULT_CTORS(WebSocket);

        static void WorkLoop(weak_ptr<WebSocket> ptr);
        void DoWork();
        
        void DnsComplete(DnsCacheHandle handle, int error, DNS_RESULT_HANDLE result);
        void OnWebSocketOpened(WS_OPEN_RESULT_DETAILED open_result_detailed);
        void OnWebSocketFrameReceived(unsigned char frame_type, const unsigned char* buffer, size_t size);
        void OnWebSocketPeerClosed(uint16_t* closeCode, const unsigned char* extraData, size_t extraDataLength);
        void OnWebSocketError(WS_ERROR errorCode);
        void OnWebSocketClosed();

        uint64_t GetTimestamp() const;

    private:
        using DnsCachePtr = unique_ptr<_DNS_CONTEXT, decltype(&DnsCacheDestroy)>;
        using HeadersPtr = unique_ptr<HTTP_HEADERS_HANDLE_DATA_TAG, decltype(&HTTPHeaders_Free)>;
        struct connection_params;

        shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> m_threadService;
        const Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity m_affinity;
        const chrono::milliseconds m_pollingIntervalMs;
        atomic_bool m_valid;
        uint64_t m_creationTime;
        uint64_t m_connectionTime;
        DnsCachePtr m_dnsCache;

        Impl::HttpEndpointInfo::Internals m_webSocketEndpoint;
        UWS_CLIENT_HANDLE m_WSHandle;
        atomic_bool m_open;
        atomic<WebSocketState> m_state;
        queue<unique_ptr<TransportPacket>> m_queue;
        mutex m_queue_lock;
    };

} } } }
