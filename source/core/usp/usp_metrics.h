//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// usp_metrics.h: The metrics implementation that uses the USP web socket to send telemetry events
//

#pragma once

#include <queue>
#include <mutex>
#include <i_telemetry.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

/**
 * The PTELEMETRY_WRITE type represents an application-defined
 * callback function used to handle raw telemetry data.
 * @param data The telemetry string data to write
 * @param requestId The request ID for the current turn
 */
using PTELEMETRY_WRITE = std::function<void(std::string&& data, const std::string& requestId)>;


struct TELEMETRY_DATA
{
    std::string requestId;
    bool bPayloadSet{};
    // TODO: This does not make sense, why telemetry does not have
    // a proper set inside?
    std::array<nlohmann::json, countOfMsgTypes> receivedMsgs;

    nlohmann::json connectionJson{};
    nlohmann::json audioStartJson{};
    nlohmann::json microphoneJson{};
    nlohmann::json listeningTriggerJson{};
    nlohmann::json ttsJson{};
    nlohmann::json deviceJson{};
    nlohmann::json phraseLatencyJson{};
    nlohmann::json firstHypothesisLatencyJson{};
};


/**
 * Implementation that uses a USP web socket connection to send telemetry events
 */
struct Telemetry : public ITelemetry
{
    /**
    * Creates a object handle to maintain telemetry state, initializes the internal structures and
    * sets the application defined callbacks used for uploading in-band telemetry events.
    * @param callback The callback method to use
    */
    Telemetry(PTELEMETRY_WRITE callback);
    virtual ~Telemetry();

    /**
     * Flushes any outstanding telemetry events. Also flushes the telemetry associated with a given requestId.
     * @param requestId used to specify what telemetry to flush
     */
    void Flush(const std::string& requestId) override;

    /**
     * Timestamps and records telemetry event.
     * @param requestId The requestId associated with the current event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    void InbandEventTimestampPopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key) override;

    /**
     *  Records connection telemetry event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    void InbandConnectionTelemetry(const std::string& id, const std::string& key, const nlohmann::json& value) override;

    /**
     * Populates and records telemetry event.
     * @param requestId The requestId associated with the current event.
     * @param eventName The name of the event to be recorded.
     * @param id The unique id of the event to be recorded.
     * @param key The key of the JSON element to be recorded.
     * @param value The value of the JSON element to be recorded.
     */
    void InbandEventKeyValuePopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key, const nlohmann::json& ) override;

    /**
     * Received metric event population function.
     * @param requestId The requestId associated with the received message.
     * @param receivedMsg The name of the received event from service.
     */
    void RecordReceivedMsg(const std::string& requestId, const std::string&receivedMsg) override;

    /**
     * Result latency population function.
     * @param requestId The requestId associated with the received message.
     * @param latencyInTicks The latency value in ticks.
     * @param isPhraseLatency If it is true, the latency is for phrase result. Otherwise it is for hypothesis result.
     */
    void RecordResultLatency(const std::string& requestId, uint64_t latencyInTicks, bool isPhraseLatency) override;

    /**
     * Handles the necessary changes for a requestId change event.
     * @param requestId the currently active requestId.
     */
    void RegisterNewRequestId(const std::string& requestId) override;

private:
    void PrepareSend(const TELEMETRY_DATA& telemetryObject) const;
    void SendSerializedTelemetry(std::string&& serialized, const std::string& requestId) const;
    TELEMETRY_DATA* GetTelemetryForRequestId(const std::string& request_id) const;

    PTELEMETRY_WRITE m_callback;
    std::unique_ptr<TELEMETRY_DATA> m_current_telemetry_object;

    std::unordered_map<std::string, std::unique_ptr<TELEMETRY_DATA>> m_telemetry_object_map;
    std::queue<std::unique_ptr<TELEMETRY_DATA>> m_inband_telemetry_queue;
    std::mutex m_lock;
};

} } } }
