//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// WebSocket.cpp: A web socket C++ implementation using the Azure C shared library
//

#include "stdafx.h"
#include "WebSocket.h"
#include "exception.h"
#include "azure_c_shared_utility_includes.h"
#include "azure_c_shared_utility_macro_utils.h"
#include "uspcommon.h"

// uncomment the line below to see all non-binary protocol messages in the log
// #define LOG_TEXT_MESSAGES

using namespace std;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    // USP: This is the unknown value for telemetry. Switch to passing correct USP value
    constexpr uint8_t METRIC_MESSAGE_TYPE_INVALID = 0xff;

    // The maximum number of send requests to process per cycle in the background web socket thread.
    // This prevents us blocking for a long time if the rate of send requests is too high
    constexpr size_t MAX_SEND_PER_CYCLE = 20;

    struct TransportPacket
    {
        inline TransportPacket(uint8_t msgtype, uint8_t wstype, size_t buffer_size)
            : msgtype{ msgtype }, wstype{ wstype }, length{ buffer_size }, buffer{ std::make_unique<uint8_t[]>(buffer_size) }
        {}

        TransportPacket(const TransportPacket&) = delete;
        TransportPacket(TransportPacket&&) = default;

        uint8_t                    msgtype;
        uint8_t                    wstype;
        size_t                     length;
        std::unique_ptr<uint8_t[]> buffer;
    };

    template<typename TInstance, typename TValue>
    struct InstanceAndValue
    {
        InstanceAndValue() : InstanceAndValue(nullptr, nullptr) {}
        InstanceAndValue(weak_ptr<TInstance> instance, unique_ptr<TValue>&& value)
            : m_instance(instance), m_value(std::move(value))
        {
        }

        static inline unique_ptr<InstanceAndValue<TInstance, TValue>> from_ptr(void * ptr)
        {
            return std::unique_ptr<InstanceAndValue<TInstance, TValue>>(static_cast<InstanceAndValue<TInstance, TValue>*>(ptr));
        }

        template<typename ...TArgs>
        void invoke(void (TInstance::* method)(TArgs...), TArgs... args)
        {
            auto instance = m_instance.lock();
            if (instance != nullptr)
            {
                (instance.get()->*method)(args...);
            }
        }

        TValue * value()
        {
            return m_value.get();
        }

    private:
        weak_ptr<TInstance> m_instance;
        unique_ptr<TValue> m_value;
    };

    struct WebSocket::connection_params
    {
        connection_params() = default;
        connection_params(const WebSocketParams& params) :
            scheme(params.IsSecure() ? "wss" : "ws"),
            host(params.Host()),
            port(params.Port()),
            pathAndQuery(params.Path() + params.QueryString()),
            protocols(params.webSocketProtocols()),
            connectionId(params.ConnectionId()),
            headers(HTTPHeaders_Alloc()),
            proxy(params.proxyServerInfo()),
            singleTrustedCert(params.singleTrustedCertificate()),
            disableCrlChecks(params.disableCrlChecks()),
            disableDefaultVerifyPaths(params.disableDefaultVerifyPaths())
        {
            if (host.empty())
            {
                Impl::ThrowInvalidArgumentException("Received an empty host. Please provide a valid host.");
            }
            else if (port <= 0 || port > 65535)
            {
                Impl::ThrowInvalidArgumentException("The port must be a value between 1 and 65535 (inclusive)");
            }

            if (pathAndQuery.empty() || pathAndQuery[0] != '/')
            {
                pathAndQuery = "/" + pathAndQuery;
            }

            // set the headers
            for (const auto& kvp : params.Headers())
            {
                HTTP_HEADERS_RESULT res = HTTPHeaders_ReplaceHeaderNameValuePair(headers, kvp.first.c_str(), kvp.second.c_str());
                if (res != HTTP_HEADERS_OK)
                {
                    std::string errMsg("Failed to set the following web socket error: ");
                    errMsg += kvp.first;
                    Impl::ThrowRuntimeError(errMsg);
                }
            }

            HTTPHeaders_ReplaceHeaderNameValuePair(headers, "X-ConnectionId", connectionId.c_str());
        }

        ~connection_params()
        {
            HTTPHeaders_Free(headers);
        }

        std::string scheme;
        std::string host;
        int port;
        std::string pathAndQuery;
        std::string protocols;
        std::string connectionId;
        HTTP_HEADERS_HANDLE headers;
        ProxyServerInfo proxy;
        std::string singleTrustedCert;
        bool disableCrlChecks;
        bool disableDefaultVerifyPaths;
    };

    static uint64_t get_epoch_time()
    {
        auto now = chrono::high_resolution_clock::now();
        auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch());
        return ms.count();
    }

    template<typename ...Args>
    static void cast_and_invoke(void *context, void (WebSocket::* callback)(Args...), Args... args)
    {
        auto webSocket = static_cast<WebSocket*>(context);
        if (webSocket != nullptr)
        {
            (webSocket->*callback)(args...);
        }
    }

    WebSocket::WebSocket(
            shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> threadService,
            const chrono::milliseconds& pollingIntervalMs) :
        m_threadService(threadService),
        m_pollingIntervalMs(pollingIntervalMs),
        m_valid(false),
        m_creationTime(get_epoch_time()),
        m_connectionTime(),
        m_dnsCache(nullptr, DnsCacheDestroy),
        m_params(),
        m_WSHandle(),
        m_open(),
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
        m_dnsCache.reset();
        m_params.reset();

        if (m_WSHandle)
        {
            uws_client_destroy(m_WSHandle);
        }

        queue<unique_ptr<TransportPacket>> empty;
        swap(m_queue, empty);
    }

    void WebSocket::Connect(const WebSocketParams& params)
    {
        if (m_open)
        {
            Impl::ThrowLogicError("Web socket is already connected.");
        }

        // copy and validate the connection parameters. This is needed to keep
        // valid c_strings pointers in memory
        m_params = make_unique<connection_params>(params);

        // USP: This is where the original transport code used the protocol header value to add
        //      additional headers. The latest version of the UWS Azure C shared code includes
        //      a proper way to set headers

        WS_PROTOCOL wsProto;
        wsProto.protocol = m_params->protocols.c_str();
        int protocolCount = m_params->protocols.empty() ? 0 : 1;

        // create the underlying web socket handle
        if (!m_params->proxy.host.empty())
        {
            HTTP_PROXY_IO_CONFIG proxyConfig;
            proxyConfig.hostname = m_params->host.c_str();
            proxyConfig.port = m_params->port;
            proxyConfig.proxy_hostname = m_params->proxy.host.c_str();
            proxyConfig.proxy_port = m_params->proxy.port;
            proxyConfig.username = m_params->proxy.username.c_str();
            proxyConfig.password = m_params->proxy.password.c_str();

            void* underlyingIOParameters = &proxyConfig;
            const IO_INTERFACE_DESCRIPTION* underlyingIOInterface = http_proxy_io_get_interface_description();
            if (underlyingIOInterface == nullptr)
            {
                Impl::ThrowRuntimeError("NULL proxy interface description");
            }

            TLSIO_CONFIG tlsioConfig;
            if (params.IsSecure())
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
                m_params->host.c_str(),
                m_params->port,
                m_params->pathAndQuery.c_str(),
                protocolCount > 0 ? &wsProto : nullptr,
                protocolCount);
        }
        else
        {
            m_WSHandle = uws_client_create(
                m_params->host.c_str(),
                m_params->port,
                m_params->pathAndQuery.c_str(),
                params.IsSecure(),
                protocolCount > 0 ? &wsProto : nullptr,
                protocolCount);
        }

        if (m_WSHandle == nullptr)
        {
            Impl::ThrowRuntimeError("Failed to create the web socket");
            return;
        }

#ifdef SPEECHSDK_USE_OPENSSL
        int tls_version = OPTION_TLS_VERSION_1_2;
        uws_client_set_option(m_WSHandle, OPTION_TLS_VERSION, &tls_version);

        uws_client_set_option(m_WSHandle, OPTION_DISABLE_DEFAULT_VERIFY_PATHS, &(m_params->disableDefaultVerifyPaths));
        if (!m_params->singleTrustedCert.empty())
        {
            uws_client_set_option(m_WSHandle, OPTION_TRUSTED_CERT, m_params->singleTrustedCert.c_str());

            uws_client_set_option(m_WSHandle, OPTION_DISABLE_CRL_CHECK, &(m_params->disableCrlChecks));
        }
#endif

#ifdef __linux__
        m_dnsCache = DnsCachePtr(DnsCacheCreate(), DnsCacheDestroy);
        if (!m_dnsCache)
        {
            Impl::ThrowRuntimeError("Failed to create DNS cache.");
        }
#else
        m_dnsCache = nullptr;
#endif

        // USP: In the original transport code, this is this is deferred until you call TransportRequestPrepare
        //      but we will do this now
        ChangeState(WebSocketState::CLOSED, WebSocketState::NETWORK_CHECK);

        m_valid = true;
        WorkLoop(shared_from_this());
    }

    static void PumpWebSocketInBackground(UWS_CLIENT_HANDLE handle, std::shared_ptr<Impl::ISpxThreadService> threadService)
    {
        // Helper method to pump the Azure C shared library web socket code on a background thread. This
        // prevents situations where disconnecting can trigger event callbacks on the same thread as
        // disconnect was called on. This is problematic if you are using the thread service ExecuteSync
        // in other code.
        std::packaged_task<void()> task([handle]()
        {
            uws_client_dowork(handle);
        });

        threadService->ExecuteSync(std::move(task));
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

        if (m_dnsCache != nullptr)
        {
            DnsCacheRemoveContextMatches(m_dnsCache.get(), this);
        }

        if (m_WSHandle != nullptr)
        {
            if (m_open)
            {
                // starts the close handshake.
                LogInfo("%s: start the close handshake.", __FUNCTION__);

                int result = uws_client_close_handshake_async(
                    m_WSHandle,
                    WebSocketDisconnectReason::Normal,
                    "" /* not used */,
                    [](void *ctxt) { cast_and_invoke(ctxt, &WebSocket::OnWebSocketClosed); },
                    this);
                if (result == 0)
                {
                    // waits for close.
                    int retries = 0;
                    while (m_open && retries++ < MAX_WAIT_RETRIES)
                    {
                        LogInfo("%s: Continue to pump while waiting for close frame response (%d/%d).", __FUNCTION__, retries, MAX_WAIT_RETRIES);
                        PumpWebSocketInBackground(m_WSHandle, m_threadService);
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
                        PumpWebSocketInBackground(m_WSHandle, m_threadService);
                        std::this_thread::sleep_for(SLEEP_INTERVAL);
                    }

                    LogInfo("%s: isOpen: %s", __FUNCTION__, m_open ? "true" : "false");
                }
            }

            LogInfo("%s: destroying uwsclient.", __FUNCTION__);
            uws_client_destroy(m_WSHandle);
            m_WSHandle = nullptr;
        }

        // USP: sends telemetry: MetricsTransportClosed();
    }

    void WebSocket::SendTextData(const string & text)
    {
        size_t payloadSize = text.length();

        // std::string.length() returns the number of raw bytes in the string but rather than the number of UTF-8 code points
        auto msg = make_unique<TransportPacket>(
            static_cast<uint8_t>(METRIC_MESSAGE_TYPE_INVALID), 
            static_cast<uint8_t>(WS_FRAME_TYPE_TEXT),
            payloadSize);

        // USP: Token handling stuff here for USP goes here. E.g. ret = usp::TransportRequestPrepare(request);
        // USP: The transport code computes header sizes here, and adds time stamp, request ID headers, etc..

        memcpy(msg->buffer.get(), text.c_str(), payloadSize);

        QueuePacket(move(msg));
    }

    void WebSocket::SendBinaryData(const uint8_t * data, const size_t size)
    {
        if (data == nullptr)
        {
            return;
        }

        auto msg = make_unique<TransportPacket>(
            static_cast<uint8_t>(METRIC_MESSAGE_TYPE_INVALID),
            static_cast<uint8_t>(WS_FRAME_TYPE_TEXT),
            size);

        // USP: Does some audio processing/fiddling here
        // USP: Computes header sizes here, and adds time stamp, request ID headers, etc..

        memcpy(msg->buffer.get(), data, size);

        QueuePacket(move(msg));
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

            // USP: Sends telemetry here: MetricsTransportStart(*request->telemetry, request->connectionId);

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

    void WebSocket::QueuePacket(unique_ptr<TransportPacket> packet)
    {
        if (nullptr == packet)
        {
            return;
        }

        if (GetState() == WebSocketState::CLOSED)
        {
            LogError("Trying to send on a previously closed socket");

            // USP: Sends telemetry here: MetricsTransportInvalidStateError();

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
            m_queue.push(move(packet));
        }
    }

    int WebSocket::SendPacket(unique_ptr<TransportPacket> packet)
    {
        int err = 0;

        // USP: Changes the header time stamp value. It then updates the binary data present

        auto context = make_unique<InstanceAndValue<WebSocket, TransportPacket>>(this->shared_from_this(), std::move(packet));

        // TODO: This does not handle breaking up large payloads into multiple chunks
        err = uws_client_send_frame_async(
            m_WSHandle,
            static_cast<uint8_t>(context->value()->wstype == WS_FRAME_TYPE_TEXT ? WS_TEXT_FRAME : WS_BINARY_FRAME),
            context->value()->buffer.get(),
            context->value()->length,
            true,
            [](void *ctxt, WS_SEND_FRAME_RESULT result)
            {
                auto context = InstanceAndValue<WebSocket, TransportPacket>::from_ptr(ctxt);
                if (context != nullptr)
                {
                    auto packet = context->value();
                    context->invoke(&WebSocket::HandleWebSocketFrameSent, result, packet->msgtype, packet->wstype);
                }
            },
            context.get());

        if (err)
        {
            LogError("WS transfer failed with %d", err);
        }
        else
        {
            // Release control of the context memory so the callback can delete it
            context.release();
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
        // USP: This is where the "headers" are parsed out of the received packets

        if (m_valid && m_open)
        {
            OnTextData(data);
        }
    }

    void WebSocket::HandleBinaryData(const uint8_t * data, const size_t size)
    {
        // USP: This is where the "headers" are parsed out of the received packets

        if (m_valid && m_open)
        {
            OnBinaryData(data, size);
        }
    }

    void WebSocket::HandleError(WebSocketError reason, int errorCode, const std::string& errorMessage)
    {
        if (m_valid)
        {
            OnError(reason, errorCode, errorMessage);
        }
    }

    void WebSocket::HandleWebSocketStateChanged(WebSocketState oldState, WebSocketState newState)
    {
        OnStateChanged(oldState, newState);
    }

    void WebSocket::HandleWebSocketFrameSent(WS_SEND_FRAME_RESULT result, uint8_t msgType, uint8_t wsType)
    {
        UNUSED(result);
        UNUSED(msgType);
        UNUSED(wsType);

        // USP: Sends telemetry here:
        //      if (msgType != METRIC_MESSAGE_TYPE_INVALID) MetricsTransportStateEnd(msgType);
    }

    void WebSocket::WorkLoop(weak_ptr<WebSocket> weakPtr)
    {
        packaged_task<void()> task([weakPtr]() -> void
        {
            auto ptr = weakPtr.lock();
            if (ptr == nullptr || !ptr->m_valid)
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
            ptr->m_threadService->ExecuteAsync(move(task), ptr->m_pollingIntervalMs);
        });

        auto ptr = weakPtr.lock();
        if (ptr == nullptr || !ptr->m_valid)
        {
            return;
        }

        ptr->m_threadService->ExecuteAsync(move(task));
    }

    void WebSocket::DoWork()
    {
        switch (GetState())
        {
            case WebSocketState::CLOSED:
                {
                    lock_guard<mutex> lock(m_queue_lock);
                    queue<unique_ptr<TransportPacket>> empty;
                    std::swap(m_queue, empty);
                }
                break;

            case WebSocketState::RESETTING:
                // Do nothing -- we're waiting for WSOnCloseForReset to be invoked.
                break;

            case WebSocketState::DESTROYING:
                // Do nothing. We are waiting for closing the transport request.
                break;

            case WebSocketState::NETWORK_CHECKING:
                DnsCacheDoWork(m_dnsCache.get(), this);
                return;

            case WebSocketState::NETWORK_CHECK:
                if (nullptr == m_dnsCache)
                {
                    // skip DNS checks
                    ChangeState(WebSocketState::NETWORK_CHECK, WebSocketState::NETWORK_CHECK_COMPLETE);
                }
                else
                {
                    ChangeState(WebSocketState::NETWORK_CHECK, WebSocketState::NETWORK_CHECKING);
                    const char* host = m_params->host.c_str();
                    LogInfo("Start network check %s", host);

                    // USP: Sends telemetry here: MetricsTransportStateStart(MetricMessageType::METRIC_TRANSPORT_STATE_DNS);

                    int result = DnsCacheGetAddr(
                        m_dnsCache.get(),
                        host,
                        [](DnsCacheHandle handle, int error, DNS_RESULT_HANDLE result, void *context)
                        {
                            cast_and_invoke(context, &WebSocket::DnsComplete, handle, error, result);
                        },
                        this);

                    if (result)
                    {
                        LogError("DnsCacheGetAddr failed");
                        ChangeState(WebSocketState::NETWORK_CHECK, WebSocketState::NETWORK_CHECK_COMPLETE);
                    }
                }

                return;

            case NETWORK_CHECK_COMPLETE:
                LogInfo("%s: open transport.", __FUNCTION__);
                if (Connect())
                {
                    ChangeState(WebSocketState::NETWORK_CHECK_COMPLETE, WebSocketState::CLOSED);
                    LogError("Failed to open transport");
                    return;
                }
                else
                {
                    ChangeState(WebSocketState::NETWORK_CHECK_COMPLETE, WebSocketState::OPENING);
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
                    unique_ptr<TransportPacket> packet;
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
                        // USP: Sends telemetry here:
                        //      if (packet->msgtype != METRIC_MESSAGE_TYPE_INVALID) MetricsTransportStateStart(packet->msgtype);

                        int res = SendPacket(std::move(packet));
                        if (res != 0)
                        {
                            HandleError(WebSocketError::WEBSOCKET_SEND_FRAME, res, std::string{});
                        }
                    }
                }

                break;
        }

        uws_client_dowork(m_WSHandle);
    }

    void WebSocket::DnsComplete(DnsCacheHandle handle, int error, DNS_RESULT_HANDLE result)
    {
        assert(GetState() == WebSocketState::NETWORK_CHECKING);

        (void)handle;
        (void)result;

        // USP: Sends telemetry here: MetricsTransportStateEnd(MetricMessageType::METRIC_TRANSPORT_STATE_DNS);

        if (error != 0)
        {
            LogError("Network Check failed %d", error);

            // The network instance has not been connected yet so it is not
            // open, and it's safe to call OnTransportError.  We do the DNS
            // check before opening the connection.  We assert about this with
            // the WebSocketState::NETWORK_CHECKING check above.
            HandleError(WebSocketError::DNS_FAILURE, error, std::string{});
        }
        else
        {
            LogInfo("Network Check completed");
            ChangeState(WebSocketState::NETWORK_CHECKING, WebSocketState::NETWORK_CHECK_COMPLETE);
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

            // USP: Sends telemetry here: MetricsTransportConnected(*request->telemetry, request->connectionId);

            HandleConnected();
        }
        else
        {
            // It is safe to transition to TRANSPORT_STATE_CLOSED because the
            // connection is not open.  We must be careful with this state for the
            // reasons described in OnTransportError.
            ChangeState(WebSocketState::CLOSED);

            // USP: Sends telemetry here: MetricsTransportDropped();

            LogError("WS open operation failed with result=%d(%s), code=%d[0x%08x]",
                open_result,
                ENUM_TO_STRING(WS_OPEN_RESULT, open_result),
                open_result_detailed.code,
                open_result_detailed.code);

            if (open_result == WS_OPEN_ERROR_BAD_RESPONSE_STATUS)
            {
                HandleError(WebSocketError::WEBSOCKET_UPGRADE, /* HTTP status */ open_result_detailed.code, "");
            }
            else
            {
                HandleError(WebSocketError::CONNECTION_FAILURE, open_result_detailed.result, to_string(open_result_detailed.code).c_str());
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
                PROTOCOL_VIOLATION("Unknown message type: %d", frame_type);
                break;
        }
    }

    void WebSocket::OnWebSocketPeerClosed(uint16_t * closeCode, const unsigned char * extraData, size_t extraDataLength)
    {
        LogInfo("%s: context=%p", __FUNCTION__, this);

        m_open = false;
        ChangeState(WebSocketState::CLOSED);

        // USP: Sends telemetry: MetricsTransportDropped();

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

        // USP: Sends telemetry: MetricsTransportDropped();

        HandleError(WebSocketError::WEBSOCKET_ERROR, errorCode, ENUM_TO_STRING(WS_ERROR, errorCode));
    }

    void WebSocket::OnWebSocketClosed()
    {
        LogInfo("%s: context=%p", __FUNCTION__, this);

        m_open = false;

        if (GetState() == WebSocketState::RESETTING)
        {
            HandleDisconnected(WebSocketDisconnectReason::Normal, "Web socket connection is resetting");

            // Re-open the connection.
            ChangeState(WebSocketState::RESETTING, WebSocketState::NETWORK_CHECK);
        }
        else
        {
            ChangeState(WebSocketState::CLOSED);

            // USP: Sends telemetry: MetricsTransportClosed();

            HandleDisconnected(WebSocketDisconnectReason::Normal, "");
        }
    }

    uint64_t WebSocket::GetTimestamp() const
    {
        return get_epoch_time() - m_creationTime;
    }

}}}}
