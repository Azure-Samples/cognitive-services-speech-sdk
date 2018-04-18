//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspinternal.h: the header file only used by usplib internally
//

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <map>
#include <memory>
#include <chrono>
#include <utility>

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdlib.h>
#include <memory.h>
#ifdef __linux__
#include <unistd.h>
#include <errno.h>
#endif

#include <assert.h>
#include <sstream>

#include "usp.h"
#include "transport.h"

#ifndef ARRAYSIZE
#define ARRAYSIZE(__a) (sizeof(__a) / sizeof(__a[0]))
#endif

#ifdef __linux__
#define localtime_s(__tm, __timet) localtime_r(__timet, __tm)
#define gmtime_s(__tm, __timet) gmtime_r(__timet, __tm)
#define scanf_s scanf
#define sscanf_s sscanf
#endif

typedef struct _DNS_CONTEXT* DnsCacheHandle;
typedef struct _TransportRequest* TransportHandle;
typedef struct _TELEMETRY_CONTEXT* TELEMETRY_HANDLE;
typedef struct HTTP_HEADERS_HANDLE_DATA_TAG* HTTP_HEADERS_HANDLE;

namespace USP {

class Connection::Impl : public std::enable_shared_from_this<Connection::Impl>
{

public:
    /**
    * Creates a new Impl instance.
    * @param config Specifies USP client configuration parameters (including an instance of the Callbacks class).
    */
    Impl(const Client& config);

    /**
    * Establishes a new connection to the service, launches a worker thread
    * to process incoming/outgoing requests.
    */
    void Connect();

    /**
    * Adds an audio segment to the outgoing queue.
    * @param data The audio data to be sent. Audio data must be aligned on the audio sample boundary.
    * @param size The length of the audio data, in bytes.
    */
    void QueueAudioSegment(const uint8_t* data, size_t size);

    /**
    * Adds an empty audio sigment to the outgoing queue, which serves as a singnal to the service that the audio 
    * stream has ended (i.e., has been uploaded completely).
    * @param uspHandle the UspHandle for sending the audio.
    */
    void QueueAudioEnd();

    /**
    * Adds a message to the outgoing queue.
    * @param path The path associated with the message being sent.
    * @param data The message payload.
    * @param size The length of the message in bytes.
    */
    void QueueMessage(const std::string& path, const uint8_t* data, size_t size);

    /**
    * Requests the connection to service to be shut down.
    * @param uspContext A pointer to the UspContext.
    * @return A UspResult indicating success or error.
    */
    void Shutdown();

private:

    using DnsCachePtr = deleted_unique_ptr<std::remove_pointer<DnsCacheHandle>::type>;

    using TelemetryPtr = deleted_unique_ptr<std::remove_pointer<TELEMETRY_HANDLE>::type>;

    using TransportPtr = deleted_unique_ptr<std::remove_pointer<TransportHandle>::type>;

    void Validate();

    static void WorkThread(std::weak_ptr<Connection::Impl> ptr);

    void SignalWork();
    void SignalConnected();

    std::string ConstructConnectionUrl();

    Client m_config;

    bool m_connected;

    // Todo: can multiple UspContexts share the work thread?
    bool m_haveWork;

    std::recursive_mutex m_mutex;
    std::condition_variable_any m_cv;
    std::thread m_worker;

    size_t m_audioOffset;

    DnsCachePtr m_dnsCache;
    TelemetryPtr m_telemetry;
    TransportPtr m_transport;

    const uint64_t m_creationTime;

    static void OnTelemetryData(const uint8_t* buffer, size_t bytesToWrite, void *context, const char *requestId);

    static void OnTransportError(TransportHandle transportHandle, TransportError reason, void* context);

    static void OnTransportData(TransportHandle transportHandle, HTTP_HEADERS_HANDLE responseHeader, const unsigned char* buffer, size_t size, unsigned int errorCode, void* context);

    uint64_t getTimestamp();
};

}
