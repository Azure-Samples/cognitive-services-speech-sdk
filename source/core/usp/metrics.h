//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// metrics.h: metric messages related structure and methods
//

// Todo: refactor to add/remove messages according to USP specification
// Todo: refactor to follow coding guideline.

#pragma once

#include <stdint.h>

#include <functional>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <array>
#include <memory>

#include <stdarg.h>
#include <stdlib.h>

#include <json.h>

/**
 * Returns the current time used for telemetry events.
 * @return A 64-bit time value.
 */
extern uint64_t telemetry_gettime();

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

constexpr size_t NO_DASH_UUID_LEN = 37;
constexpr size_t TIME_STRING_MAX_SIZE = 30;
constexpr size_t ERROR_STRING_MAX_SIZE = 50;
constexpr size_t MAX_EVENT_ARRAY_SIZE = 500;

#define IS_STRING_NULL_OR_EMPTY(str) (!str || !*str)


static const char music_session_id_arg[] = "music_session_id";
static const char action_name_arg[] = "action_name";
static const char buffer_session_id_arg[] = "buffer_id";

typedef void* PROPERTYBAG_HANDLE;

typedef struct TELEMETRY_CONTEXT* TELEMETRY_HANDLE;

/**
 * The PTELEMETRY_WRITE type represents an application-defined
 * callback function used to handle raw telemetry data.
 * @param pBuffer A pointer to a raw telemetry data.
 * @param byteToWrite The length of pBuffer in bytes.
 * @param pContext A pointer to the application-defined callback context.
 */
using PTELEMETRY_WRITE = std::function<void(const uint8_t* pBuffer, size_t byteToWrite, void *pContext, const char* requestId)>;

enum IncomingMsgType
{
    turnStart,
    turnEnd,
    speechStartDetected,
    speechEndDetected,
    speechHypothesis,
    speechFragment,
    speechPhrase,
    translationHypothesis,
    translationPhrase,
    translationSynthesis,
    translationSynthesisEnd,
    audio,
    response,
    countOfMsgTypes
};

struct TELEMETRY_DATA
{
    std::string requestId;
    bool bPayloadSet{};
    // TODO: This does not make sense, why telementry does not have
    // a proper set inside?
    std::array<nlohmann::json, countOfMsgTypes> receivedMsgs;

    nlohmann::json connectionJson{};
    nlohmann::json audioStartJson{};
    nlohmann::json microphoneJson{};
    nlohmann::json listeningTriggerJson{};
    nlohmann::json ttsJson{};
    nlohmann::json deviceJson{};
};

/**
* Returns the the current time in ISO8601 format
* @param buffer char buffer to hold the ISO8601 time string
* @param bufferLength the length of the buffer to be used to store the string
* @return 0 in case of success and -1 in case of failure
*/
int GetISO8601Time(char *buffer, unsigned int bufferLength);

/**
* Returns the the current time in ISO8601 format after adjusting for the offset
* @param buffer char buffer to hold the ISO8601 time string
* @param bufferLength the length of the buffer to be used to store the string
* @param offset    the time value to be offset from current time in milliseconds. If time needs to be added a negative value needs to be provided
* @return 0 in case of success and -1 in case of failure
*/
int GetISO8601TimeOffset(char *buffer, unsigned int bufferLength, int offset);

struct Telemetry
{
    /**
    * Creates a object handle to maintain telemetry state, initializes the internal structures and
    * sets the application defined callbacks used for uploading in-band telemetry events.
    * @param pfnCallback The callbacks used for uploading in-band telemetry events.
    * @param pContext The callback context.
    */
    Telemetry(PTELEMETRY_WRITE callback, void* context);
    ~Telemetry();

    /**
     * Flushes any outstanding telemetry events. Also flushes the telemetry associated with a given requestId.
     * @param requestId used to specify what telemetry to flush
     */
    void Flush(const std::string& requestId);

    /**
     * Timestamps and records telemetry event.
     * @param requestId The requestId associated with the current event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    void InbandEventTimestampPopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key);

    /**
     *  Records connection telemetry event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    void InbandConnectionTelemetry(const std::string& id, const std::string& key, const nlohmann::json& value);

    /**
     * Populates and records telemetry event.
     * @param requestId The requestId associated with the current event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    void InbandEventKeyValuePopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key, const nlohmann::json& );

    /**
     * Received metric event population function.
     * @param requestId The requestId associated with the received message.
     * @param receivedMsg The name of the received event from service.
     */
    void RecordReceivedMsg(const std::string& requestId, const std::string&receivedMsg);

    /**
     * Handles the necessary changes for a requestId change event.
     * @param requestId the currently active requestId.
     */
    void RegisterNewRequestId(const std::string& requestId);

private:
    void PrepareSend(const TELEMETRY_DATA& telemetryObject) const;
    void SendSerializedTelemetry(const std::string& serialized, const std::string& requestId) const;
    TELEMETRY_DATA* GetTelemetryForRequestId(const std::string& request_id) const;

    PTELEMETRY_WRITE m_callback;
    void* m_context;
    std::unique_ptr<TELEMETRY_DATA> m_current_telemetry_object;

    std::unordered_map<std::string, std::unique_ptr<TELEMETRY_DATA>> m_telemetry_object_map;
    std::queue<std::unique_ptr<TELEMETRY_DATA>> m_inband_telemetry_queue;
    std::mutex m_lock;
};

typedef enum _METRIC_ID
{
#define DEFINE_METRIC_ID(__id) METRIC_ID_ ## __id,
#include "metricids.h"
#undef DEFINE_METRIC_ID
    METRIC_ID_MAX
} METRIC_ID;

/* These could be inline once we go C++17 */
namespace event
{
    namespace keys
    {
        // Top level Array keys for events
        namespace array
        {
            static const std::string ReceivedMessages = "ReceivedMessages";
            static const std::string Metrics = "Metrics";
        }

        // Received Message event Keys
        namespace received
        {
            static const std::string Audio = "audio";
            static const std::string Response = "response";
        }

        static const std::string Value = "value";
        static const std::string EventType = "EventType";
        static const std::string Name = "name";
        static const std::string Start = "Start";
        static const std::string End = "End";
        static const std::string DeviceId = "DeviceId";
        static const std::string Id = "Id";
        static const std::string Memory = "Memory";
        static const std::string CPU = "CPU";
        static const std::string Error = "Error";
        static const std::string Status = "Status";
    }

    // Metric event name keys
    namespace name
    {
        static const std::string AudioPlayback = "audio:playback";
        static const std::string AudioStart = "AudioStart";
        static const std::string Microphone = "Microphone";
        static const std::string ListeningTrigger = "ListeningTrigger";
        static const std::string Connection = "Connection";
        static const std::string Device = "device";
        static const std::string Notification = "notification";
        static const std::string SDK = "sdk";

    }
}


// Received the specified message from the service.
inline void MetricsReceivedMessage(Telemetry& telemetry, const std::string& requestId, const std::string& x)
{
    telemetry.RecordReceivedMsg(requestId, x);
}

// Metric Events defined in telemetry spec
inline void MetricsAudioStart(Telemetry& telemetry, const std::string& requestId)
{
    telemetry.InbandEventTimestampPopulate(requestId, event::name::Microphone, std::string{}, event::keys::Start);
}

inline void MetricsAudioEnd(Telemetry& telemetry, const std::string& requestId)
{
    telemetry.InbandEventTimestampPopulate(requestId, event::name::Microphone, std::string{}, event::keys::End);
}

inline void MetricsTransportStart(Telemetry& telemetry, const std::string& connectionId)
{
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::Start, nlohmann::json{});
}

inline void MetricsDeviceStartup(Telemetry& telemetry, const std::string& connectionId, const std::string& deviceId)
{
    nlohmann::json value{ deviceId };
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::DeviceId, value);
}

inline void MetricsTransportConnected(Telemetry& telemetry, const std::string& connectionId)
{
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::End, nlohmann::json{});
}

inline void MetricsTransportError(Telemetry& telemetry, const std::string& connectionId, double error)
{
    nlohmann::json value{ error };
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::Error, value);
}

// Transport error metrics

// errors that aren't the same as the 0x8XXXXXXX SDK errors
enum class MetricMessageType : int
{
    INTERNAL_TRANSPORT_ERROR_PARSEERROR = -1,
    INTERNAL_TRANSPORT_UNHANDLEDRESPONSE = -2,
    INTERNAL_TRANSPORT_INVALIDSTATE = -3,
    INTERNAL_CORTANA_EVENT_COALESCED = -4,
    METRIC_MESSAGE_TYPE_DEVICECONTEXT = 1,
    METRIC_MESSAGE_TYPE_AUDIO_START = 2,
    METRIC_MESSAGE_TYPE_AUDIO_LAST = 3,
    METRIC_MESSAGE_TYPE_TELEMETRY = 4,
    METRIC_TRANSPORT_STATE_DNS = 5,
    METRIC_TRANSPORT_STATE_DROPPED = 6,
    METRIC_TRANSPORT_STATE_CLOSED = 7,
    METRIC_TRANSPORT_STATE_CANCELLED = 8,
    METRIC_TRANSPORT_STATE_RESET = 9
};

constexpr uint8_t METRIC_MESSAGE_TYPE_INVALID = 0xff;


// Transport metrics
/* The transport has started a state transition. */
template<typename T>
inline void MetricsTransportStateStart(T/* state*/)
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (to one DS)*/
}

/* The transport has completed a state transition. */
template<typename T>
inline void MetricsTransportStateEnd(T/* state*/)
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (to one DS)*/
}

/* Client request identifier for the current turn */
inline void MetricsTransportRequestId(Telemetry* handle, const std::string& requestId)
{
    handle->RegisterNewRequestId(requestId);
}

inline void MetricsTransportCancelled()
{
    MetricsTransportStateEnd(MetricMessageType::METRIC_TRANSPORT_STATE_CANCELLED);
}

/* The transport connection was closed. */
inline void MetricsTransportClosed()
{
    MetricsTransportStateEnd(MetricMessageType::METRIC_TRANSPORT_STATE_CLOSED);
}

/* The transport connection was dropped. */
inline void MetricsTransportDropped()
{
    MetricsTransportStateEnd(MetricMessageType::METRIC_TRANSPORT_STATE_DROPPED);
}

/* The transport connection was reset.  We do this when the token changes
 * during the connection. */
inline void MetricsTransportReset()
{
    MetricsTransportStateEnd(MetricMessageType::METRIC_TRANSPORT_STATE_RESET);
}

/* The transport has failed to parse a response. */
inline void MetricsTransportParsingError()
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (maybe to 1DS?)*/
}

/* The transport has dropped a packet because it was in a closed state. */
inline void MetricsTransportInvalidStateError()
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (maybe to 1DS?)*/
}

/* Web socket response contained a request ID different from our last request */
inline void MetricsUnexpectedRequestId(const std::string&/* requestid*/)
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (maybe to 1DS?)*/
}

/* audio stream events*/
inline void MetricsAudioStreamInit()
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (maybe to 1DS?)*/
}

inline void MetricsAudioStreamFlush()
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (maybe to 1DS?)*/
}

template<typename T>
inline void MetricsAudioStreamData(T /*__i0*/)
{
    /* Implementation was a convoluted NoOp, leaving here to discuss the possibility of rewiring in the future (maybe to 1DS?)*/
}

} } } }
