//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// metrics.h: metic messages related structure and methods
//

// Todo: refactor to add/remove messages according to USP specification
// Todo: refactor to follow coding guideline.

#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "parson.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NO_DASH_UUID_LEN 37
#define TIME_STRING_MAX_SIZE 30
#define ERROR_STRING_MAX_SIZE 50
#define MAX_EVENT_ARRAY_SIZE 500
#define IS_STRING_NULL_OR_EMPTY(str) (!str || !*str)

static const char music_session_id_arg[] = "music_session_id";
static const char action_name_arg[] = "action_name";
static const char buffer_session_id_arg[] = "buffer_id";

typedef void* PROPERTYBAG_HANDLE;

typedef struct _TELEMETRY_CONTEXT* TELEMETRY_HANDLE;

/**
 * The PTELEMETRY_WRITE type represents an application-defined
 * callback function used to handle raw telemetry data.
 * @param pBuffer A pointer to a raw telemetry data.
 * @param byteToWrite The length of pBuffer in bytes.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PTELEMETRY_WRITE)(const uint8_t* pBuffer, size_t byteToWrite, void *pContext, const char *requestId);

/**
 * Returns the the current time used for telemetry events.
 * @return A 64-bit time value.
 */
extern uint64_t telemetry_gettime();

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

/**
 * Flushes any outstanding telemetry events.
 */
void telemetry_flush(TELEMETRY_HANDLE handle);


 /**
 * Creates a object handle to maintain telemetry state, initializes the internal structures and
 * sets the application defined callbacks used for uploading in-band telemetry events.
 * @param pfnCallback The callbacks used for uploading in-band telemetry events.
 * @param pContext The callback context.
 */
TELEMETRY_HANDLE telemetry_create(PTELEMETRY_WRITE callback, void* context);

/**
 * Clears all memory. To be called at the completion of all telemetry operations
 *
 */
void telemetry_destroy(TELEMETRY_HANDLE handle);

typedef enum _METRIC_ID
{
#define DEFINE_METRIC_ID(__id) METRIC_ID_ ## __id,
#include "metricids.h"
#undef DEFINE_METRIC_ID
    METRIC_ID_MAX
} METRIC_ID;

/**
 * Log an event to our Aria telemetry pipeline.  This telemetry pipeline is
 * used for skill-specific telemetry.  Speech has its own telemetry pipeline
 * through Truman.
 *
 * Avoid consuming this API directly.  Instead, wrap your telemetry in a
 * metrics_* macro defined in this file.
 *
 * Here are some examples:
 *  telemetry_log_event(METRIC_ID_example_without_properties, NULL);
 *  telemetry_log_event(METRIC_ID_example_with_string, "s", "my_string", "Hello World!");
 *
 * @param event_name The name of the event in Aria.  This should be underscore
 *        case (e.g. my_property) by convention.
 * @param types If this is non-null, then it is a string where each character
 *        indicates two variadic arguments: a const char* key, and a value.
 *        The type of the value is determined by the character:
 *          s: const char*
 *          l: long
 *          d: double
 *
 *        A property key should be underscore case (e.g. my_property) by convention.
 */
int telemetry_log_event(METRIC_ID event_id, const char* types, ...);
int v_telemetry_log_event(METRIC_ID event_id, const char* types, va_list args);

/**
* Timestamps and records telemetry event.
* @param handle A handle that captures current telemetry state.
* @param eventName The name of the event to be recorded.
* @param id The unique id of the event to be recorded.
* @param key The key of the JSON element to be recorded.
* @param value The value of the JSON element to be recorded.
*/
void inband_event_timestamp_populate(TELEMETRY_HANDLE handle, const char *eventName, const char *id, const char *key);

/**
* Received metric event population function.
* @param handle A handle that captures current telemetry state.
* @param receivedMsg The name of the received event from service.
*/
void record_received_msg(TELEMETRY_HANDLE handle, const char *receivedMsg);

/**
* Records connection telemetry event.
* @param handle A handle that captures current telemetry state.
* @param eventName The name of the event to be recorded.
* @param id The unique id of the event to be recorded.
* @param key The key of the JSON element to be recorded.
* @param value The value of the JSON element to be recorded.
*/
void inband_connection_telemetry(TELEMETRY_HANDLE handle, const char *id, const char *key, void *value);

/**
* Records tts telemetry event
* @param handle A handle that captures current telemetry state.
* @param id The unique id of the event to be recorded.
* @param key The key of the JSON element to be recorded.
* @param value The value of the JSON element to be recorded.
*/
void inband_tts_telemetry(TELEMETRY_HANDLE handle, const char *id, const char *key, void *value);

/**
* Creates the start and end events for kws.
* @param handle A handle that captures current telemetry state.
* @param kwsStartOffset the offset in milliseconds used to determine the listening trigger start.
* @param audioStartOffset the offset in milliseconds used to determine the start of audio.
*/
void inband_kws_telemetry(TELEMETRY_HANDLE handle, int kwsStartOffsetMS, int audioStartOffsetMS);

/**
* Handles the necessary changes for a requestId change event.
* @param handle A handle that captures current telemetry state.
* @param requestId the currently active requestId.
*/
void register_requestId_change_event(TELEMETRY_HANDLE handle, const char *requestId);

/**
* Populates and records telemetry event.
* @param handle A handle that captures current telemetry state.
* @param eventName The name of the event to be recorded.
* @param id The unique id of the event to be recorded.
* @param key The key of the JSON element to be recorded.
* @param value The value of the JSON element to be recorded.
*/
void inband_event_key_value_populate(TELEMETRY_HANDLE handle, const char *eventName, const char *id, const char *key, void *value);

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

typedef struct _request_telemetry_object_data
{
    char requestId[NO_DASH_UUID_LEN];
    int bPayloadSet;
    // TODO: This does not make sense, why telementry does not have 
    // a proper set inside?
    PROPERTYBAG_HANDLE receivedMsgsJsonArray[countOfMsgTypes];

    PROPERTYBAG_HANDLE connectionJson;
    PROPERTYBAG_HANDLE audioStartJson;
    PROPERTYBAG_HANDLE microphoneJson;
    PROPERTYBAG_HANDLE listeningTriggerJson;
    PROPERTYBAG_HANDLE ttsJson;
    PROPERTYBAG_HANDLE deviceJson;
} TELEMETRY_DATA;

// Top level Array keys for events
extern const char* kRcvd_msgs_key;
extern const char* kMetric_events_key;

// Received Message event Keys
extern const char* kRcvd_msg_response_key;
extern const char* kRcvd_msg_audio_key;

// Metric event name keys
extern const char* kEvent_type_audioPlayback;
extern const char* kEvent_type_audioStart;
extern const char* kEvent_type_microphone;
extern const char* kEvent_type_listeningTrigger;
extern const char* kEvent_type_connection;
extern const char* kEvent_value;

#define telemetry_log_event_v(__event_id)           telemetry_log_event(__event_id, NULL)
#define telemetry_log_event_i(__event_id, __i0)     telemetry_log_event(__event_id, "l", kEvent_value, (long)(__i0))
#define telemetry_log_event_s(__event_id, __s)      telemetry_log_event(__event_id, "s", kEvent_value, __s)
#define telemetry_log_event_d(__event_id, __d)      telemetry_log_event(__event_id, "d", kEvent_value, ((double)__d))

#define telemetry_log_event2_i(__event_id, __eventname, __i0)   telemetry_log_event(__event_id, "sl", kEvent_name_key, __eventname, kEvent_value, (long)(__i0))
#define telemetry_log_event2_s(__event_id, __eventname, __s)    telemetry_log_event(__event_id, "ss", kEvent_name_key, __eventname, kEvent_value, __s)
#define telemetry_log_event2_d(__event_id, __eventname, __d)    telemetry_log_event(__event_id, "sd", kEvent_name_key, __eventname, kEvent_value, (double)(__d))

// For events where relative times are important, we must use a clock that
// doesn't change when the time changes.  We expect the device's clock to
// change when it is connected to Internet, for example.  Aria's timestamp
// changes if the device's time changes, so we can't reliably use it to diff
// times and get durations.
#ifdef _MSC_VER
#define telemetry_log_perf_event(__event_id, __format, ...) \
    telemetry_log_event(__event_id, "d" __format, "tick_count_ms", (double)cortana_gettickcount(), __VA_ARGS__)
#else
#define telemetry_log_perf_event(__event_id, __format, ...) \
    telemetry_log_event(__event_id, "d" __format, "tick_count_ms", (double)cortana_gettickcount(), ##__VA_ARGS__)
#endif

extern const char* kEvent_type_key;

extern const char* kEvent_type_device;
extern const char* kEvent_type_notification;
extern const char* kEvent_type_sdk;

extern const char* kEvent_name_key;
extern const char* kEvent_start_key;
extern const char* kEvent_end_key;
extern const char* kEvent_deviceid_key;
extern const char* kEvent_id_key;
extern const char* kEvent_memory_key;
extern const char* kEvent_cpu_key;
extern const char* kEvent_error_key;
extern const char* kEvent_status_key;

#define metrics_device_startup(handle, deviceid) \
{ \
    JSON_Value * value = json_value_init_string(deviceid); \
    inband_event_key_value_populate(handle, kEvent_type_device, NULL, kEvent_deviceid_key, value); \
    json_value_free(value); \
}

// Recieved the specified message from the service. 
#define metrics_received_message(handle, x) record_received_msg(handle, x)

// Metric Events defined in telemetry spec
#define metrics_listening_start(handle, kwsStartTime) \
{ \
    JSON_Value * value = json_value_init_string(kwsStartTime); \
    inband_event_key_value_populate(handle, kEvent_type_listeningTrigger, NULL, kEvent_start_key, value); \
    json_value_free(value); \
}

#define metrics_listening_stop(handle) \
    inband_event_timestamp_populate(handle, kEvent_type_listeningTrigger, NULL, kEvent_end_key)

/* Key word spotter model load has been completed. */
#define metrics_keywordspotter_acceptedkeyword(kwsStartOffset, audioStartOffset) \
    inband_kws_telemetry(kwsStartOffset, audioStartOffset)

/* Start of the audio stream event which includes the initial silence before KWS */
#define metrics_recording_start(handle, audioStartTime) \
{ \
    JSON_Value * value = json_value_init_string(audioStartTime); \
    inband_event_key_value_populate(handle, kEvent_type_audioStart, NULL, kEvent_start_key, value); \
    json_value_free(value); \
}

#define metrics_tts_start(handle, requestId) \
    inband_tts_telemetry(handle, requestId, kEvent_start_key, NULL)

#define metrics_tts_stop(handle, requestId)    \
    inband_tts_telemetry(handle, requestId, kEvent_end_key, NULL)

#define metrics_audio_start(handle) \
    inband_event_timestamp_populate(handle, kEvent_type_microphone, NULL, kEvent_start_key)

#define metrics_audio_end(handle)    \
    inband_event_timestamp_populate(handle, kEvent_type_microphone, NULL, kEvent_end_key)

#define metrics_audio_error(handle, error) \
{ \
    JSON_Value *  value = json_value_init_string(error); \
    inband_event_key_value_populate(handle, kEvent_type_microphone, NULL, kEvent_error_key, value); \
    json_value_free(value); \
}

#define metrics_transport_start(handle, connectionId) \
    inband_connection_telemetry(handle, connectionId, kEvent_start_key, NULL)

#define metrics_transport_connected(handle, connectionId) \
    inband_connection_telemetry(handle, connectionId, kEvent_end_key, NULL)

#define metrics_transport_error(handle, connectionId, error) \
{ \
    JSON_Value * value = json_value_init_number(error); \
    inband_connection_telemetry(handle, connectionId, kEvent_error_key, value); \
    json_value_free(value); \
}

// Transport metrics
/* The transport has started a state transition. */
#define metrics_transport_state_start(__state)    telemetry_log_event_i(METRIC_ID_xport_state_start, (int)(__state))

/* The transport has completed a state transition. */
#define metrics_transport_state_end(__state)    telemetry_log_event_i(METRIC_ID_xport_state_end, (int)(__state))

/* Client request identifier for the current turn */
#define metrics_transport_requestid(handle, requestId)    \
    register_requestId_change_event(handle, requestId)

/* Service request identifer for the current turn */
#define metrics_transport_serviceid(__serviceId)  telemetry_log_event_s(METRIC_ID_servicetag, (__serviceId))

/* Client UPL */
#define metrics_transportlibrary_clientupl(__i0)   

// Transport error metrics

// errors that aren't the same as the 0x8XXXXXXX SDK errors
#define INTERNAL_TRANSPORT_ERROR_PARSEERROR     -1
#define INTERNAL_TRANSPORT_UNHANDLEDRESPONSE    -2
#define INTERNAL_TRANSPORT_INVALIDSTATE         -3
#define INTERNAL_CORTANA_EVENT_COALESCED        -4
#define METRIC_MESSAGE_TYPE_DEVICECONTEXT       1
#define METRIC_MESSAGE_TYPE_AUDIO_START         2
#define METRIC_MESSAGE_TYPE_AUDIO_LAST          3
#define METRIC_MESSAGE_TYPE_TELEMETRY            4
#define METRIC_TRANSPORT_STATE_DNS                5
#define METRIC_TRANSPORT_STATE_DROPPED          6
#define METRIC_TRANSPORT_STATE_CLOSED           7
#define METRIC_TRANSPORT_STATE_CANCELLED        8
#define METRIC_TRANSPORT_STATE_RESET            9
#define METRIC_MESSAGE_TYPE_INVALID             0xff

/* The current operation was cancelled. */
#define metrics_transport_cancelled()    metrics_transport_state_end(METRIC_TRANSPORT_STATE_CANCELLED)

/* The transport connection was closed. */
#define metrics_transport_closed()    metrics_transport_state_end(METRIC_TRANSPORT_STATE_CLOSED)

/* The transport connection was dropped. */
#define metrics_transport_dropped()    metrics_transport_state_end(METRIC_TRANSPORT_STATE_DROPPED)

/* The transport connection was reset.  We do this when the token changes
 * during the connection. */
#define metrics_transport_reset() \
    metrics_transport_state_end(METRIC_TRANSPORT_STATE_RESET)

/* The transport has received an unhandled event. */
#define metrics_transport_unhandledresponse() \
    metrics_transport_platformerror(INTERNAL_TRANSPORT_UNHANDLEDRESPONSE, 0 /* audible */)

/* The transport has failed to parse a response. */
#define metrics_transport_parsingerror() \
    metrics_transport_platformerror(INTERNAL_TRANSPORT_ERROR_PARSEERROR, 0 /* audible */)

/* The transport has dropped a packet because it was in a closed state. */
#define metrics_transport_invalidstateerror() \
    metrics_transport_platformerror(INTERNAL_TRANSPORT_INVALIDSTATE, 0 /* audible */)

/* Logs a generic platform error. */
#define metrics_transport_platformerror(__error, __is_audible_error) \
    telemetry_log_event(METRIC_ID_error, "ll", kEvent_value, (long)(__error), "audible", (long)(__is_audible_error))

// Microphone events
/* The microphone device has starting recording audio. */
#define metrics_microphone_start()    

/* The microphone device has stopped recording audio. */
#define metrics_microphone_stop()    

/* The platform is being initialized. */
#define metrics_platform_start()    

/* The platform has been shutdown. */
#define metrics_platform_stop()    

/* When a skill context has been sent. */
#define metrics_skill_contextsent()    

/* When a skill is not supported. */
#define metrics_skill_unhandled(skillId)

/* Web socket response contained a request ID different from our last request */
#define metrics_unexpected_requestid(requestid) \
    telemetry_log_event(METRIC_ID_unexpected_requestid, "s", "requestid", requestid)

/* An agent event is about to be sent, or has been sent. */
#define metrics_agent_event(__id, __name) \
    telemetry_log_event(METRIC_ID_agent_event, "sss", kEvent_name_key, "sending", \
        "id", __id, kEvent_value, __name);
#define metrics_agent_event_complete(__id, __name, __result, __retries) \
    telemetry_log_event(METRIC_ID_agent_event, "sssll", kEvent_name_key, "complete", \
        "id", __id, kEvent_value, __name, "error", (long)(__result), "retries", (long)(__retries));

#define metrics_earcon(__event) telemetry_log_event_s(METRIC_ID_earcon, __event)

/* When an earcon has been started. */
#define metrics_earcon_start()  metrics_earcon("start")

/* When an earcon has finished. */
#define metrics_earcon_end()    metrics_earcon("end")

/* The current peak memory usage. */
#define metrics_memory_peak(__i0) \
    telemetry_log_event2_i(METRIC_ID_resource_report, "memory_peak", __i0)

/* The current memory in use. */
#define metrics_memory_current(__i0) \
    telemetry_log_event2_i(METRIC_ID_resource_report, "memory_current", __i0)

/* The number of crashes detected. */
#define metrics_crash_count(__i0)    telemetry_log_event_i(METRIC_ID_crash_count, __i0)

/* Audio stack error message. */
#define metrics_audio_stack_error(__i0)    telemetry_log_event_i(METRIC_ID_audio_stack_error, __i0)

/* The number of audio samples that were sent to the keyword spotter. */
#define metrics_audio_sentkws(__i0)    telemetry_log_event_i(METRIC_ID_audio_sentkws, __i0)

/* The number of audio samples that were dropped and not sent to either the keyword spotter or the recognizer. */
#define metrics_audio_dropped(__i0)    telemetry_log_event_i(METRIC_ID_audio_dropped, __i0)

/* The number of audio samples that were sent to the recognizer. */
#define metrics_audio_sentrecognizer(__i0)    telemetry_log_event_i(METRIC_ID_audio_sentrecognizer, __i0)

/* The current Cortana state. */
#define metrics_cortana_state(__i0)    telemetry_log_event_i(METRIC_ID_cortana_state, __i0)

/* The listening state reason. */
#define metrics_ux_listening(__i0)     telemetry_log_event2_i(METRIC_ID_cortana_state, "listening", __i0)

/* When and which KWS was reset */
#define metrics_kws_reset(__i0) \
    telemetry_log_event2_i(METRIC_ID_kws, "reset", (long)(__i0))

/* Whether the turn started from KWS. */
#define metrics_audio_kws_triggered(__i0)    telemetry_log_event_i(METRIC_ID_audio_kws_trigger, __i0)

/* audio stream events*/
#define metrics_audiostream_init()  telemetry_log_event_v(METRIC_ID_audio_stream_init)
#define metrics_audiostream_flush()  telemetry_log_event_v(METRIC_ID_audio_stream_flush)
#define metrics_audiostream_data(__i0)  telemetry_log_event_i(METRIC_ID_audio_stream_data, __i0)

/* audio encoder events. */
#define metrics_encoder_init()  telemetry_log_event_v(METRIC_ID_audio_encoder_init)
#define metrics_encoder_flush()  telemetry_log_event_v(METRIC_ID_audio_encoder_flush)
#define metrics_encoder_data(__i0)  telemetry_log_event_i(METRIC_ID_audio_encoder_data, __i0)

/* KWS enable/disable events. */
#define metrics_kws_enabled()  telemetry_log_event_s(METRIC_ID_kws, "enabled")
#define metrics_kws_disabled()  telemetry_log_event_s(METRIC_ID_kws, "disabled")

/* KWS detection event. */
#define metrics_kws_detected(__i0)      telemetry_log_event2_i(METRIC_ID_kws, "detected", __i0)

/* KWS rejection event. */
#define metrics_kws_rejected(__i0)      telemetry_log_event2_i(METRIC_ID_kws, "rejected", __i0)

/* The KWS confidence level. */
#define metrics_kws_confidence(__d0)    telemetry_log_event2_d(METRIC_ID_kws, "confidence", __d0)

/* The KWS threshold. */
#define metrics_kws_threshold(__d0)     telemetry_log_event2_d(METRIC_ID_kws, "threshold", __d0)

/* The KWS model version. */
#define metrics_kws_version(__s0)       telemetry_log_event2_s(METRIC_ID_kws, "version", __s0)

/* An incoming call */
#define metrics_telephony_incomingcall()    telemetry_log_event_v(METRIC_ID_telephony_incomingcall)

/* A call has been accepted. */
#define metrics_telephony_accept()    telemetry_log_event_v(METRIC_ID_telephony_accept)

/* Telephony services have been initialized. */
#define metrics_telephony_initialize()    telemetry_log_event_v(METRIC_ID_telephony_initialize)

/* Telephony services have been shutdown. */
#define metrics_telephony_shutdown()    telemetry_log_event_v(METRIC_ID_telephony_shutdown)

/* A call has been put on hold. */
#define metrics_telephony_hold()    telemetry_log_event_v(METRIC_ID_telephony_hold)

/* A call has been resumed. */
#define metrics_telephony_resume()    telemetry_log_event_v(METRIC_ID_telephony_resume)

/* This event can be correlated to the server-side timer events using the "id"
key.  It's used to judge the health of timers -- for example, we use these
events to judge how many of the timers that are created in the service
actually fire on the client. */
#define metrics_timer_event(__event, __timer_id, __error_code) \
    telemetry_log_event(METRIC_ID_timer_event, "ssl", "event", __event, "id", __timer_id, "error", __error_code)

/* This event can be correlated to the server-side alarm events using the "id"
   key.  It's used to judge the health of alarms -- for example, we use these
   events to judge how many of the alarms that are created in the service
   actually fire on the client. */
#define metrics_alarm_event(__event, __alarm_id, __error_code) \
    telemetry_log_event(METRIC_ID_alarm_event, "ssl", "event", __event, "id", __alarm_id, "error", __error_code)

/* CDP initialization succeed or fail. */
#define metrics_cdp_initialize(__result)    telemetry_log_event(METRIC_ID_cdp_initialize, "l", "result", __result)

/* CDP get device thumbprint succeed or fail. */
#define metrics_cdp_device_thumbprint(__result, __thumbprint) \
    telemetry_log_event(METRIC_ID_cdp_device_thumbprint, "sls", \
        "version", "2", "result", __result, "thumbprint", __thumbprint)

/* Register notification callback to WNS client. */
#define metrics_notification_register_callback(__channel_id) \
    telemetry_log_event(METRIC_ID_notification_wns_channel, "ss", \
        "state", "register_callback", "channel_id", __channel_id)

/* Create WNS channel for notification flow succeed or fail. */
#define metrics_notification_create_wns_channel(__result, __retry_count) \
    telemetry_log_event(METRIC_ID_notification_wns_channel, "sll", \
        "state", "create_channel", "result", __result, "retry_count", __retry_count)

/* Refresh WNS channel for notification flow succeed or fail. */
#define metrics_notification_refresh_wns_channel(__result, __retry_count, __channel_id) \
    telemetry_log_event(METRIC_ID_notification_wns_channel, "slls", \
        "state", "renew_channel", "result", __result, "retry_count", __retry_count, "channel_id", __channel_id)

/* Status in WNS callback. */
#define metrics_notification_create_wns_channel_callback(__status, __error_code, __retry_count, __channel_id, __channel_uri) \
    telemetry_log_event(METRIC_ID_notification_wns_channel, "slllss", \
        "state", "create_channel_callback", "status", __status, "error_code", __error_code, "retry_count", __retry_count, \
        "channel_id", __channel_id, "channel_uri", __channel_uri)

/* Add notification registration event to speech context. */
#define metrics_notification_registration(__state, __channel_uri) \
    telemetry_log_event(METRIC_ID_notification_registration, "ls", "state", __state, "channel_uri", __channel_uri)

/* Incoming WNS message which is notification. */
#define metrics_notification_incoming_message(__message_id, __experience_id) \
    telemetry_log_event(METRIC_ID_notification_incoming_message, "ss", \
        "message_id", __message_id, "experience_id", __experience_id)

/* A tap event is being sent to the service, in response to a WNS message. */
#define metrics_notification_tap(__message_id) \
    telemetry_log_event(METRIC_ID_notification_tap, "s", "message_id", __message_id)

/*
 * Incoming WNS message which is CCS.
 * timestamp_check: 1 for pass, -1 for not pass and dropped, 0 for not check.
 */
#define metrics_ccs_incoming_message(__timestamp_check) \
    telemetry_log_event(METRIC_ID_ccs_incoming_message, "l", "timestamp_check", __timestamp_check);

/*
 * An uploadLog command was received.  If result is negative, then the log
 * upload failed.  Otherwise the result is defined by the OEM.
 *
 * The beginning of the upload can be estimated by looking for the logId in
 * metrics_ccs_command.
 */
#define metrics_upload_log(__log_id, __options, __result) \
    telemetry_log_event(METRIC_ID_upload_log, "sl", "log_id", __log_id, "log_options", __options, "result", __result)

/* FSM state transition happened. */
#define metrics_system_state_transition(__state, __event) \
    telemetry_log_event(METRIC_ID_system_state_transition, "ls", "state", __state, "event", __event)

/* Refresh tokens */
#define metrics_token_refresh(__scope, __result, __errormsg) \
    telemetry_log_event(METRIC_ID_token_refresh, "sls", "scope", __scope, "result", __result, "errormsg", __errormsg)

/* Get tokens */
#define metrics_token_get(__scope, __result) \
    telemetry_log_event(METRIC_ID_token_get, "sl", "scope", __scope, "result", __result)

/* OOBE: before starting fetch tokens */
#define metrics_oobe_start_fetch_token(__transfer_code) \
    telemetry_log_event(METRIC_ID_oobe, "ss", "state", "start_fetch_token", "transfer_code", __transfer_code)

/* OOBE: CDP initialization result */
#define metrics_oobe_cdp_initialize(__transfer_code, __result) \
    telemetry_log_event(METRIC_ID_oobe, "ssl", "state", "cdp_initialize", "transfer_code", __transfer_code, "result", __result)

/* OOBE: Cortana says we are all set */
#define metrics_oobe_complete(__transfer_code) \
    telemetry_log_event(METRIC_ID_oobe, "ss", "state", "complete", "transfer_code", __transfer_code)

/* OOBE: Cortana misssing some token */
#define metrics_oobe_token_missing(__transfer_code) \
    telemetry_log_event(METRIC_ID_oobe, "ss", "state", "no_token", "transfer_code", __transfer_code)

/* OOBE: OOBE sound play status */
#define metrics_oobe_sound_play(__sound, __state, __result) \
    telemetry_log_event(METRIC_ID_oobe_sound_play, "sss", "sound", __sound, "state", __state, "result", __result)

/* Music playback requested */
#define metrics_audioPlayer_play_request(__url, __provider, __music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_play_request, "sss", "url", __url, "provider", __provider, music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_play(__url, __music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_play, "ss", "url", __url, music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_stop(__music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_stop, "s", music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_pause(__music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_pause, "s", music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_resume(__music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_resume, "s", music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_prefetch_request(__music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_prefetch_request, "s", music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_prefetch_complete(__music_session_id) telemetry_log_event(METRIC_ID_audioPlayer_prefetch_complete, "s", music_session_id_arg, __music_session_id)

/* Music playback requested */
#define metrics_audioPlayer_buffer(__music_session_id, __buffer_session_id) telemetry_log_event(METRIC_ID_audioPlayer_buffer, "sl", music_session_id_arg, __music_session_id, buffer_session_id_arg, __buffer_session_id)

/* Music playback requested */
#define metrics_audioPlayer_buffer_complete(__music_session_id, __buffer_session_id, __provider, __buffer_duration) telemetry_log_event(METRIC_ID_audioPlayer_buffer_complete, "slsl", music_session_id_arg, __music_session_id, buffer_session_id_arg, __buffer_session_id, "provider", __provider, "duration", (long)(__buffer_duration))

//Metrics for any action start.
#define metrics_action_start(__action_name) telemetry_log_event(METRIC_ID_ActionStart, "s", action_name_arg, __action_name)

//Metrics for headers we recieve from service.
#define metrics_audioPlayer_headers(__music_session_id, __uri, __redirect_uri,  __http_status_code, __headers) \
    telemetry_log_event(METRIC_ID_audioPlayer_headers, "sssls", \
        music_session_id_arg, __music_session_id, "uri", __uri, "redirect_uri", __redirect_uri, "http_status_code",(long)(__http_status_code), "headers", __headers);

/*Metrics for playback error from gstreamer */
#define metrics_audioPlayer_playbackerror(__music_session_id, __debugInfo, __errorCode, __errorMessage) telemetry_log_event(METRIC_ID_audioPlayer_playbackerror, "ssls", music_session_id_arg, __music_session_id, "debug_info", __debugInfo, "errorCode", (long)(__errorCode), "errorMessage", __errorMessage);

// An error occurred while handling a Spotify callback.
#define metrics_spotify_error(__api_name, __error, __is_logged_in, __has_device_id) \
    telemetry_log_event(METRIC_ID_spotify_error, \
        "slll", "apiName", __api_name, \
        "errorCode", (long)(__error), "isLoggedIn", (long)(__is_logged_in), \
        "hasDeviceId", (long)(__has_device_id))

/* tts decode error */
#define metrics_tts_decode_error(__result) telemetry_log_event(METRIC_ID_tts_decode_error, "l", "result", __result)

/* tts audio fall back to non-streaming */
#define metrics_tts_audio_nonstreaming(__firstblocktime) telemetry_log_event(METRIC_ID_tts_audio_nonstreaming, "l", "firstblocktime", __firstblocktime)

/* tts audio buffer underrun */
#define metrics_tts_audio_buffer_underrun(__delaytime) telemetry_log_event(METRIC_ID_tts_audio_buffer_underrun, "l", "delaytime", __delaytime)

/* tts audio length */
#define metrics_tts_audio_length(__audiolength, __playtime) telemetry_log_event(METRIC_ID_tts_audio_length, "ll", "audiolength", __audiolength, "approximate_played", __playtime)

 /* tts first chunk */
#define metrics_tts_first_chunk() telemetry_log_event_v(METRIC_ID_tts_first_chunk)

 /* tts last chunk */
#define metrics_tts_last_chunk() telemetry_log_event_v(METRIC_ID_tts_last_chunk)

/* A conflict was detected -- the process will likely be terminated */
#define metrics_hang_detected(__type, __duration) \
    telemetry_log_event(METRIC_ID_hang_detected, "sl", "type", __type, "duration_ms", (long)(__duration))

/* A stall was detected -- a "stall" happens at a lower threshold than a "h*ng" */
#define metrics_stall(__type, __duration) \
    telemetry_log_event(METRIC_ID_stall, "sl", "type", __type, "duration_ms", (long)(__duration))

/* This is logged when Cortana starts up and wasn't previously down.  It
 * contains the time since the device booted, in seconds. */
#define metrics_boottime(__duration) telemetry_log_event_i(METRIC_ID_boottime, __duration)

/* This is logged when Cortana starts up and contains the amount of time that
 * Cortana was down.  If Cortana was not down, then this records 0.  This
 * duration is in milliseconds. */
#define metrics_downtime(__duration) telemetry_log_event(METRIC_ID_downtime, "d", "downtime", (double)(__duration))

/* This is logged periodically and used to generate a mean time to failure
 * metric.  This duration is in minutes. */
#define metrics_uptime(__duration) telemetry_log_event_i(METRIC_ID_uptime, __duration)

/*
 * The following events are used to gather resource usage information about the
 * Cortana process.  The bucket size doesn't change in a process' lifetime.
 * See resource_monitor.c for more information.
 *
 * Each resource has a 'change' event and a 'report' event.  To get a summary
 * of resource usage, first pick the event with the latest time in a given process'
 * session, and keep it if it's a 'report' event.  The event time and session
 * ID are added by the telemetry platform (e.g. Aria).  Rename the "value"
 * property to "old_value" for the following step.
 *
 * Add to the above a union of all of the 'change' events.  Then summarize the
 * sum of "duration" by "old_value / bucket_size".
 */
#define metrics_cpu_change(__duration_seconds, __old_cpu_percent, __cpu_percent, __bucket_size_percent) \
    telemetry_log_event(METRIC_ID_resource_report, "sllll", \
        kEvent_name_key, "cpu_change", \
        "duration", (long)(__duration_seconds), \
        "old_value", (long)(__old_cpu_percent), /* Assume a single core */ \
        kEvent_value, (long)(__cpu_percent), /* Assume a single core */ \
        "bucket_size", (long)(__bucket_size_percent))
#define metrics_cpu_report(__duration_seconds, __cpu_percent, __bucket_size_percent) \
    telemetry_log_event(METRIC_ID_resource_report, "slll", \
        kEvent_name_key, "cpu_report", \
        "duration", (long)(__duration_seconds), \
        kEvent_value, (long)(__cpu_percent), /* Assume a single core */ \
        "bucket_size", (long)(__bucket_size_percent))
#define metrics_vsize_change(__duration_seconds, __old_vsize_KB, __vsize_KB, __bucket_size_KB) \
    telemetry_log_event(METRIC_ID_resource_report, "sllll", \
        kEvent_name_key, "vsize_change", \
        "duration", (long)(__duration_seconds), \
        "old_value", (long)(__old_vsize_KB), \
        kEvent_value, (long)(__vsize_KB), \
        "bucket_size", (long)(__bucket_size_KB))
#define metrics_vsize_report(__duration_seconds, __vsize_KB, __bucket_size_KB) \
    telemetry_log_event(METRIC_ID_resource_report, "slll", \
        kEvent_name_key, "vsize_report", \
        "duration", (long)(__duration_seconds), \
        kEvent_value, (long)(__vsize_KB), \
        "bucket_size", (long)(__bucket_size_KB))
#define metrics_rsize_report(__resident_KB) \
    telemetry_log_event(METRIC_ID_resource_report, "sl", \
        kEvent_name_key, "rsize_report", kEvent_value, __resident_KB)

/* These events draw a timeline for boot */
#define metrics_boot_phase(__phase) telemetry_log_perf_event(METRIC_ID_boot_phase, "s", "phase", __phase)

#define metrics_skype_outgoing_call_skypeid(__callid)   telemetry_log_event_s(METRIC_ID_skype_outgoing_call_skypeid, __callid)
#define metrics_skype_incoming_call_skypeid(__callid)   telemetry_log_event_s(METRIC_ID_skype_incoming_call_skypeid, __callid)
#define metrics_skype_call_state(__state, __callid)     telemetry_log_event(METRIC_ID_skype_call_state, "ss", "state", __state, "callid", __callid)
#define metrics_skype_login_failed(__errmsg)            telemetry_log_event_s(METRIC_ID_metrics_skype_login_failed, __errmsg)

/*logs when we receive an incoming WNS message from CCS 
 * Sample command:
 *{"_type": "VolumeControl", "id": "skill:audioPlayer", "action": "setVolume", "amount": 84, "sessionId": "36bb684f-b365-4920-80b2-00e3b618b408"  }*/
#define metrics_ccs_command(__command)            telemetry_log_event(METRIC_ID_ccs_command_received, "s", "command", __command)

/* Metric for Wifi states and event changes. */
#define metrics_wifiinfo(__rate, __qual, __level, __noise) telemetry_log_event(METRIC_ID_wifiinfo, "llll", "rate", (long)(__rate), "qual", (long)(__qual), "level", (long)(__level), "noise", (long)(__noise))

/* Metric for wifi state changes */
#define metrics_wifistate(__state)           telemetry_log_event_i(METRIC_ID_wifistate, __state)

/* Metric for time to connect to Wifi */
#define metrics_wificonnected(__latencyms)   telemetry_log_event_i(METRIC_ID_wificonnected, __latencyms)

/* Metric for NTP syncronization and latency */
#define metrics_ntpsync(__latencyms)         telemetry_log_event_i(METRIC_ID_ntpsync, __latencyms)

/* Metric for DNS event changes. */
#define metrics_dnsinfo(__addr, __error)     telemetry_log_event(METRIC_ID_dnsinfo, "ll", "addr", (long)(__addr), "error", (long)(__error))

/* Metric for link event changes. */
#define metrics_netlink(__type, __value)     telemetry_log_event(METRIC_ID_netlinkinfo, "ll", "type", (long)(__type), kEvent_value, (long)(__value))

/*
* This is logged when a user physically interacts with the device. eg. Press bluetooth button, turn the volume ring, etc..
* @param __action : the physical interaction eg: bluetoothOn, VolumeUp, VolumeDown, MuteOn
* @param __stateInfo : an optional JSON representing the state of the device depending on the action. For volume, could be the current volume. This Contract has to be decided for every device manufacturer.
*/
#define metrics_device_physical_interaction(__action, __stateInfo)  telemetry_log_event2_s(METRIC_ID_oem_event, __action, __stateInfo);

#ifdef __cplusplus
}
#endif
