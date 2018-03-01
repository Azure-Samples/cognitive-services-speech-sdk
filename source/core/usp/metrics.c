//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// metrics.h: send and process telemetry messages.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

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

#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/queue.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/agenttime.h"
#include "uspcommon.h"
#include "metrics.h"


const char* kEvent_type_key = "EventType";
const char* kRcvd_msgs_key = "ReceivedMessages";
const char* kMetric_events_key = "Metrics";

const char* kEvent_type_skill = "skill";
const char* kEvent_type_audioPlayback = "audio:playback";
const char* kEvent_type_viewDisplayText = "view:displayText";
const char* kEvent_type_viewCard = "view:card";
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
const char* kEvent_type_skill_action_key = "action";

const char *kRcvd_msg_audio_key = "audio";

static const char *speechMsgNames[countOfMsgTypes];

PROPERTYBAG_HANDLE evMetricArray;
PROPERTYBAG_HANDLE evRcvMsgsArray;

#define GOTO_EXIT_IF_NONZERO(exp) { retval = exp; if (retval != 0) { goto Exit; } }
#define RESET(val)  val = NULL

#pragma pack(push, 1)

static LOCK_HANDLE metricLock;

static PTELEMETRY_WRITE g_pfnCallback = NULL;
static void*            g_pContext = NULL;
static LIST_HANDLE inband_telemetry_queue = NULL;
TELEMETRY_DATA *current_telemetry_object = NULL;
TELEMETRY_DATA *current_connection_telemetry_object = NULL;
// Flag to check if this is first request 
static bool bSetFirstRequestId = false;

static PROPERTYBAG_HANDLE current_skill_object = NULL;

static void free_telemetry_global_arrays()
{
    RESET(evRcvMsgsArray);
    RESET(evMetricArray);
}

static void initialize_message_name_array()
{
    // The order of this array needs to be consistent with the enum incomingMsgType
    speechMsgNames[turnStart] = g_messagePathTurnStart;
    speechMsgNames[speechStartDetected] = g_messagePathSpeechStartDetected;
    speechMsgNames[speechHypothesis] = g_messagePathSpeechHypothesis;
    speechMsgNames[speechEndDetected] = g_messagePathSpeechEndDetected;
    speechMsgNames[speechPhrase] = g_messagePathSpeechPhrase;
    speechMsgNames[audio] = kRcvd_msg_audio_key;
    speechMsgNames[turnEnd] = g_messagePathTurnEnd;
}

static void initialize_telemetry_global_arrays()
{
    evMetricArray = json_value_init_array();
    if (NULL == evMetricArray)
    {
        LogInfo("Telemetry: Unable to initialize Metrics event array\r\n");
    }

    evRcvMsgsArray = json_value_init_array();
    if (NULL == evRcvMsgsArray)
    {
        LogInfo("Telemetry: Unable to initialize Received Messages array\r\n");
    }
}

static void PropertybagAddValueToArray(PROPERTYBAG_HANDLE propertyHandle, void * value)
{
    if (propertyHandle && value)
    {
        json_array_append_value(json_array(propertyHandle), (JSON_Value *)value);
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
            PropertybagSetNumberValue(json_object(propertyHandle), name, json_value_get_number((JSON_Value*)value));
            return 0;
        }
        case JSONBoolean:
        {
            PropertybagSetBooleanValue(json_object(propertyHandle), name, json_value_get_boolean((JSON_Value*)value));
            return 0;
        }
        case JSONString:
        {
            PropertybagSetStringValue(json_object(propertyHandle), name, json_value_get_string((JSON_Value*)value));
            return 0;
        }
        case JSONArray:
        {
            PropertybagSetArrayValue(json_object(propertyHandle), name, (JSON_Value*)value);
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

static PROPERTYBAG_HANDLE getReceivedMsgJsonArray(const char *arrayName)
{
    if (!strcmp(arrayName, g_messagePathTurnStart))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[turnStart]);
    }
    if (!strcmp(arrayName, g_messagePathSpeechStartDetected))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[speechStartDetected]);
    }
    if (!strcmp(arrayName, g_messagePathSpeechHypothesis))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[speechHypothesis]);
    }
    if (!strcmp(arrayName, g_messagePathSpeechEndDetected))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[speechEndDetected]);
    }
    if (!strcmp(arrayName, g_messagePathSpeechPhrase))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[speechPhrase]);
    }
    if (!strcmp(arrayName, kRcvd_msg_audio_key))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[audio]);
    }
    if (!strcmp(arrayName, g_messagePathTurnEnd))
    {
        return initialize_jsonArray(&current_telemetry_object->receivedMsgsJsonArray[turnEnd]);
    }

    return NULL;
}

static PROPERTYBAG_HANDLE initialize_metricobject(
    const char            *name,
    const char            *id)
{
    PROPERTYBAG_HANDLE pBag = PropertybagInitializeWithKeyValue(NULL, NULL);
    if (pBag && name)
    {
        PropertybagSetStringValue(json_object(pBag), "Name", name);
        if (id)
        {
            PropertybagSetStringValue(json_object(pBag), "Id", id);
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
    uint64_t current_ms;
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
    snprintf(buffer + 20, 5, "%03dZ", stCurrentTimeAfterOffset.wMilliseconds);
#else
    strftime(buffer, length, "%FT%T.", timeinfo);
    snprintf(buffer + 20, 5, "%03dZ", adjusted_ms);
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
    timeStringLength += snprintf(buffer + 20, 5, "%03dZ", sysTime.wMilliseconds);
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

void telemetry_setcallbacks(PTELEMETRY_WRITE pfnCallback, void* pContext)
{
#if defined(GB_DEBUG_ALLOC)
    json_set_allocation_functions(gballoc_malloc, gballoc_free);
#endif

    g_pfnCallback = pfnCallback;
    g_pContext = pContext;

    metricLock = Lock_Init();
}

static void telemetry_add_metricevents(TELEMETRY_DATA *telemetry_object)
{
    if (telemetry_object->connectionJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->connectionJson);
        // current_connection_telemetry_object would be the only one that would have
        // connectionJson initialized. If connectionJson is initialized for 
        // any TELEMETRY_DATA object, they would not contain values. 
        // Hence no need to check other pointers and we can return here.
        return;
    }

    if (telemetry_object->audioStartJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->audioStartJson);
    }

    if (telemetry_object->microphoneJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->microphoneJson);
    }

    if (telemetry_object->listeningTriggerJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->listeningTriggerJson);
    }

    if (telemetry_object->skillJson)
    {
        if (current_skill_object)
        {
            PropertybagAddValueToArray(telemetry_object->skillJson, current_skill_object);
            current_skill_object = NULL;
        }
        PropertybagAddValueToArray(evMetricArray, telemetry_object->skillJson);
    }

    if (telemetry_object->audioPlaybackJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->audioPlaybackJson);
    }

    if (telemetry_object->viewDisplayTextJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->viewDisplayTextJson);
    }

    if (telemetry_object->viewCardJson)
    {
        PropertybagAddValueToArray(evMetricArray, telemetry_object->viewCardJson);
    }
}

static void telemetry_add_recvmsgs(TELEMETRY_DATA *telemetry_object)
{
    if (telemetry_object)
    {
        for (int i = 0; i < countOfMsgTypes; i++)
        {
            PROPERTYBAG_HANDLE recvObj = NULL;
            if (telemetry_object->receivedMsgsJsonArray[i])
            {
                recvObj = PropertybagInitializeWithKeyValue(speechMsgNames[i], telemetry_object->receivedMsgsJsonArray[i]);
                PropertybagAddValueToArray(evRcvMsgsArray, recvObj);
            }
        }
    }
}

int telemetry_serialize(PROPERTYBAG_HANDLE propertyHandle, void* pContext)
{
    TELEMETRY_DATA *telemetry_object = (TELEMETRY_DATA *)pContext;

    // Create and append all received events
    telemetry_add_recvmsgs(telemetry_object);

    // append all metric events
    telemetry_add_metricevents(telemetry_object);

    // Root Object
    if (propertyHandle)
    {
        PropertybagSetArrayValue(propertyHandle, kRcvd_msgs_key, evRcvMsgsArray);
        PropertybagSetArrayValue(propertyHandle, kMetric_events_key, evMetricArray);
    }
    return 0;
}

int tts_serialize(PROPERTYBAG_HANDLE propertyHandle, void* pContext)
{
    if (pContext)
    {
        PropertybagAddValueToArray(evMetricArray, pContext);
    }

    // Root Object
    if (propertyHandle)
    {
        PropertybagSetArrayValue(propertyHandle, kRcvd_msgs_key, evRcvMsgsArray);
        PropertybagSetArrayValue(propertyHandle, kMetric_events_key, evMetricArray);
    }
    return 0;
}

static void send_serialized_telemetry(PROPERTYBAG_HANDLE pHandle, 
    PPROPERTYBAG_OBJECT_CALLBACK fn, 
    const char *requestId)
{
    STRING_HANDLE hString;
    const char *eSerialArray;

    // Serialize the received messages events and metric events.
    hString = PropertybagSerialize(fn, pHandle);

    if (hString)
    {
        // Serialize the received messages events and metric events.
        eSerialArray = STRING_c_str(hString);
        if (eSerialArray && g_pfnCallback)
        {
            g_pfnCallback((const uint8_t*)eSerialArray, strlen(eSerialArray), g_pContext, requestId);
        }

        STRING_delete(hString);
    }
}

static void tts_flush(const char *eventName, const char *id, const char *key, void *value)
{
    // Create a JSON object that stores the telemetry events
    PROPERTYBAG_HANDLE *pBag = initialize_metricobject(eventName, NULL);
    if (pBag == NULL)
    {
        return;
    }

    // IF value is NULL, this is either a TTS_START or a TTS_END event
    // Associate the timestamp with the event and flush it
    if (value == NULL)
    {
        char timeString[TIME_STRING_MAX_SIZE];
        if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
        {
            return;
        }

        PropertybagSetStringValue(json_object((JSON_Value*)pBag), key, timeString);
    }
    // Case for TTS_ERROR
    else
    {
        PropertybagSetValue(pBag, key, value);
    }

    initialize_telemetry_global_arrays();
    send_serialized_telemetry(pBag, tts_serialize, (const char*)id);
    free_telemetry_global_arrays();
}

static void prepare_send_free(TELEMETRY_DATA *telemetry_object)
{
    initialize_telemetry_global_arrays();
    // Get request ID from telemetry object if any
    char requestId[37] = "";
    if (!IS_STRING_NULL_OR_EMPTY(telemetry_object->requestId))
    {
        snprintf(requestId, 37, "%s", telemetry_object->requestId);
    }
    
    send_serialized_telemetry(telemetry_object, telemetry_serialize, requestId);

    if (telemetry_object != NULL)
    {
        free(telemetry_object);
    }
    free_telemetry_global_arrays();
}

void telemetry_flush()
{    
    Lock(metricLock);
    // Check if events exist in queue. If yes, flush them out first.
    if (inband_telemetry_queue)
    {
        TELEMETRY_DATA *telemetry_object = NULL;
        while (NULL != (telemetry_object = (TELEMETRY_DATA *)queue_dequeue(inband_telemetry_queue)))
        {
            prepare_send_free(telemetry_object);
        }
        // Once the list is empty. Set the queue to NULL
        list_destroy(inband_telemetry_queue);
        inband_telemetry_queue = NULL;
    }
    // Once events in the queue are flushed, flush the events associated with current requestId
    prepare_send_free(current_telemetry_object);
    // Reallocate the current telemetry object
    current_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
    Unlock(metricLock);
}

void record_received_msg(const char *receivedMsg)
{
    int ret = 0;

    if (current_telemetry_object == NULL)
    {
        LogInfo("Telemetry: Telemetry object not initialized\r\n");
        return;
    }

    if (IS_STRING_NULL_OR_EMPTY(receivedMsg))
    {
        LogInfo("Telemetry: Invalid received Message Event\r\n");
        return;
    }

    char timeString[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
    {
        return;
    }

    PROPERTYBAG_HANDLE evArray = getReceivedMsgJsonArray(receivedMsg);
    if (evArray == NULL)
    {
        return;
    }

    ret = json_array_append_string(json_array(evArray), timeString);
    if (ret)
    {
        LogInfo("Telemetry: Failed to add time to received event msg: (%s).\r\n", receivedMsg);
        return;
    }
}

void register_requestId_change_event(const char *requestId)
{
    if (IS_STRING_NULL_OR_EMPTY(requestId))
    {
        LogInfo("Telemetry: Invalid request ID change Event\r\n");
        return;
    }
    // Check if current_telelmetry_object is initialized and populated
    // Also, check if this is not the first requestId set
    Lock(metricLock);
    if (current_telemetry_object && 
        !IS_STRING_NULL_OR_EMPTY(current_telemetry_object->requestId) &&
        bSetFirstRequestId)
    {
        // If the telemetry queue is not yet initialized, create it and set the flag
        if (NULL == inband_telemetry_queue)
        {
            inband_telemetry_queue = list_create();
        }
        // Insert current telemetry object into the queue
        
        if (current_telemetry_object->bPayloadSet)
        {
            queue_enqueue(inband_telemetry_queue, current_telemetry_object);
            // Assign new memory with zeros to current telemetry object
            current_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
        }        
    }
    // If this is the first requestId set, then set the flag
    else
    {
        bSetFirstRequestId = true;
    }

    if (current_telemetry_object == NULL)
    {
        LogError("Incorrect initialization for telemetryobject.");
        goto exit;
    }
    // Set the requestId element for the current telemetry object
    strcpy_s(current_telemetry_object->requestId, 37, requestId);

exit:
    Unlock(metricLock);
}

void inband_skill_telemetry(const char *skillId, const char *key, void *value)
{
    if (current_telemetry_object == NULL)
    {
        LogInfo("Telemetry: Telemetry object not initialized\r\n");
        return;
    }

    Lock(metricLock);
    if (NULL == current_telemetry_object->skillJson)
    {
        current_telemetry_object->skillJson = json_value_init_array();
    }
    // If skill-start event and current_skill_object already intialized. Push the object
    // into the skillArray and reset it to NULL
    if (!strcmp(key, kEvent_start_key) && current_skill_object)
    {
        PropertybagAddValueToArray(current_telemetry_object->skillJson, current_skill_object);
        current_skill_object = NULL;
    }

    if (value == NULL)
    {
        inband_event_timestamp_populate(&current_skill_object, kEvent_type_skill, skillId, key);
    }
    else
    {
        inband_event_key_value_populate(&current_skill_object, kEvent_type_skill, skillId, key, value);
    }
    Unlock(metricLock);
}

void inband_connection_telemetry(const char *connectionId, const char *key, void *value)
{
    Lock(metricLock);
    if (current_connection_telemetry_object == NULL)
    {
        current_connection_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
    }

    if (!strcmp(kEvent_start_key, key))
    {
        inband_event_timestamp_populate(&current_connection_telemetry_object->connectionJson, kEvent_type_connection, connectionId, key);
    }
    else
    {
        if (value == NULL)
        {
            inband_event_timestamp_populate(&current_connection_telemetry_object->connectionJson, kEvent_type_connection, connectionId, key);
        }
        else
        {
            inband_event_key_value_populate(&current_connection_telemetry_object->connectionJson, kEvent_type_connection, connectionId, key, value);
        }
        // Add current_telemetry_object to telemetry queue
        if (NULL == inband_telemetry_queue)
        {
            inband_telemetry_queue = list_create();
        }
        
        queue_enqueue(inband_telemetry_queue, current_connection_telemetry_object);
        
        // Assign new memory with zeros to current telemetry object
        current_connection_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
    }
    Unlock(metricLock);
}

void inband_tts_telemetry(const char *id, const char *key, void *value)
{
    Lock(metricLock);
    // If current telemetry object is initialized and populated, 
    // compare the current active requestId with the requestId associated with the tts event
    // Add the tts event to the current telemetry object if the requestIds are the same
    if (current_telemetry_object && 
        !IS_STRING_NULL_OR_EMPTY(current_telemetry_object->requestId) &&
        !strcmp(current_telemetry_object->requestId, id))
    {
        if (value == NULL)
        {
            inband_event_timestamp_populate(&current_telemetry_object->audioPlaybackJson, kEvent_type_audioPlayback, NULL, key);
        }
        else
        {
            inband_event_key_value_populate(&current_telemetry_object->audioPlaybackJson, kEvent_type_audioPlayback, NULL, key, value);
        }
    }
    // flush the tts event as is immediately
    else
    {
        tts_flush(kEvent_type_audioPlayback, id, key, value);
    }
    Unlock(metricLock);
}

void inband_kws_telemetry(int kwsStartOffsetMS, int audioStartOffsetMS)
{
    Lock(metricLock);
    metrics_listening_stop();

    char time[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601TimeOffset(time, TIME_STRING_MAX_SIZE, kwsStartOffsetMS))
    {
        goto exit;
    }
    metrics_listening_start(time);

    if (-1 == GetISO8601TimeOffset(time, TIME_STRING_MAX_SIZE, audioStartOffsetMS))
    {
        goto exit;
    }
    metrics_recording_start(time);

exit:
    Unlock(metricLock);
}

void inband_event_key_value_populate(PROPERTYBAG_HANDLE *pBag, const char *eventName, const char *id, const char *key, void *value)
{
    int ret = 0;
    
    if (current_telemetry_object == NULL && current_connection_telemetry_object == NULL)
    {
        return;
    }
   
    if (IS_STRING_NULL_OR_EMPTY(eventName))
    {
        LogInfo("Telemetry: Invalid event Name\r\n");
        return;
    }
    if (IS_STRING_NULL_OR_EMPTY(key))
    {
        LogInfo("Telemetry: Invalid key\r\n");
        return;
    }

    Lock(metricLock);
    *pBag = get_metric_object(*pBag, eventName, id);
    if (*pBag == NULL)
    {
        goto exit;
    }

    ret = PropertybagSetValue(*pBag, key, value);
    if (ret)
    {
        LogInfo("Telemetry: Failed to add to new event object (%s) with key: %s.\r\n", eventName, key);
        goto exit;
    }

    // Set the bPayloadSet flag
    current_telemetry_object->bPayloadSet = 1;

exit:
    Unlock(metricLock);
}

void inband_event_timestamp_populate(PROPERTYBAG_HANDLE *pBag, const char *eventName, const char *id, const char *key)
{
    Lock(metricLock);
    if (current_telemetry_object == NULL && current_connection_telemetry_object == NULL)
    {
        goto exit;
    }
   
    *pBag = get_metric_object(*pBag, eventName, id);
    if (*pBag == NULL)
    {
        goto exit;
    }

    char timeString[TIME_STRING_MAX_SIZE];
    if (-1 == GetISO8601Time(timeString, TIME_STRING_MAX_SIZE))
    {
        goto exit;
    }

    PropertybagSetStringValue(json_object(*pBag), key, timeString);

    // Set the bPayloadSet flag
    current_telemetry_object->bPayloadSet = 1;

exit:
    Unlock(metricLock);
}

#pragma pack(pop)


void telemetry_uninitialize()
{
    if (current_telemetry_object != NULL)
    {
        free(current_telemetry_object);
        current_telemetry_object = NULL;
    }
    if (current_connection_telemetry_object != NULL)
    {
        free(current_connection_telemetry_object);
        current_connection_telemetry_object = NULL;
    }
    if (inband_telemetry_queue != NULL)
    {
        list_destroy(inband_telemetry_queue);
        inband_telemetry_queue = NULL;
    }

#ifdef USE_ARIA
    if (gAriaLogger != NULL)
        gAriaLogger = NULL;

    skype_library_shutdown();
#endif
}

void inband_telemetry_initialize()
{
    if (current_telemetry_object != NULL)
    {
        LogError("Incorrect initialization of inband telemetry.");
    }
    current_telemetry_object = (TELEMETRY_DATA *)calloc(1, sizeof(TELEMETRY_DATA));
    initialize_message_name_array();
}

int telemetry_initialize()
{
    inband_telemetry_initialize();
#ifndef USE_ARIA
    // No op for all Aria APIs.
    return 0;
#else // ifndef USE_ARIA
    int retval = skype_library_init();

    if (retval != 0)
    {
        LogError("Skype library init failed");
    }

    GOTO_EXIT_IF_NONZERO(aria_get_logger(
        ARIA_TENANT_TOKEN,
        "CortanaSDK",
        &gAriaLogger));

    // Make a best effort to set the SDK version that identifies this build.
    telemetry_set_context_string("sdk_version", get_cortana_sdk_version());

    telemetry_set_context_long("metrics_version", METRICS_VERSION);

    // N.B.: Aria has a common property called EventInfo.InitId that can be
    //       used to identify this instance of Cortana.

Exit:
    return retval;
#endif // ifndef USE_ARIA else
}


int telemetry_set_context_string(const char* key, const char* value)
{
#ifndef USE_ARIA
    (void)key;
    (void)value;
    return 0;
#else // ifndef USE_ARIA
    if (gAriaLogger == NULL)
    {
        LogError("Aria not initialized. Dropping context '%s', val:%s", key, value);
        return -1;
    }

    return aria_set_logger_context_string(gAriaLogger, key, value ? value : "");
#endif // ifndef USE_ARIA else
}

int telemetry_set_context_double(const char* key, double value)
{
#ifndef USE_ARIA
    (void)key;
    (void)value;
    return 0;
#else // ifndef USE_ARIA
    if (gAriaLogger == NULL)
    {
        LogError("Aria not initialized. Dropping context '%s', val:%lf", key, value);
        return -1;
    }

    return aria_set_logger_context_double(gAriaLogger, key, value);
#endif // ifndef USE_ARIA else
}

int telemetry_set_context_long(const char* key, long value)
{
#ifndef USE_ARIA
    (void)key;
    (void)value;
    return 0;
#else // ifndef USE_ARIA
    if (gAriaLogger == NULL)
    {
        LogError("Aria not initialized. Dropping context '%s', val:%ld", key, value);
        return -1;
    }

    return aria_set_logger_context_long(gAriaLogger, key, value);
#endif // ifndef USE_ARIA else
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

