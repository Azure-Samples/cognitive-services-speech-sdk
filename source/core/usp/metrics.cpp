//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// metrics.h: send and process telemetry messages.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <map>

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>

#include "uspcommon.h"
#include "metrics.h"
#include "propbag.h"

#include "azure_c_shared_utility_strings_wrapper.h"
#include "azure_c_shared_utility_singlylinkedlist_wrapper.h"
#include "azure_c_shared_utility_lock_wrapper.h"
#include "azure_c_shared_utility_xlogging_wrapper.h"
#include "azure_c_shared_utility_crt_abstractions_wrapper.h"
#include "azure_c_shared_utility_tickcounter_wrapper.h"
#include "azure_c_shared_utility_agenttime_wrapper.h"

const char* kEvent_type_key = "EventType";
const char* kRcvd_msgs_key = "ReceivedMessages";
const char* kMetric_events_key = "Metrics";

const char* kEvent_type_audioPlayback = "audio:playback";
const char* kEvent_type_audioStart = "AudioStart";
const char* kEvent_type_microphone = "Microphone";
const char* kEvent_type_listeningTrigger = "ListeningTrigger";
const char* kEvent_type_connection = "Connection";
const char* kEvent_type_device = "device";
const char* kEvent_type_notification = "notification";
const char* kEvent_type_sdk = "sdk";
const char* kEvent_value = "value";
const char* kEvent_name_key = "name";
const char* kEvent_start_key = "Start";
const char* kEvent_end_key = "End";
const char* kEvent_id_key = "Id";
const char* kEvent_memory_key = "Memory";
const char* kEvent_cpu_key = "CPU";
const char* kEvent_error_key = "Error";
const char* kEvent_status_key = "Status";

const char *kRcvd_msg_audio_key = "audio";
const char *kRcvd_msg_response_key = "response";

static std::map<IncomingMsgType, std::string> messageIndexToName;

#define GOTO_EXIT_IF_NONZERO(exp) { retval = exp; if (retval != 0) { goto Exit; } }
#define RESET(val)  val = NULL

#pragma pack(push, 1)

using namespace Microsoft::CognitiveServices::Speech::USP;

typedef struct _TELEMETRY_CONTEXT
{
    LOCK_HANDLE lock;
    PTELEMETRY_WRITE callback;
    void* callbackContext;
    SINGLYLINKEDLIST_HANDLE inband_telemetry_queue;
    TELEMETRY_DATA *current_connection_telemetry_object;
    TELEMETRY_DATA *current_telemetry_object;
} TELEMETRY_CONTEXT;

static void initialize_message_name_array()
{
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

    messageIndexToName = std::map<IncomingMsgType, std::string>
    {
        { turnStart, path::turnStart },
        { turnEnd, path::turnEnd },
        { speechStartDetected, path::speechStartDetected },
        { speechEndDetected, path::speechEndDetected },
        { speechHypothesis, path::speechHypothesis},
        { speechFragment, path::speechFragment},
        { speechPhrase, path::speechPhrase},
        { translationHypothesis, path::translationHypothesis},
        { translationPhrase, path::translationPhrase},
        { translationSynthesis, path::translationSynthesis},
        { translationSynthesisEnd, path::translationSynthesisEnd},
        { audio, kRcvd_msg_audio_key},
        { response, kRcvd_msg_response_key}
    };
}

static void PropertybagAddValueToArray(PROPERTYBAG_HANDLE propertyHandle, void * value)
{
    if (propertyHandle && value)
    {
        json_array_append_value(json_array((JSON_Value*)propertyHandle), (JSON_Value *)value);
    }
}

static void PropertybagSetArrayValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, void* value)
{
    if (propertyHandle && name)
    {
        json_object_set_value((JSON_Object*)propertyHandle, name, (JSON_Value *)value);
    }
}

static int PropertybagSetValue(PROPERTYBAG_HANDLE propertyHandle, const char* name, void* value)
{
    if (propertyHandle && name)
    {
        switch (json_value_get_type((JSON_Value*)value))
        {
        case JSONNumber:
        {
            PropertybagSetNumberValue(json_object((JSON_Value*)propertyHandle), name, json_value_get_number((JSON_Value*)value));
            return 0;
        }
        case JSONBoolean:
        {
            PropertybagSetBooleanValue(json_object((JSON_Value*)propertyHandle), name, json_value_get_boolean((JSON_Value*)value));
            return 0;
        }
        case JSONString:
        {
            PropertybagSetStringValue(json_object((JSON_Value*)propertyHandle), name, json_value_get_string((JSON_Value*)value));
            return 0;
        }
        case JSONArray:
        {
            PropertybagSetArrayValue(json_object((JSON_Value*)propertyHandle), name, (JSON_Value*)value);
            return 0;
        }
        default:
            return -1;
        }
    }

    return -1;
}

static PROPERTYBAG_HANDLE PropertybagInitializeWithKeyValue(const char *key,    void *value)
{
    PROPERTYBAG_HANDLE propertyHandle = json_value_init_object();
    if (propertyHandle && key)
    {
        PropertybagSetValue(propertyHandle, key, (JSON_Value *)value);
    }

    return propertyHandle;
}

static PROPERTYBAG_HANDLE initialize_jsonArray(PROPERTYBAG_HANDLE *pArray)
{
    return *pArray == NULL ? (*pArray = json_value_init_array()) : *pArray;
}

static PROPERTYBAG_HANDLE initialize_metricobject(const char *name, const char *id)
{
    PROPERTYBAG_HANDLE pBag = PropertybagInitializeWithKeyValue(NULL, NULL);
    if (pBag && name)
    {
        PropertybagSetStringValue(json_object((JSON_Value*)pBag), "Name", name);
        if (id)
        {
            PropertybagSetStringValue(json_object((JSON_Value*)pBag), "Id", id);
        }
    }

    return pBag;
}

static PROPERTYBAG_HANDLE get_metric_object(PROPERTYBAG_HANDLE pBag, const char *eventName, const char *id)
{
    return pBag ? pBag : initialize_metricobject(eventName, id);
}

int GetISO8601TimeOffset(char *buffer, unsigned int length, int offset)
{
    if (length < TIME_STRING_MAX_SIZE)
    {
        return -1;
    }

// TODO: get rid of WIN32 branch, else branch should work for 
// all platforms.
#if defined(WIN32)
    struct tm timeinfo;
    SYSTEMTIME stCurrentTime = { 0 };
    FILETIME ftCurrentTime = { 0 };

    // Get current time and store it in a ULARGE_INTEGER
    GetSystemTime(&stCurrentTime);
    SystemTimeToFileTime(&stCurrentTime, &ftCurrentTime);
    ULARGE_INTEGER uCurrentTime = { 0 };
    memcpy(&uCurrentTime, &ftCurrentTime, sizeof(uCurrentTime));

    // Adjust current time with the computed offset
    uCurrentTime.QuadPart -= (offset * 10000);

    // Convert adjusted Time to SYSTEMTIME
    memcpy(&ftCurrentTime, &uCurrentTime, sizeof(ftCurrentTime));
    SYSTEMTIME stCurrentTimeAfterOffset = { 0 };
    FileTimeToSystemTime(&ftCurrentTime, &stCurrentTimeAfterOffset);

    memset(&timeinfo, 0, sizeof(struct tm));

    timeinfo.tm_mday = stCurrentTimeAfterOffset.wDay;
    timeinfo.tm_mon = stCurrentTimeAfterOffset.wMonth - 1;
    timeinfo.tm_year = stCurrentTimeAfterOffset.wYear - 1900;

    timeinfo.tm_sec = stCurrentTimeAfterOffset.wSecond;
    timeinfo.tm_min = stCurrentTimeAfterOffset.wMinute;
    timeinfo.tm_hour = stCurrentTimeAfterOffset.wHour;
#else
    TICK_COUNTER_HANDLE tickHandle = tickcounter_create();

    int result;
    tickcounter_ms_t current_ms;
    result = tickcounter_get_current_ms(tickHandle, &current_ms);

    tickcounter_destroy(tickHandle);

    if (result != 0)
    {
        LogError("Unable to determine current system Time");
        return -1;
    }

    current_ms -= offset;

    time_t adjusted_seconds = (time_t)(current_ms / 1000);
    int adjusted_ms = current_ms % 1000;

    struct tm* timeinfo = get_gmtime(&(adjusted_seconds));

    if (timeinfo == NULL)
    {
        return -1;
    }
#endif

#if defined(WIN32)
    strftime(buffer, length, "%FT%T.", &timeinfo);
    (void) snprintf(buffer + 20, 5, "%03uZ", stCurrentTimeAfterOffset.wMilliseconds);
#else
    strftime(buffer, length, "%FT%T.", timeinfo);
    (void) snprintf(buffer + 20, 5, "%03dZ", adjusted_ms);
#endif
    return 0;
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
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);

    timeStringLength += strftime(buffer, length, "%FT%T.", timeinfo);

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


static int populate_event_timestamp(PROPERTYBAG_HANDLE *pBag, const char *eventName, const char *id, const char *key)
{
    *pBag = get_metric_object(*pBag, eventName, id);
    if (*pBag == NULL)
    {
        return -1;
    }

    char timeString[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
    {
        return -1;
    }

    PropertybagSetStringValue(json_object((JSON_Value*)*pBag), key, timeString);

    return 0;
}

int populate_event_key_value(PROPERTYBAG_HANDLE *pBag, const char *eventName, const char *id, const char *key, void *value)
{
    int ret = 0;

    if (IS_STRING_NULL_OR_EMPTY(eventName))
    {
        LogError("Telemetry: event name is empty.\r\n");
        return -1;
    }
    if (IS_STRING_NULL_OR_EMPTY(key))
    {
        LogError("Telemetry: key name is empty\r\n");
        return -1;
    }

    *pBag = get_metric_object(*pBag, eventName, id);
    if (*pBag == NULL)
    {
        return -1;
    }

    ret = PropertybagSetValue(*pBag, key, value);
    if (ret)
    {
        LogInfo("Telemetry: Failed to add to new event object (%s) with key: %s.\r\n", eventName, key);
        return -1;
    }

    return 0;
}

static PROPERTYBAG_HANDLE telemetry_add_metricevents(TELEMETRY_DATA *telemetry_object)
{

    PROPERTYBAG_HANDLE json_array = json_value_init_array();
    if (NULL == json_array)
    {
        LogInfo("Telemetry: Unable to initialize Metrics event array\r\n");
        return NULL;
    }

    if (telemetry_object->connectionJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->connectionJson);
        // current_connection_telemetry_object would be the only one that would have
        // connectionJson initialized. If connectionJson is initialized for 
        // any TELEMETRY_DATA object, they would not contain values. 
        // Hence no need to check other pointers and we can return here.
        return json_array;
    }

    if (telemetry_object->audioStartJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->audioStartJson);
    }

    if (telemetry_object->microphoneJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->microphoneJson);
    }

    if (telemetry_object->listeningTriggerJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->listeningTriggerJson);
    }

    if (telemetry_object->ttsJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->ttsJson);
    }

    if (telemetry_object->ttsJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->ttsJson);
    }

    if (telemetry_object->deviceJson)
    {
        PropertybagAddValueToArray(json_array, telemetry_object->deviceJson);
    }

    return json_array;
}

static PROPERTYBAG_HANDLE telemetry_add_recvmsgs(TELEMETRY_DATA *telemetry_object)
{
    if (NULL == telemetry_object)
        return NULL;

    PROPERTYBAG_HANDLE json_array = json_value_init_array();
    if (NULL == json_array)
    {
        LogInfo("Telemetry: Unable to initialize Received Messages array\r\n");
        return NULL;
    }

    for (int i = 0; i < countOfMsgTypes; i++)
    {
        PROPERTYBAG_HANDLE recvObj = NULL;
        if (telemetry_object->receivedMsgsJsonArray[i])
        {
            recvObj = PropertybagInitializeWithKeyValue(messageIndexToName[(IncomingMsgType)i].c_str(), telemetry_object->receivedMsgsJsonArray[i]);
            PropertybagAddValueToArray(json_array, recvObj);
        }
    }
    return json_array;
}

int telemetry_serialize(PROPERTYBAG_HANDLE root, void* pContext)
{
    if (!root) {
        return -1;
    }

    TELEMETRY_DATA *telemetry_object = (TELEMETRY_DATA *)pContext;
    // Root Object
    // Create and append all received events
    PROPERTYBAG_HANDLE received_msg_array = telemetry_add_recvmsgs(telemetry_object);
    PropertybagSetArrayValue(root, kRcvd_msgs_key, received_msg_array);

    // append all metric events
    PROPERTYBAG_HANDLE metric_array = telemetry_add_metricevents(telemetry_object);
    PropertybagSetArrayValue(root, kMetric_events_key, metric_array);
    return 0;
}

int tts_serialize(PROPERTYBAG_HANDLE root, void* pContext)
{
    if (!root || !pContext) {
        return -1;
    }

    PROPERTYBAG_HANDLE json_array = json_value_init_array();
    if (NULL == json_array)
    {
        LogError("Telemetry: Unable to create json array\r\n");
        return -1;
    }

    PropertybagAddValueToArray(json_array, pContext);
    PropertybagSetArrayValue(root, kRcvd_msgs_key, json_value_init_array());
    PropertybagSetArrayValue(root, kMetric_events_key, json_array);

    return 0;
}

static void send_serialized_telemetry(TELEMETRY_HANDLE handle, STRING_HANDLE string,
    const char *requestId)
{
    if (string)
    {
        // Serialize the received messages events and metric events.
        const char * c_str = STRING_c_str(string);
        if (c_str && handle->callback)
        {
            handle->callback((const uint8_t*)c_str, strlen(c_str), handle->callbackContext, requestId);
        }
    }
}

static void tts_flush(TELEMETRY_HANDLE handle, const char *eventName, const char *id, const char *key, void *value)
{
    // Create a JSON object that stores the telemetry events
    PROPERTYBAG_HANDLE *pBag = NULL;

    // IF value is NULL, this is either a TTS_START or a TTS_END event
    // Associate the timestamp with the event and flush it
    int ret;
    if (value == NULL)
    {
        ret = populate_event_timestamp(pBag, eventName, id, key);
    }
    // Case for TTS_ERROR
    else
    {
        ret = populate_event_key_value(pBag, eventName, id, key, value);
    }

    if (ret == 0) 
    {
        // Serialize the received messages events and metric events.
        STRING_HANDLE string = PropertybagSerialize(tts_serialize, pBag);
        send_serialized_telemetry(handle, string, (const char*)id);
    }
}

static void prepare_send_free(TELEMETRY_HANDLE handle, TELEMETRY_DATA *telemetry_object)
{
    // Get request ID from telemetry object if any
    char requestId[NO_DASH_UUID_LEN] = "";
    if (!IS_STRING_NULL_OR_EMPTY(telemetry_object->requestId))
    {
        (void) snprintf(requestId, NO_DASH_UUID_LEN, "%s", telemetry_object->requestId);
    }
    
    STRING_HANDLE serialized = PropertybagSerialize(telemetry_serialize, telemetry_object);
    if (serialized) 
    {
        send_serialized_telemetry(handle, serialized, requestId);
        STRING_delete(serialized);
    }

    if (telemetry_object != NULL)
    {
        free(telemetry_object);
    }
}

static PROPERTYBAG_HANDLE * getJsonForEvent(TELEMETRY_HANDLE handle, const char *eventName)
{
    if (strcmp(eventName, kEvent_type_audioStart) == 0)
        return &handle->current_telemetry_object->audioStartJson;
    if (strcmp(eventName, kEvent_type_microphone) == 0)
        return &handle->current_telemetry_object->microphoneJson;
    if (strcmp(eventName, kEvent_type_audioPlayback) == 0)
        return &handle->current_telemetry_object->ttsJson;
    if (strcmp(eventName, kEvent_type_device) == 0)
        return &handle->current_telemetry_object->deviceJson;

    LogError("Telemetry: invalid event name (%s),\r\n", eventName);
    return NULL;
}

void telemetry_flush(TELEMETRY_HANDLE handle)
{    
    Lock(handle->lock);
    // Check if events exist in queue. If yes, flush them out first.
    LIST_ITEM_HANDLE queue_item = NULL;
    while (NULL != (queue_item = singlylinkedlist_get_head_item(handle->inband_telemetry_queue)))
    {
        TELEMETRY_DATA* telemetry_data = (TELEMETRY_DATA*)singlylinkedlist_item_get_value(queue_item);
        prepare_send_free(handle, telemetry_data);
        singlylinkedlist_remove(handle->inband_telemetry_queue, queue_item);
    }
    // Once events in the queue are flushed, flush the events associated with current requestId
    prepare_send_free(handle, handle->current_telemetry_object);
    // Reallocate the current telemetry object
    handle->current_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
    Unlock(handle->lock);
}

void record_received_msg(TELEMETRY_HANDLE handle, const char *receivedMsg)
{
    int ret = 0;

    if (IS_STRING_NULL_OR_EMPTY(receivedMsg))
    {
        LogError("Telemetry: received an empty message.\r\n");
        return;
    }

    char timeString[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
    {
        return;
    }

    IncomingMsgType msgType = countOfMsgTypes;
    bool found = false;
    for (const auto& msg : messageIndexToName)
    {
        if (msg.second == receivedMsg)
        {
            msgType = msg.first;
            found = true;
        }
    }

    if (!found)
    {
        LogError("Telemetry: received unexpected msg: (%s).\r\n", receivedMsg);
        return;
    }

    Lock(handle->lock);

    TELEMETRY_DATA * telemetry_data = handle->current_telemetry_object;
    PROPERTYBAG_HANDLE evArray = initialize_jsonArray(&telemetry_data->receivedMsgsJsonArray[msgType]);
    ret = json_array_append_string(json_array((JSON_Value*)evArray), timeString);
    if (ret)
    {
        LogError("Telemetry: Failed to add time to received event msg: (%s).\r\n", receivedMsg);
    }

    Unlock(handle->lock);
}

void register_requestId_change_event(TELEMETRY_HANDLE handle, const char *requestId)
{
    if (IS_STRING_NULL_OR_EMPTY(requestId))
    {
        LogError("Telemetry: empty request id\r\n");
        return;
    }
    // Check if current_telelmetry_object is initialized and populated
    // Also, check if this is not the first requestId set
    if (handle->current_telemetry_object) {
        Lock(handle->lock);

        TELEMETRY_DATA* telemetry_data = handle->current_telemetry_object;
        if (!IS_STRING_NULL_OR_EMPTY(telemetry_data->requestId) && telemetry_data->bPayloadSet)
        {
            // Insert current telemetry object into the queue
            (void)singlylinkedlist_add(handle->inband_telemetry_queue, telemetry_data);
            // Assign new memory with zeros to current telemetry object
            handle->current_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
        }

        if (handle->current_telemetry_object) {
            // Set the requestId element for the current telemetry object
            strcpy_s(handle->current_telemetry_object->requestId, NO_DASH_UUID_LEN, requestId);
        }

        Unlock(handle->lock);
    }
}

void inband_connection_telemetry(TELEMETRY_HANDLE handle, const char *connectionId, const char *key, void *value)
{
    Lock(handle->lock);
    TELEMETRY_DATA* connection_data = handle->current_connection_telemetry_object;
    PROPERTYBAG_HANDLE *connectionJson = &connection_data->connectionJson;
    const char* eventName = kEvent_type_connection;
    int ret = 0;
    if (strcmp(kEvent_start_key, key) == 0)
    {
        ret = populate_event_timestamp(connectionJson, eventName, connectionId, kEvent_start_key);
        connection_data->bPayloadSet = (ret == 0);
    }
    else
    {
        if (value == NULL)
        {
            ret = populate_event_timestamp(connectionJson, eventName, connectionId, key);
        }
        else
        {
            ret = populate_event_key_value(connectionJson, eventName, connectionId, key, value);
        }

        connection_data->bPayloadSet &= (ret == 0);

        if (connection_data->bPayloadSet) 
        {
            (void)singlylinkedlist_add(handle->inband_telemetry_queue, connection_data);
        }
        else 
        {
            free(connection_data);
        }

        // Assign new memory with zeros to current telemetry object
        handle->current_connection_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
    }
    Unlock(handle->lock);
}

void inband_tts_telemetry(TELEMETRY_HANDLE handle, const char *id, const char *key, void *value)
{
    // NOTE: Cortana relic, this function is not currently used anywhere.
    Lock(handle->lock);
    // If current telemetry object is initialized and populated, 
    // compare the current active requestId with the requestId associated with the tts event
    // Add the tts event to the current telemetry object if the requestIds are the same
    
    // NOTE: IMPORTANT! this logic is no loger sound!!!
    if (handle->current_telemetry_object) {
        if (!IS_STRING_NULL_OR_EMPTY(handle->current_telemetry_object->requestId) &&
            strcmp(handle->current_telemetry_object->requestId, id) == 0)
        {
            PROPERTYBAG_HANDLE* ttsJson = &handle->current_telemetry_object->ttsJson;
            if (value == NULL)
            {
                populate_event_timestamp(ttsJson, kEvent_type_audioPlayback, NULL, key);
            }
            else
            {
                populate_event_key_value(ttsJson, kEvent_type_audioPlayback, NULL, key, value);
            }
        }
    }
    // flush the tts event as is immediately
    else
    {
        tts_flush(handle, kEvent_type_audioPlayback, id, key, value);
    }
    Unlock(handle->lock);
}

void inband_kws_telemetry(TELEMETRY_HANDLE handle, int kwsStartOffsetMS, int audioStartOffsetMS)
{
    // TODO: this function is not currently used anywhere.
    Lock(handle->lock);
    metrics_listening_stop(handle);

    char time[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601TimeOffset(time, TIME_STRING_MAX_SIZE, kwsStartOffsetMS))
    {
        goto exit;
    }
    metrics_listening_start(handle, time);

    if (-1 == GetISO8601TimeOffset(time, TIME_STRING_MAX_SIZE, audioStartOffsetMS))
    {
        goto exit;
    }
    metrics_recording_start(handle, time);

exit:
    Unlock(handle->lock);
}

void inband_event_key_value_populate(TELEMETRY_HANDLE handle, const char *eventName, const char *id, const char *key, void *value)
{
    Lock(handle->lock);
    PROPERTYBAG_HANDLE *pBag = getJsonForEvent(handle, eventName);
    if (pBag != NULL)
    {
        int ret = populate_event_key_value(pBag, eventName, id, key, value);
        // Set the bPayloadSet flag
        handle->current_telemetry_object->bPayloadSet |= (ret == 0);
    }
    Unlock(handle->lock);
}

void inband_event_timestamp_populate(TELEMETRY_HANDLE handle, const char *eventName, const char *id, const char *key)
{
    Lock(handle->lock);
    PROPERTYBAG_HANDLE *pBag = getJsonForEvent(handle, eventName);
    if (pBag != NULL)
    {
        int ret = populate_event_timestamp(pBag, eventName, id, key);
        // Set the bPayloadSet flag
        handle->current_telemetry_object->bPayloadSet |= (ret == 0);
    }
    Unlock(handle->lock);
}

#pragma pack(pop)

TELEMETRY_HANDLE telemetry_create(PTELEMETRY_WRITE callback, void* context)
{
    PropertybagInitialize();
    initialize_message_name_array();
    TELEMETRY_CONTEXT* ctx = (TELEMETRY_CONTEXT *)calloc(1, sizeof(TELEMETRY_CONTEXT));
    if (ctx) {
        ctx->current_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
        // as 
        ctx->current_connection_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
        ctx->lock = Lock_Init();
        ctx->callback = callback;
        ctx->callbackContext = context;
        ctx->inband_telemetry_queue = singlylinkedlist_create();
    }
    return ctx;
}

void telemetry_destroy(TELEMETRY_HANDLE handle)
{
    if (handle->current_telemetry_object != NULL)
    {
        free(handle->current_telemetry_object);
        handle->current_telemetry_object = NULL;
    }
    if (handle->current_connection_telemetry_object != NULL)
    {
        free(handle->current_connection_telemetry_object);
        handle->current_connection_telemetry_object = NULL;
    }
    if (handle->inband_telemetry_queue != NULL)
    {
        singlylinkedlist_destroy(handle->inband_telemetry_queue);
        handle->inband_telemetry_queue = NULL;
    }
    PropertybagShutdown();

    if (handle->lock != NULL)
    {
        Lock_Deinit(handle->lock);
    }

    free(handle);
}

#if defined(PRINT_TELEMETRY_EVENTS) || defined(USE_ARIA)
// This is a map from metric ID to name.
static const char* const gMetricNames[] =
{
#define DEFINE_METRIC_ID(__id) #__id,
#include "metric-ids.h"
#undef DEFINE_METRIC_ID
    ""
};

static const char* telemetry_get_name(METRIC_ID id)
{
    assert(id >= 0);
    assert(id < METRIC_ID_MAX);

    return gMetricNames[id];
}
#endif // defined(PRINT_TELEMETRY_EVENTS) || defined(USE_ARIA)

int telemetry_log_event(METRIC_ID event_id, const char* types, ...)
{
    va_list args;
    va_start(args, types);

    int const result = v_telemetry_log_event(event_id, types, args);

    va_end(args);
    return result;
}

// See the function declaration in metrics.h for examples on how to use this.
int v_telemetry_log_event(METRIC_ID event_id, const char* types, va_list args)
{
#ifdef PRINT_TELEMETRY_EVENTS
    char string_to_print[1024] = {0};
    int total_printed = 0;

    #define PRINT_TELEMETRY_INFO(...) \
        { \
            int const printed = snprintf( \
                string_to_print + total_printed, \
                sizeof(string_to_print) - total_printed, \
                __VA_ARGS__); \
            total_printed += (printed >= 0) ? printed : 0; \
        }

    PRINT_TELEMETRY_INFO("Telemetry event: %s",
        telemetry_get_name(event_id));
#else // PRINT_TELEMETRY_EVENTS
    #define PRINT_TELEMETRY_INFO(...) // Do nothing
#endif // ifdef PRINT_TELEMETRY_EVENTS else

#ifndef USE_ARIA
    (void)event_id;
    (void)types;
    (void)args;

#ifdef PRINT_TELEMETRY_EVENTS
    // Even though Aria is disabled, we want to print the telemetry events to
    // the log.
    PRINT_TELEMETRY_INFO(" (ARIA DISABLED)");

    if (types != NULL)
    {
        while (types[0] != '\0')
        {
            const char* const key = va_arg(args, const char*);

            switch (types[0])
            {
            case 's':
                PRINT_TELEMETRY_INFO(" {%s: \"%s\"}",
                    key, va_arg(args, const char*));
                break;

            case 'l':
                PRINT_TELEMETRY_INFO(" {%s: %ld}", key, va_arg(args, long));
                break;

            case 'd':
                PRINT_TELEMETRY_INFO(" {%s: %lf}", key, va_arg(args, double));
            }

            types++;
        }
    }

    LogInfo("%s", string_to_print);
#endif // PRINT_TELEMETRY_EVENTS

    return 0; // No op for all Aria APIs.
#else // ifndef USE_ARIA
    int retval = 0;
    ARIA_EVENT event = NULL;

    if (gAriaLogger == NULL)
    {
        LogError("Aria not initialized. Dropping event '%s'",
            telemetry_get_name(event_id));
        retval = -1;
        goto Exit;
    }

    GOTO_EXIT_IF_NONZERO(aria_create_event(telemetry_get_name(event_id), &event));

    if (types != NULL)
    {
        while (types[0] != '\0')
        {
            const char* key = va_arg(args, const char*);

            switch (types[0])
            {
            case 's':
                {
                    // The property won't show up on the backend if it has an
                    // empty string as a value.
                    const char* value = va_arg(args, const char*);
                    value = value ? value : "";
                    GOTO_EXIT_IF_NONZERO(aria_set_event_property_string(event, key,
                        value));

                    PRINT_TELEMETRY_INFO(" {%s: \"%s\"}", key, value);
                    break;
                }

            case 'l':
                {
                    long const value = va_arg(args, long);
                    GOTO_EXIT_IF_NONZERO(aria_set_event_property_long(event, key,
                        value));
                    PRINT_TELEMETRY_INFO(" {%s: %ld}", key, value);
                }
                break;

            case 'd':
                {
                    double const value = va_arg(args, double);
                    GOTO_EXIT_IF_NONZERO(aria_set_event_property_double(event, key,
                        value));
                    PRINT_TELEMETRY_INFO(" {%s: %lf}", key, value);
                }
                break;

            default:
                LogError("Unknown type '%c'", types[0]);
                retval = -1;
                goto Exit;
            }

            types++;
        }
    }

    GOTO_EXIT_IF_NONZERO(aria_log_event(gAriaLogger, event));

#ifdef PRINT_TELEMETRY_EVENTS
    LogInfo("%s", string_to_print);
#endif // PRINT_TELEMETRY_EVENTS

Exit:
    if (event != NULL)
    {
        aria_destroy_event(event);
    }
    return retval;
#endif // ifndef USE_ARIA else
}

