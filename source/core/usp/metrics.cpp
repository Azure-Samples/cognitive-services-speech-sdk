//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// metrics.h: send and process telemetry messages.
//
#include "metrics.h"
#include "uspcommon.h"

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <tuple>
#include <time.h>

#include "azure_c_shared_utility_tickcounter_wrapper.h"
#include "azure_c_shared_utility_xlogging_wrapper.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

constexpr std::size_t MaxMessagesToRecord{ 50 };

const std::array<std::tuple<IncomingMsgType, const std::string*>, static_cast<size_t>(countOfMsgTypes)> message_mappings{ {
    std::make_tuple(turnStart, &path::turnStart),
    std::make_tuple(turnEnd, &path::turnEnd),
    std::make_tuple(speechStartDetected, &path::speechStartDetected),
    std::make_tuple(speechEndDetected, &path::speechEndDetected),
    std::make_tuple(speechHypothesis, &path::speechHypothesis),
    std::make_tuple(speechFragment, &path::speechFragment),
    std::make_tuple(speechPhrase, &path::speechPhrase),
    std::make_tuple(translationHypothesis, &path::translationHypothesis),
    std::make_tuple(translationPhrase, &path::translationPhrase),
    std::make_tuple(translationSynthesis, &path::translationSynthesis),
    std::make_tuple(translationSynthesisEnd, &path::translationSynthesisEnd),
    std::make_tuple(audio, &event::keys::received::Audio),
    std::make_tuple(response, &event::keys::received::Response)
} };

const std::string* get_message_name(const IncomingMsgType type)
{
    /* No point in using a map to such a small collection, loss of locality and heap copies probably worse than O(n) vs O(log(n)) */
    for (auto& m : message_mappings)
    {
        if (std::get<0>(m) == type)
        {
            return std::get<1>(m);
        }
    }
    return nullptr;
}

IncomingMsgType message_from_name(const std::string& name)
{
    for (const auto& m : message_mappings)
    {
        const auto& n = *std::get<1>(m);
        if (name == n)
        {
            return std::get<0>(m);
        }
    }
    return countOfMsgTypes;
}

static nlohmann::json PropertybagInitializeWithKeyValue(const std::string& key, const nlohmann::json& value)
{
    nlohmann::json pb;
    if (!key.empty())
    {
        pb[key] = value;
    }
    return pb;
}

static nlohmann::json& initialize_jsonArray(nlohmann::json& array)
{
    return array.is_array() ? array : (array = nlohmann::json::array());
}

namespace MetricObjectKeys
{
    const std::string Name{ "Name" };
    const std::string Id{ "Id" };
}

static void populate_metric_object(nlohmann::json& object, const std::string& eventName, const std::string& id)
{
    if (object.is_null())
    {
        object = nlohmann::json{};
        if (!eventName.empty())
        {
            object[MetricObjectKeys::Name] = eventName;
        }
        if (!id.empty())
        {
            object[MetricObjectKeys::Id] = id;
        }
    }
}

int GetISO8601Time(char *buffer, unsigned int length)
{
    // TODO: refactor using azure-c-shared functions.
    if (length < TIME_STRING_MAX_SIZE)
    {
        return -1;
    }
    size_t timeStringLength = 0;
    time_t rawtime;
    struct tm timeinfo;

    time(&rawtime);
#ifdef _MSC_VER
    gmtime_s(&timeinfo, &rawtime);
#else
    gmtime_r(&rawtime, &timeinfo);
#endif

    timeStringLength += strftime(buffer, length, "%FT%T.", &timeinfo);

#if defined(WIN32)
    SYSTEMTIME sysTime;
    GetSystemTime(&sysTime);
    timeStringLength += snprintf(buffer + 20, 5, "%03uZ", sysTime.wMilliseconds);
#else
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
#ifdef __MACH__  // Ugh!
    timeStringLength += snprintf(buffer + 20, 5, "%03dZ", (curTime.tv_usec / 1000));
#else
    timeStringLength += snprintf(buffer + 20, 5, "%03ldZ", (curTime.tv_usec / 1000));
#endif
#endif

    return (int)timeStringLength;
}

static bool populate_event_timestamp(nlohmann::json& object, const std::string& eventName, const std::string& id, const std::string& key)
{
    if (eventName.empty() || id.empty())
    {
        return false;
    }
    populate_metric_object(object, eventName, id);
    if (object.is_null())
    {
        return false;
    }
    char timeString[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
    {
        return false;
    }
    object[key] = std::string{ timeString };
    return true;
}

bool populate_event_key_value(nlohmann::json& pBag, const std::string& eventName, const std::string& id, const std::string& key, const nlohmann::json& value)
{
    if (eventName.empty())
    {
        LogError("Telemetry: event name is empty.");
        return false;
    }
    if (key.empty())
    {
        LogError("Telemetry: key name is empty.");
        return false;
    }

    populate_metric_object(pBag, eventName, id);
    if (pBag.is_null())
    {
        return false;
    }

    pBag[key] = value;

    return true;
}

/* This will be inlined by the compiler */
bool push_if_not_null(nlohmann::json& array, const nlohmann::json& object)
{
    if (object.is_null())
    {
        return false;
    }
    array.push_back(object);
    return true;
}

static nlohmann::json telemetry_add_metricevents(const TELEMETRY_DATA& telemetry_object)
{
    nlohmann::json json_array = nlohmann::json::array();

    if (push_if_not_null(json_array, telemetry_object.connectionJson))
    {
        // deviceJson should only be relevant when we have a connectionJson
        push_if_not_null(json_array, telemetry_object.deviceJson);

        // we can return here because when we have a connectionJson we know
       // all of the jsons besides deviceJson and connectionJson are not relevant
        return json_array;
    }

    push_if_not_null(json_array, telemetry_object.audioStartJson);
    push_if_not_null(json_array, telemetry_object.microphoneJson);
    push_if_not_null(json_array, telemetry_object.listeningTriggerJson);
    push_if_not_null(json_array, telemetry_object.ttsJson);
    if (telemetry_object.phraseLatencyJson != nullptr)
    {
        auto recvObj = PropertybagInitializeWithKeyValue(event::name::PhraseLatency, telemetry_object.phraseLatencyJson);
        json_array.push_back(recvObj);
    }
    if (telemetry_object.hypothesisLatencyJson != nullptr)
    {
        auto recvObj = PropertybagInitializeWithKeyValue(event::name::HypothesisLatency, telemetry_object.hypothesisLatencyJson);
        json_array.push_back(recvObj);
    }

    return json_array;
}


static nlohmann::json* getJsonForEvent(TELEMETRY_DATA* telemetryObject, const std::string& eventName)
{
    if (eventName == event::name::AudioStart)
        return &telemetryObject->audioStartJson;
    if (eventName == event::name::Microphone)
        return &telemetryObject->microphoneJson;
    if (eventName == event::name::AudioPlayback)
        return &telemetryObject->ttsJson;

    LogError("Telemetry: invalid event name (%s)", eventName.c_str());
    return nullptr;
}

void Telemetry::RegisterNewRequestId(const std::string& requestId)
{
    if (requestId.empty())
    {
        LogError("Telemetry: empty request id");
        return;
    }

    std::lock_guard<std::mutex> lk{ m_lock };

    if (!GetTelemetryForRequestId(requestId))
    {
        auto telemetry_data = std::make_unique<TELEMETRY_DATA>();
        telemetry_data->requestId = requestId;
        m_telemetry_object_map.emplace(requestId, std::move(telemetry_data));
    }
    else
    {
        LogError("Telemetry: Attempting to register an already registered requestId: %s", requestId.c_str());
    }
}

void Telemetry::InbandEventKeyValuePopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key, const nlohmann::json& value)
{
    std::lock_guard<std::mutex> lk{ m_lock };
    auto telemetry_data = GetTelemetryForRequestId(requestId);
    if (telemetry_data != nullptr)
    {
        auto pBag = getJsonForEvent(telemetry_data, eventName);
        if (pBag != nullptr)
        {
            // Set the bPayloadSet flag
            telemetry_data->bPayloadSet |= populate_event_key_value(*pBag, eventName, id, key, value);
        }
    }
    else
    {
        LogError("Telemetry: received unexpected requestId: (%s).", requestId.c_str());
    }
}

void Telemetry::InbandConnectionTelemetry(const std::string& connectionId, const std::string& key, const nlohmann::json& value)
{
    std::lock_guard<std::mutex> lk{ m_lock };
    TELEMETRY_DATA* connection_data = m_current_telemetry_object.get();
    auto& connectionJson = connection_data->connectionJson;
    const std::string& eventName = event::name::Connection;
    if (event::keys::Start == key)
    {
        connection_data->bPayloadSet = populate_event_timestamp(connectionJson, eventName, connectionId, event::keys::Start);
    }
    else if (event::keys::DeviceId == key)
    {
        connection_data->bPayloadSet = populate_event_key_value(connection_data->deviceJson, event::name::Device, std::string{}, key, value);
    }
    else
    {
        connection_data->bPayloadSet &= value.is_null() ?
            populate_event_timestamp(connectionJson, eventName, connectionId, event::keys::Start) :
            populate_event_key_value(connectionJson, eventName, connectionId, key, value);

        if (connection_data->bPayloadSet)
        {
            m_inband_telemetry_queue.push(std::move(m_current_telemetry_object));
        }

        // Assign new memory with zeros to current telemetry object
        m_current_telemetry_object = std::make_unique<TELEMETRY_DATA>();
    }
}

void Telemetry::InbandEventTimestampPopulate(const std::string& requestId, const std::string& eventName, const std::string& id, const std::string& key)
{
    std::lock_guard<std::mutex> lk{ m_lock };
    auto telemetry_data = GetTelemetryForRequestId(requestId);
    if (telemetry_data != nullptr)
    {
        auto* pBag = getJsonForEvent(telemetry_data, eventName);
        if (pBag != nullptr)
        {
            // Set the bPayloadSet flag
            telemetry_data->bPayloadSet |= populate_event_timestamp(*pBag, eventName, id, key);
        }
    }
    else
    {
        LogError("Telemetry: received unexpected requestId: (%s).", requestId.c_str());
    }
}

static nlohmann::json telemetry_add_recvmsgs(const TELEMETRY_DATA& telemetry_object)
{
    nlohmann::json json_array = nlohmann::json::array();

    for (int i = 0; i < countOfMsgTypes; i++)
    {
        if (!telemetry_object.receivedMsgs[i].is_null())
        {
            auto recvObj = PropertybagInitializeWithKeyValue(*get_message_name(static_cast<IncomingMsgType>(i)), telemetry_object.receivedMsgs[i]);
            json_array.push_back(recvObj);
        }
    }
    return json_array;
}


int telemetry_serialize(nlohmann::json& root, const TELEMETRY_DATA& telemetry_object)
{
    // Root Object
    // Create and append all received events
    root[event::keys::array::ReceivedMessages] = telemetry_add_recvmsgs(telemetry_object);

    // append all metric events
    root[event::keys::array::Metrics] = telemetry_add_metricevents(telemetry_object);
    return 0;
}


Telemetry::Telemetry(PTELEMETRY_WRITE callback, void* context) : m_callback{ callback }, m_context{ context }, m_current_telemetry_object{ std::make_unique<TELEMETRY_DATA>() }
{
}

Telemetry::~Telemetry()
{
}

void Telemetry::Flush(const std::string& requestId)
{
    std::lock_guard<std::mutex> lk{ m_lock };
    // Check if events exist in queue. If yes, flush them out first.
    while (!m_inband_telemetry_queue.empty())
    {
        auto&& item = m_inband_telemetry_queue.front();
        // Telemetry messages need a requestId, so use the one we have if the object in queue doesn't have one
        if (item->requestId.empty())
        {
            item->requestId = requestId;
        }
        PrepareSend(*item);
        m_inband_telemetry_queue.pop();
    }
    auto telemetry_object = GetTelemetryForRequestId(requestId);
    if (telemetry_object != nullptr)
    {
        PrepareSend(*telemetry_object);
        m_telemetry_object_map.erase(requestId);
    }
    else
    {
        LogError("Telemetry: received unexpected requestId: (%s).", requestId.c_str());
    }
}

void Telemetry::PrepareSend(const TELEMETRY_DATA& telemetryObject) const
{
    std::string requestId{};
    // Get request ID from telemetry object if any
    if (!telemetryObject.requestId.empty())
    {
        requestId = telemetryObject.requestId;
    }
    nlohmann::json root;
    if (telemetry_serialize(root, telemetryObject) == 0)
    {
        auto serialized = root.dump();
        SendSerializedTelemetry(serialized, requestId);
    }
}

void Telemetry::SendSerializedTelemetry(const std::string& serialized, const std::string& requestId) const
{
    if (!serialized.empty())
    {
        // Serialize the received messages events and metric events.
        if (m_callback)
        {
            LogInfo("%s: Send telemetry (requestId:%s): %s", __FUNCTION__, requestId.c_str(), serialized.c_str());
            m_callback(reinterpret_cast<const uint8_t*>(serialized.c_str()), serialized.size(), m_context, requestId.c_str());
        }
    }
}

void Telemetry::RecordReceivedMsg(const std::string& requestId, const std::string& messagePath)
{
    if (messagePath.empty())
    {
        LogError("Telemetry: received an empty message.");
        return;
    }

    char timeString[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
    {
        return;
    }

    IncomingMsgType msgType = message_from_name(messagePath);

    if (msgType == countOfMsgTypes)
    {
        LogError("Telemetry: received unexpected msg: (%s).", messagePath.c_str());
        return;
    }

    std::lock_guard<std::mutex> lk{ m_lock };
    auto telemetry_object = GetTelemetryForRequestId(requestId);
    if (telemetry_object != nullptr)
    {
        auto& telemetry_data = m_telemetry_object_map[requestId];
        auto& evArray = initialize_jsonArray(telemetry_data->receivedMsgs[static_cast<size_t>(msgType)]);
        // If we reach the max number of messages, drop it.
        if (evArray.size() < MaxMessagesToRecord)
        {
            evArray.push_back(timeString);
        }
    }
    else
    {
        LogError("Telemetry: received unexpected requestId: (%s).", requestId.c_str());
    }
}

void Telemetry::RecordResultLatency(const std::string& requestId, uint64_t latencyInTicks, bool isPhraseLatency)
{
    std::lock_guard<std::mutex> lk{ m_lock };
    auto telemetry_object = GetTelemetryForRequestId(requestId);
    if (telemetry_object != nullptr)
    {
        auto& telemetry_data = m_telemetry_object_map[requestId];
        assert(telemetry_object == telemetry_data.get());
        auto& resultLatencyJson = isPhraseLatency ? telemetry_data->phraseLatencyJson : telemetry_data->hypothesisLatencyJson;
        auto& evArray = initialize_jsonArray(resultLatencyJson);
        /* If we reach the max number of messages, drop it */
        if (evArray.size() < MaxMessagesToRecord)
        {
            evArray.push_back(latencyInTicks);
        }
    }
    else
    {
        LogError("%s: Telemetry for %s: received unexpected requestId: (%s).", __FUNCTION__, isPhraseLatency ? "phrase" : "hypothesis", requestId.c_str());
    }
}

TELEMETRY_DATA* Telemetry::GetTelemetryForRequestId(const std::string& request_id) const
{
    const auto it = m_telemetry_object_map.find(request_id);
    if (it != m_telemetry_object_map.end())
    {
        return std::get<1>(*it).get();
    }
    return nullptr;
}

} } } }
