//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// WebSocket.cpp: A web socket C++ implementation using the Azure C shared library
//

#include "stdafx.h"
#include <azure_c_shared_utility_includes.h>
#include <azure_c_shared_utility_macro_utils.h>
#include <proxy_server_info.h>
#include <web_socket.h>
#include <exception.h>
#include <string_utils.h>
#include <error_info.h>

DEFINE_ENUM_STRINGS(WS_OPEN_RESULT, WS_OPEN_RESULT_VALUES)
DEFINE_ENUM_STRINGS(WS_ERROR, WS_ERROR_VALUES)

// uncomment the line below to see all non-binary protocol messages in the log
// #define LOG_TEXT_MESSAGES

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    // The maximum number of send requests to process per cycle in the background web socket thread.
    // This prevents us blocking for a long time if the rate of send requests is too high
    constexpr size_t MAX_SEND_PER_CYCLE = 20;

    struct WebSocketMessage : public IWebSocketMessage
    {
        inline WebSocketMessage(size_t size, uint8_t frameType, USP::MetricMessageType metricType = USP::MetricMessageType::METRIC_MESSAGE_TYPE_INVALID)
            : m_metricType(metricType),
            m_frameType(frameType),
            m_size(size),
            m_buffer(new uint8_t[size], [](uint8_t* p) { delete[] p; }),
            m_messageSent()
        {
        }

        inline WebSocketMessage(const std::string& data)
            : WebSocketMessage(data.length(), WS_FRAME_TYPE_TEXT)
        {
            // CAUTION: copied string will **NOT** be null terminated
            memcpy(m_buffer.get(), data.c_str(), m_size);
        }

        inline WebSocketMessage(const uint8_t* data, const size_t size)
            : WebSocketMessage(size, WS_FRAME_TYPE_BINARY)
        {
            // we are sending on another thread. This means that we cannot guarantee that the pointer
            // will still be valid at the time we send so we create a copy here to be safe
            memcpy(m_buffer.get(), data, size);
        }

        USP::MetricMessageType MetricMessageType() const override { return m_metricType; }
        uint8_t FrameType() const override { return m_frameType; }
        size_t Size() const override { return m_size; }
        std::future<bool> MessageSent() override { return m_messageSent.get_future(); }
        void MessageSent(bool success) override { m_messageSent.set_value(success); }

        size_t Serialize(std::shared_ptr<uint8_t>& buffer) override
        {
            buffer = m_buffer;
            return m_size;
        }

    private:
        USP::MetricMessageType m_metricType;
        uint8_t m_frameType;
        size_t m_size;
        std::shared_ptr<uint8_t> m_buffer;
        std::promise<bool> m_messageSent;
    };

    struct BoundMessage
    {
        BoundMessage(unique_ptr<IWebSocketMessage> message, weak_ptr<WebSocket> instance)
            : message(std::move(message)), instance(instance)
        {
        }

        std::unique_ptr<IWebSocketMessage> message;
        std::weak_ptr<WebSocket> instance;
        std::shared_ptr<uint8_t> buffer;
    };

    static uint64_t get_epoch_time()
    {
        auto now = chrono::high_resolution_clock::now();
        auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
        return ms.count();
    }

    template<typename ...Args>
    static inline void cast_and_invoke(void *context, void (WebSocket::* callback)(Args...), Args... args)
    {
        auto webSocket = static_cast<WebSocket*>(context);
        if (webSocket != nullptr)
        {
            (webSocket->*callback)(args...);
        }
    }

    size_t WebSocket::ParseHttpHeaders(const uint8_t* buffer, size_t size, std::unordered_map<std::string, std::string>& headers)
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
                    name = std::string(reinterpret_cast<const char*>(buffer + ns), offset - ns);
                    vs = offset + 1;
                }
                break;
            case '\r':
                if (!name.empty())
                {
                    value = std::string(reinterpret_cast<const char*>(buffer + vs), offset - vs);
                    headers[PAL::StringUtils::TrimEnd(name)] = PAL::StringUtils::TrimStart(value);

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

    WebSocket::WebSocket(
            shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService::Affinity affinity,
            const chrono::milliseconds& pollingIntervalMs,
            ITelemetry& telemetry) :
        m_valid(false),
        m_open(false),
        m_telemetry(telemetry),
        m_connectionId(),
        m_threadService(threadService),
        m_affinity(affinity),
        m_pollingIntervalMs(pollingIntervalMs),
        m_creationTime(get_epoch_time()),
        m_connectionTime(),
        m_webSocketEndpoint(),
        m_WSHandle(),
        m_state(WebSocketState::CLOSED),
        m_queue(),
        m_queue_lock()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    WebSocket::~WebSocket()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        // stop background work if it is still running
        m_valid = false;

        // if we have not disconnected, do so now
        if (m_open)
        {
            Disconnect();
        }

        m_open = false;

        m_threadService.reset();

        if (m_WSHandle)
        {
            uws_client_destroy(m_WSHandle);
        }

        queue<unique_ptr<IWebSocketMessage>> empty;
        swap(m_queue, empty);
    }

    void WebSocket::Connect(const Impl::HttpEndpointInfo & endpoint, const std::string& connectionId)
    {
        if (m_open)
        {
            Impl::ThrowLogicError("Web socket is already connected.");
        }

        if (!m_connectionId.empty() && NO_DASH_UUID_LEN < connectionId.size() + 1)
        {
            Impl::ThrowInvalidArgumentException("Invalid size of connection Id. Please use a valid GUID with dashes removed.");
        }

        m_connectionId = connectionId;

        // copy and validate the connection parameters. This is needed to keep valid c_strings pointers in memory
        m_webSocketEndpoint = endpoint.GetInternals();

        if (endpoint.Scheme() != Impl::UriScheme::WS && endpoint.Scheme() != Impl::UriScheme::WSS)
        {
            Impl::ThrowInvalidArgumentException("You must specify a WS or WSS scheme for the endpoint");
        }

        WS_PROTOCOL wsProto;
        wsProto.protocol = m_webSocketEndpoint.webSocketProtcols.c_str();
        int protocolCount = static_cast<int>(m_webSocketEndpoint.webSocketProtocolCount);

        // create the underlying web socket handle
        if (!m_webSocketEndpoint.proxy.host.empty())
        {
            HTTP_PROXY_IO_CONFIG proxyConfig;
            proxyConfig.hostname = m_webSocketEndpoint.host.c_str();
            proxyConfig.port = m_webSocketEndpoint.port;
            proxyConfig.proxy_hostname = m_webSocketEndpoint.proxy.host.c_str();
            proxyConfig.proxy_port = m_webSocketEndpoint.proxy.port;
            proxyConfig.username = m_webSocketEndpoint.proxy.username.c_str();
            proxyConfig.password = m_webSocketEndpoint.proxy.password.c_str();

            void* underlyingIOParameters = &proxyConfig;
            const IO_INTERFACE_DESCRIPTION* underlyingIOInterface = http_proxy_io_get_interface_description();
            if (underlyingIOInterface == nullptr)
            {
                Impl::ThrowRuntimeError("NULL proxy interface description");
            }

            TLSIO_CONFIG tlsioConfig;
            if (m_webSocketEndpoint.isSecure())
            {
                tlsioConfig.hostname = proxyConfig.hostname;
                tlsioConfig.port = proxyConfig.port;
                tlsioConfig.underlying_io_interface = underlyingIOInterface;
                tlsioConfig.underlying_io_parameters = underlyingIOParameters;

                underlyingIOParameters = &tlsioConfig;
                underlyingIOInterface = platform_get_default_tlsio();
                if (underlyingIOInterface == nullptr)
                {
                    Impl::ThrowRuntimeError("NULL TLSIO interface description");
                }
            }

            m_WSHandle = uws_client_create_with_io(
                underlyingIOInterface,
                underlyingIOParameters,
                m_webSocketEndpoint.host.c_str(),
                m_webSocketEndpoint.port,
                m_webSocketEndpoint.pathAndQuery.c_str(),
                protocolCount > 0 ? &wsProto : nullptr,
                protocolCount);
        }
        else
        {
            m_WSHandle = uws_client_create(
                m_webSocketEndpoint.host.c_str(),
                m_webSocketEndpoint.port,
                m_webSocketEndpoint.pathAndQuery.c_str(),
                m_webSocketEndpoint.isSecure(),
                protocolCount > 0 ? &wsProto : nullptr,
                protocolCount);
        }

        if (m_WSHandle == nullptr)
        {
            Impl::ThrowRuntimeError("Failed to create the web socket");
            return;
        }

        // set the headers
        for (const auto& entry : m_webSocketEndpoint.headers)
        {
            int ret = uws_client_set_request_header(m_WSHandle, entry.first.c_str(), entry.second.c_str());
            if (ret)
            {
                Impl::ThrowRuntimeError(std::string("Failed to set the web socket request header. Error: ") + std::to_string(ret));
            }
        }

#ifdef SPEECHSDK_USE_OPENSSL
        if (m_webSocketEndpoint.isSecure())
        {
            int tls_version = OPTION_TLS_VERSION_1_2;
            if (uws_client_set_option(m_WSHandle, OPTION_TLS_VERSION, &tls_version) != HTTPAPI_OK)
            {
                throw std::runtime_error("Could not set TLS 1.2 option");
            }

            uws_client_set_option(m_WSHandle, OPTION_DISABLE_DEFAULT_VERIFY_PATHS, &(m_webSocketEndpoint.disableDefaultVerifyPaths));
            if (!m_webSocketEndpoint.singleTrustedCert.empty())
            {
                uws_client_set_option(m_WSHandle, OPTION_TRUSTED_CERT, m_webSocketEndpoint.singleTrustedCert.c_str());
                uws_client_set_option(m_WSHandle, OPTION_DISABLE_CRL_CHECK, &(m_webSocketEndpoint.disableCrlChecks));
            }
        }
#endif

        ChangeState(WebSocketState::CLOSED, WebSocketState::INITIAL);

        m_valid = true;
        WorkLoop(shared_from_this());
    }

    static void PumpWebSocketInBackground(UWS_CLIENT_HANDLE handle)
    {
        // Helper method to pump the Azure C shared library web socket code on a background thread. This
        // prevents situations where disconnecting can trigger event callbacks on the same thread as
        // disconnect was called on. This is problematic if you are using the thread service ExecuteSync
        // in other code.
        auto ignored = std::async(std::launch::async, [handle]()
        {
            uws_client_dowork(handle);
        });

        // NOTE: the destructor of the async future will block here until the call to uws_client_dowork completes
    }

    void WebSocket::Disconnect()
    {
        static const chrono::milliseconds SLEEP_INTERVAL = 10ms;
        static const int MAX_WAIT_RETRIES = 100;

        auto state = m_state.load();
        switch (state)
        {
            case WebSocketState::DESTROYING:
                // wait for socket to close
                for (size_t i = 0; i < MAX_WAIT_RETRIES; i++)
                {
                    if (!m_open)
                    {
                        break;
                    }

                    this_thread::sleep_for(SLEEP_INTERVAL);
                }
                return;
            case WebSocketState::CLOSED:
                // nothing to do
                return;

            default:
                // fall through
                break;
        }

        ChangeState(WebSocketState::DESTROYING);

        // stop background thread
        m_valid = false;

        if (m_WSHandle != nullptr)
        {
            if (m_open)
            {
                // starts the close handshake.
                LogInfo("%s: start the close handshake.", __FUNCTION__);

                int result = uws_client_close_handshake_async(
                    m_WSHandle,
                    static_cast<uint16_t>(WebSocketDisconnectReason::Normal),
                    "" /* not used */,
                    [](void *ctxt) { cast_and_invoke(ctxt, &WebSocket::OnWebSocketClosed); },
                    this);
                if (result == 0)
                {
                    // first wait for the poll period to give the background worker a chance to terminate
                    std::this_thread::sleep_for(m_pollingIntervalMs);

                    // waits for close.
                    int retries = 0;
                    while (m_open && retries++ < MAX_WAIT_RETRIES)
                    {
                        LogInfo("%s: Continue to pump while waiting for close frame response (%d/%d).", __FUNCTION__, retries, MAX_WAIT_RETRIES);
                        PumpWebSocketInBackground(m_WSHandle);
                        std::this_thread::sleep_for(SLEEP_INTERVAL);
                    }

                    LogInfo("%s: retries %d. isOpen: %s", __FUNCTION__, retries, m_open ? "true" : "false");
                }

                if (m_open)
                {
                    LogError("%s: force close websocket. (result=%d)", __FUNCTION__, result);
                    // force to close.
                    (void)uws_client_close_async(
                        m_WSHandle,
                        [](void *ctxt) { cast_and_invoke(ctxt, &WebSocket::OnWebSocketClosed); },
                        this);

                    // wait for force close.
                    while (m_open)
                    {
                        PumpWebSocketInBackground(m_WSHandle);
                        std::this_thread::sleep_for(SLEEP_INTERVAL);
                    }

                    LogInfo("%s: isOpen: %s", __FUNCTION__, m_open ? "true" : "false");
                }
            }

            LogInfo("%s: destroying uwsclient.", __FUNCTION__);
            uws_client_destroy(m_WSHandle);
            m_WSHandle = nullptr;
        }

        MetricsTransportClosed();
    }

    void WebSocket::SendTextData(const string & text)
    {
        if (text.empty())
        {
            return;
        }

        QueueMessage(std::make_unique<WebSocketMessage>(text));
    }

    void WebSocket::SendBinaryData(const uint8_t * data, const size_t size)
    {
        if (data == nullptr)
        {
            return;
        }

        QueueMessage(std::make_unique<WebSocketMessage>(data, size));
    }

    void WebSocket::SendData(unique_ptr<IWebSocketMessage> message)
    {
        if (message == nullptr)
        {
            return;
        }

        QueueMessage(move(message));
    }

    int WebSocket::Connect()
    {
        if (m_open)
        {
            return 0;
        }
        else if (nullptr == m_WSHandle)
        {
            return -1;
        }
        else
        {
            LogInfo(R"(Start to open websocket. WebSocket: 0x%x, wsio handle: 0x%x)", this, m_WSHandle);

            MetricsTransportStart(m_telemetry, m_connectionId);

            const int result = uws_client_open_async(
                m_WSHandle,
                [](void *ctxt, WS_OPEN_RESULT_DETAILED result)
                {
                    cast_and_invoke(ctxt, &WebSocket::OnWebSocketOpened, result);
                }, this,
                [](void *ctxt, unsigned char frame_type, const unsigned char* buffer, size_t size)
                {
                    cast_and_invoke(ctxt, &WebSocket::OnWebSocketFrameReceived, frame_type, buffer, size);
                }, this,
                [](void *ctxt, uint16_t* closeCode, const unsigned char* extraData, size_t extraDataLength)
                {
                    cast_and_invoke(ctxt, &WebSocket::OnWebSocketPeerClosed, closeCode, extraData, extraDataLength);
                }, this,
                [](void *ctxt, WS_ERROR errorCode)
                {
                    cast_and_invoke(ctxt, &WebSocket::OnWebSocketError, errorCode);
                }, this);

            if (result)
            {
                LogError("uws_client_open_async failed with result %d", result);
            }

            return result;
        }
    }

    void WebSocket::QueueMessage(unique_ptr<IWebSocketMessage> packet)
    {
        if (GetState() == WebSocketState::CLOSED)
        {
            LogError("Trying to send on a previously closed socket");

            MetricsTransportInvalidStateError();

            return;
        }

#ifdef LOG_TEXT_MESSAGES
        if (packet->wstype == WS_TEXT_FRAME)
        {
            LogInfo("Message sent:\n>>>>>>>>>>\n%.*s\n>>>>>>>>>>", packet->length, packet->buffer.get());
        }
#endif

        {
            lock_guard<mutex> lock(m_queue_lock);
            m_queue.push(std::move(packet));
        }
    }

    int WebSocket::SendMessage(unique_ptr<IWebSocketMessage> message)
    {
        int err = 0;

        // get the serialized buffer of data to send
        auto boundMessage = std::make_unique<BoundMessage>(std::move(message), shared_from_this());
        size_t bytes = boundMessage->message->Serialize(boundMessage->buffer);

        // TODO: This does not handle breaking up large payloads into multiple chunks
        err = uws_client_send_frame_async(
            m_WSHandle,
            static_cast<uint8_t>(boundMessage->message->FrameType() == WS_FRAME_TYPE_TEXT ? WS_TEXT_FRAME : WS_BINARY_FRAME),
            boundMessage->buffer.get(),
            bytes,
            true,
            [](void *ctxt, WS_SEND_FRAME_RESULT result)
            {
                std::unique_ptr<BoundMessage> boundMessage(static_cast<BoundMessage*>(ctxt));
                if (boundMessage != nullptr)
                {
                    auto instance = boundMessage->instance.lock();
                    if (instance != nullptr)
                    {
                        instance->HandleWebSocketFrameSent(boundMessage->message.get(), result);
                    }
                }
            },
            boundMessage.get());

        if (err)
        {
            LogError("WS transfer failed with %d", err);
        }
        else
        {
            // Release control of the bound message memory so the callback can delete it
            boundMessage.release();
        }

        return err;
    }

    void WebSocket::HandleConnected()
    {
        if (m_valid)
        {
            OnConnected();
        }
    }

    void WebSocket::HandleDisconnected(WebSocketDisconnectReason reason, const string & cause)
    {
        OnDisconnected(reason, cause);
    }

    void WebSocket::HandleTextData(const string & data)
    {
        OnTextData(data);
    }

    void WebSocket::HandleBinaryData(const uint8_t * data, const size_t size)
    {
        OnBinaryData(data, size);
    }

    void WebSocket::HandleError(WebSocketError reason, int errorCode, const std::string& errorMessage)
    {
        if (m_valid)
        {
            auto error = Impl::ErrorInfo::FromWebSocket(reason, errorCode, errorMessage);
            OnError(error);
        }
    }

    void WebSocket::HandleWebSocketStateChanged(WebSocketState oldState, WebSocketState newState)
    {
        OnStateChanged(oldState, newState);
    }

    void WebSocket::HandleWebSocketFrameSent(IWebSocketMessage* packet, WS_SEND_FRAME_RESULT result)
    {
        if (packet == nullptr)
        {
            return;
        }

        auto msgType = packet->MetricMessageType();
        if (msgType != MetricMessageType::METRIC_MESSAGE_TYPE_INVALID)
        {
            MetricsTransportStateEnd(msgType);
        }

        packet->MessageSent(result == WS_SEND_FRAME_OK);
    }

    void WebSocket::WorkLoop(weak_ptr<WebSocket> weakPtr)
    {
        /* NOTE:
           To prevent being invoked on a destroyed instance, this uses a weak pointer to check
           that the web socket instance has not been destroyed. However, if the timing is just
           right you could end up with a scenario where the ptr at line 556 becomes the last
           remaining reference to the web socket instance. An example of this is if an error
           callback causes the main thread (and the expected owner of the web socket instance)
           to exit before the body of the packaged task below completes. This has the subtle
           side effect of moving the destructor of the web socket instance to the
           CSpxThreadService thread instead of where you expect leading to an exception being
           thrown.

           In normal usage this does not seem to happen. However some integration tests (e.g.
           "Port specification" in core tests) will trigger this if you don't add a delay after
           waiting for the cancelled event.
        */

        packaged_task<void()> task([weakPtr]() -> void
        {
            auto ptr = weakPtr.lock();
            if (ptr == nullptr || !ptr->m_valid || ptr->GetState() == WebSocketState::CLOSED)
            {
                return;
            }

            try
            {
                try
                {
                    ptr->DoWork();
                }
                catch (const exception& e)
                {
                    ptr->HandleError(WebSocketError::UNKNOWN, -1, e.what());
                }
                catch (...)
                {
                    ptr->HandleError(WebSocketError::UNKNOWN, -1, "Unhandled exception in the USP layer.");
                }
            }
            catch (const exception& ex)
            {
                (void)ex;
                LogError("%s Unexpected Exception %s. Thread terminated", __FUNCTION__, ex.what());
            }
            catch (...)
            {
                LogError("%s Unexpected Exception. Thread terminated", __FUNCTION__);
            }

            packaged_task<void()> task([ptr]() { WorkLoop(ptr); });
            ptr->m_threadService->ExecuteAsync(move(task), ptr->m_pollingIntervalMs, ptr->m_affinity);
        });

        auto ptr = weakPtr.lock();
        if (ptr == nullptr || !ptr->m_valid || ptr->GetState() == WebSocketState::CLOSED)
        {
            return;
        }

        ptr->m_threadService->ExecuteAsync(move(task), ptr->m_affinity);
    }

    void WebSocket::DoWork()
    {
        // We first want to read the packets we received from the service. In case of turn.end we make sure
        // we enqueue the telemetry packets inside the request->queue and by the end of this function we will
        // be sure that the packets has reached the azure_c_lib.
        uws_client_dowork(m_WSHandle);

        switch (GetState())
        {
            case WebSocketState::CLOSED:
                {
                    lock_guard<mutex> lock(m_queue_lock);
                    queue<unique_ptr<IWebSocketMessage>> empty;
                    std::swap(m_queue, empty);
                }
                break;

            case WebSocketState::DESTROYING:
                // Do nothing. We are waiting for closing the transport request.
                break;

            case WebSocketState::INITIAL:
                LogInfo("%s: open transport.", __FUNCTION__);
                if (Connect())
                {
                    ChangeState(WebSocketState::INITIAL, WebSocketState::CLOSED);
                    LogError("Failed to open transport");
                    return;
                }
                else
                {
                    ChangeState(WebSocketState::INITIAL, WebSocketState::OPENING);
                }
                break;

            case WebSocketState::OPENING:
                // waiting for the OnWebSocketOpened callback
                break;

            case WebSocketState::CONNECTED:
                // limit so as not starve receive thread in case of a high rate of packets
                // to send. Rest of queued packets will be handled in the next cycle
                for (size_t numPackets = 0; numPackets < MAX_SEND_PER_CYCLE; numPackets++)
                {
                    unique_ptr<IWebSocketMessage> packet;
                    {
                        lock_guard<mutex> lock(m_queue_lock);

                        if (m_queue.empty())
                        {
                            break;
                        }
                        else
                        {
                            packet = std::move(m_queue.front());
                            m_queue.pop();
                        }
                    }

                    if (packet)
                    {
                        if (packet->MetricMessageType() != MetricMessageType::METRIC_MESSAGE_TYPE_INVALID)
                        {
                            try
                            {
                                MetricsTransportStateStart(packet->MetricMessageType());
                            }
                            catch (...) { /* don't care */ }
                        }

                        int res = SendMessage(std::move(packet));
                        if (res != 0)
                        {
                            HandleError(WebSocketError::WEBSOCKET_SEND_FRAME, res, std::string{});
                        }
                    }
                }

                break;
        }
    }

    void WebSocket::OnWebSocketOpened(WS_OPEN_RESULT_DETAILED open_result_detailed)
    {
        WS_OPEN_RESULT open_result = open_result_detailed.result;

        if (GetState() == WebSocketState::DESTROYING)
        {
            LogInfo("%s: request is null or in destroying state, ignore OnWSOpened()", __FUNCTION__);
            return;
        }

        m_open = (open_result == WS_OPEN_OK);
        if (m_open)
        {
            ChangeState(WebSocketState::OPENING, WebSocketState::CONNECTED);
            m_connectionTime = get_epoch_time();

            LogInfo("Opening websocket completed. TransportRequest: 0x%x, wsio handle: 0x%x", this, m_WSHandle);

            MetricsTransportConnected(m_telemetry, m_connectionId);

            HandleConnected();
        }
        else
        {
            // It is safe to transition to TRANSPORT_STATE_CLOSED because the
            // connection is not open.  We must be careful with this state for the
            // reasons described in OnTransportError.
            ChangeState(WebSocketState::CLOSED);

            MetricsTransportDropped();

            LogError("WS open operation failed with result=%d(%s), code=%d[0x%08x]",
                open_result,
                ENUM_TO_STRING(WS_OPEN_RESULT, open_result),
                open_result_detailed.code,
                open_result_detailed.code);

            HttpStatusCode statusCode = static_cast<HttpStatusCode>(open_result_detailed.code);

            if (open_result == WS_OPEN_ERROR_BAD_RESPONSE_STATUS
                && statusCode != HttpStatusCode::OK)
            {
                std::string errorString;

                // special case to handle redirects
                if (statusCode == HttpStatusCode::MOVED_PERMANENTLY
                    || statusCode == HttpStatusCode::PERM_REDIRECT
                    || statusCode == HttpStatusCode::TEMP_REDIRECT)
                {
                    size_t crOffset = 0;
                    size_t buffSize = open_result_detailed.buffSize;

                    // skip the first line in the HTTP response that contains the HTTP version, status code, and status
                    // message. E.g.: HTTP/1.1 200 OK
                    for (crOffset = 0; crOffset < buffSize; crOffset++)
                    {
                        if (open_result_detailed.buffer[crOffset] == '\n')
                        {
                            break;
                        }
                    }

                    std::unordered_map<std::string, std::string> httpHeaders;
                    WebSocket::ParseHttpHeaders(
                        static_cast<const uint8_t*>(open_result_detailed.buffer + crOffset + 1),
                        buffSize - crOffset - 1,
                        httpHeaders);

                    // using the [] on an unordered map actually adds a 'default' value if none is found but we don't
                    // care about that here
                    errorString = httpHeaders["location"];
                    if (errorString.empty())
                    {
                        errorString = httpHeaders["Location"];
                    }
                }

                HandleError(WebSocketError::WEBSOCKET_UPGRADE, /* HTTP status */ (int)statusCode, errorString);
            }
            else
            {
                const auto message = std::string("Code: ") + to_string(open_result_detailed.code) + ".";
                HandleError(WebSocketError::CONNECTION_FAILURE, open_result_detailed.result, message.c_str());
            }
        }
    }

    void WebSocket::OnWebSocketFrameReceived(unsigned char frame_type, const unsigned char * buffer, size_t size)
    {
        if (GetState() == WebSocketState::DESTROYING)
        {
            LogInfo("%s: request is in destroying state, ignore OnWSFrameReceived().", __FUNCTION__);
            return;
        }

        if (!m_valid || !m_open)
        {
            LogError("%s: request is not valid and/or not open", __FUNCTION__);
            return;
        }

        string parsed;

        switch (frame_type)
        {
            case WS_FRAME_TYPE_TEXT:
#ifdef LOG_TEXT_MESSAGES
                LogInfo("Message received:%.*s", size, buffer);
#endif

                parsed = std::string(reinterpret_cast<const char *>(buffer), size);
                HandleTextData(parsed);
                break;

            case WS_FRAME_TYPE_BINARY:
                HandleBinaryData(static_cast<const uint8_t *>(buffer), size);
                break;

            default:
                LogError("ProtocolViolation: Unknown message type: %d", frame_type);
                break;
        }
    }

    void WebSocket::OnWebSocketPeerClosed(uint16_t * closeCode, const unsigned char * extraData, size_t extraDataLength)
    {
        LogInfo("%s: context=%p", __FUNCTION__, this);

        m_open = false;
        ChangeState(WebSocketState::CLOSED);

        MetricsTransportDropped();

        auto reason = static_cast<WebSocketDisconnectReason>(closeCode != nullptr ? *closeCode : -1);
        std::string cause;
        if (extraDataLength > 0)
        {
            cause = std::string(reinterpret_cast<const char *>(extraData), extraDataLength);
        }

        HandleDisconnected(reason, cause);
    }

    void WebSocket::OnWebSocketError(WS_ERROR errorCode)
    {
        LogError("WS operation failed with error code=%d(%s)", errorCode, ENUM_TO_STRING(WS_ERROR, errorCode));

        m_open = false;
        ChangeState(WebSocketState::CLOSED);

        MetricsTransportDropped();

        HandleError(WebSocketError::WEBSOCKET_ERROR, errorCode, ENUM_TO_STRING(WS_ERROR, errorCode));
    }

    void WebSocket::OnWebSocketClosed()
    {
        LogInfo("%s: context=%p", __FUNCTION__, this);

        m_open = false;
        ChangeState(WebSocketState::CLOSED);
        MetricsTransportClosed();
        HandleDisconnected(WebSocketDisconnectReason::Normal, "");
    }

    uint64_t WebSocket::GetTimestamp() const
    {
        return get_epoch_time() - m_creationTime;
    }

}}}}
