//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// i_telemetry.h: Interfaces and types for telemetry
//

#pragma once

#include <string>
#include <json.h>
#include <i_web_socket.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

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
    audioMetadata,
    response,
    audioStart,
    audioEnd,
    translationResponse,
    countOfMsgTypes
};

// errors that aren't the same as the 0x8XXXXXXX SDK errors



/**
 * The interface to implement to handle the sending of telemetry events
 */
struct ITelemetry
{
    /**
     * Destructor
     */
    virtual ~ITelemetry() {}

    /**
     * Flushes any outstanding telemetry events. Also flushes the telemetry associated with a given requestId.
     * @param requestId used to specify what telemetry to flush
     */
    virtual void Flush(const std::string& requestId) = 0;

    /**
     * Timestamps and records telemetry event.
     * @param requestId The requestId associated with the current event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    virtual void InbandEventTimestampPopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key) = 0;

    /**
     *  Records connection telemetry event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    virtual void InbandConnectionTelemetry(const std::string& id, const std::string& key, const nlohmann::json& value) = 0;

    /**
     * Populates and records telemetry event.
     * @param requestId The requestId associated with the current event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    virtual void InbandEventKeyValuePopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key, const nlohmann::json&) = 0;

    /**
     * Received metric event population function.
     * @param requestId The requestId associated with the received message.
     * @param receivedMsg The name of the received event from service.
     */
    virtual void RecordReceivedMsg(const std::string& requestId, const std::string&receivedMsg) = 0;

    /**
     * Result latency population function.
     * @param requestId The requestId associated with the received message.
     * @param latencyInTicks The latency value in ticks.
     * @param isPhraseLatency If it is true, the latency is for phrase result. Otherwise it is for hypothesis result.
     */
    virtual void RecordResultLatency(const std::string& requestId, uint64_t latencyInTicks, bool isPhraseLatency) = 0;

    /**
     * Handles the necessary changes for a requestId change event.
     * @param requestId the currently active requestId.
     */
    virtual void RegisterNewRequestId(const std::string& requestId) = 0;
};


namespace event
{
    namespace keys
    {
        // Top level Array keys for events
        namespace array
        {
            constexpr auto ReceivedMessages = "ReceivedMessages";
            constexpr auto Metrics = "Metrics";
        }

        // Received Message event Keys
        namespace received
        {
            constexpr auto Audio = "audio";
            constexpr auto AudioMetadata = "audio.metadata";
            constexpr auto Response = "response";
        }

        constexpr auto Value = "value";
        constexpr auto EventType = "EventType";
        constexpr auto Name = "name";
        constexpr auto Start = "Start";
        constexpr auto End = "End";
        constexpr auto DeviceId = "DeviceId";
        constexpr auto Id = "Id";
        constexpr auto Memory = "Memory";
        constexpr auto CPU = "CPU";
        constexpr auto Error = "Error";
        constexpr auto Status = "Status";
    }

    // Metric event name keys
    namespace name
    {
        constexpr auto AudioPlayback = "audio:playback";
        constexpr auto AudioStart = "AudioStart";
        constexpr auto Microphone = "Microphone";
        constexpr auto ListeningTrigger = "ListeningTrigger";
        constexpr auto Connection = "Connection";
        constexpr auto Device = "device";
        constexpr auto Notification = "notification";
        constexpr auto SDK = "sdk";
        constexpr auto PhraseLatency = "PhraseLatencyMs";
        constexpr auto FirstHypothesisLatency = "FirstHypothesisLatencyMs";
    }
}

constexpr size_t NO_DASH_UUID_LEN = 37;
constexpr size_t TIME_STRING_MAX_SIZE = 30;

/**
 * Returns the current time in ISO8601 format
 * @param buffer char buffer to hold the ISO8601 time string
 * @param bufferLength the length of the buffer to be used to store the string
 * @return 0 in case of success and -1 in case of failure
 */
int GetISO8601Time(char *buffer, unsigned int bufferLength);


// Received the specified message from the service.
inline void MetricsReceivedMessage(ITelemetry& telemetry, const std::string& requestId, const std::string& messagePath)
{
    telemetry.RecordReceivedMsg(requestId, messagePath);
}

// Metric Events defined in telemetry spec
inline void MetricsAudioStart(ITelemetry& telemetry, const std::string& requestId)
{
    telemetry.InbandEventTimestampPopulate(requestId, event::name::Microphone, std::string{}, event::keys::Start);
}

inline void MetricsAudioEnd(ITelemetry& telemetry, const std::string& requestId)
{
    telemetry.InbandEventTimestampPopulate(requestId, event::name::Microphone, std::string{}, event::keys::End);
}

inline void MetricsResultLatency(ITelemetry& telemetry, const std::string& requestId, uint64_t latencyInTicks, bool isPhraseLatency)
{
    telemetry.RecordResultLatency(requestId, latencyInTicks, isPhraseLatency);
}

inline void MetricsTransportStart(ITelemetry& telemetry, const std::string& connectionId)
{
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::Start, nlohmann::json{});
}

inline void MetricsDeviceStartup(ITelemetry& telemetry, const std::string& connectionId, const std::string& deviceId)
{
    nlohmann::json value{ deviceId };
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::DeviceId, value);
}

inline void MetricsTransportConnected(ITelemetry& telemetry, const std::string& connectionId)
{
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::End, nlohmann::json{});
}

inline void MetricsTransportError(ITelemetry& telemetry, const std::string& connectionId, double error)
{
    nlohmann::json value{ error };
    telemetry.InbandConnectionTelemetry(connectionId, event::keys::Error, value);
}

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
inline void MetricsTransportRequestId(ITelemetry* handle, const std::string& requestId)
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
inline void MetricsUnexpectedRequestId(const std::string&)
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
