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
#include <unordered_set>

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
#include "metrics.h"

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
typedef struct TELEMETRY_CONTEXT* TELEMETRY_HANDLE;
typedef struct HTTP_HEADERS_HANDLE_DATA_TAG* HTTP_HEADERS_HANDLE;

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
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
    * @param audioChunk the audio chunk to be queued.
    */
    void QueueAudioSegment(const Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk);

    /**
    * Adds an empty audio segment to the outgoing queue, which serves as a signal to the service that the audio
    * stream has ended (i.e., has been uploaded completely).
    * @param uspHandle the UspHandle for sending the audio.
    */
    void QueueAudioEnd();

    /**
    * Adds a message to the outgoing queue.
    * @param path The path associated with the message being sent.
    * @param data The message payload.
    * @param size The length of the message in bytes.
    * @param messageType The type of message to be sent.
    * @param requestId The request ID of this turn.
    */
    void QueueMessage(const std::string& path, const uint8_t* data, size_t size, MessageType messageType, const std::string& requestId, bool binary = false);

    /**
    * Writes latency value into telemetry data.
    * @param latencyInTicks The latency value in ticks.
    * @param isPhraseLatency If it is true, the latency is for phrase result. Otherwise it is for hypothesis result.
    */
    void WriteTelemetryLatency(uint64_t latencyInTicks, bool isPhraseLatency);

    /**
    * Requests the connection to service to be shut down.
    * @param uspContext A pointer to the UspContext.
    */
    void Shutdown();

    /**
    * Returns true if the status is connected.
    */
    bool IsConnected();

    /**
    * Returns the URL used for connection.
    */
    std::string GetConnectionUrl();

private:
    void Invoke(std::function<void()> callback);

    using DnsCachePtr = deleted_unique_ptr<std::remove_pointer<DnsCacheHandle>::type>;

    void ScheduleWork();

    static void DoWork(std::weak_ptr<Connection::Impl> ptr);
    static void WorkLoop(std::shared_ptr<Connection::Impl> ptr);

    std::string EncodeParameterString(const std::string& parameter) const;
    std::string ConstructConnectionUrl() const;

    void BuildQueryParameters(const std::vector<std::string>& parameterList, const std::unordered_map<std::string, std::string>& valueMap, bool isCustomEndpoint, std::ostringstream& oss) const;

    std::string CreateRequestId();
    void RegisterRequestId(const std::string& requestId);
    std::string UpdateRequestId(const MessageType messageType);

    const char queryParameterDelim = '&';

    std::unordered_set<std::string> m_activeRequestIds;
    std::string m_speechRequestId;
    Client m_config;
    bool m_valid;
    bool m_connected;
    bool m_speechContextMessageAllowed;
    std::string m_connectionUrl;
    size_t m_audioOffset;
    DnsCachePtr m_dnsCache;
    std::unique_ptr<Telemetry> m_telemetry;
    std::unique_ptr<TransportRequest> m_transport;
    const uint64_t m_creationTime;

    static void OnTelemetryData(const uint8_t* buffer, size_t bytesToWrite, void *context, const char *requestId);
    static void OnTransportOpened(void* context);
    static void OnTransportClosed(void* context);
    static void OnTransportError(TransportErrorInfo* errorInfo, void* context);
    static void OnTransportData(TransportResponse* response, void* context);

    void InvokeRecognitionErrorCallback(RecognitionStatus status, const std::string& response);

    uint64_t getTimestamp();
    SpeechPhraseMsg RetrieveSpeechPhraseResult(const nlohmann::json& json);
    bool isErrorRecognitionStatus(RecognitionStatus status);
    std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxThreadService> m_threadService;

    void FillLanguageForAudioOutputChunkMsg(const char* streamId, const std::string& messagePath, AudioOutputChunkMsg& msg);
    std::unordered_map<std::string, std::string> m_streamIdLangMap;
};

}}}}
