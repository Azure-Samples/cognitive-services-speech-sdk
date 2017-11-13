// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PRIVATE_IOT_CORTANA_SDK_H
#define PRIVATE_IOT_CORTANA_SDK_H

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <stdlib.h>
#include <memory.h>
#ifdef __linux__
#include <unistd.h>
#include <errno.h>
#endif
#include "azure_c_shared_utility/gballoc.h"
#include "iot-cortana-sdk.h"
#include "private_audio_manager.h"
#include "metrics.h"
#include "timeex.h"
#include "azure_c_shared_utility/audio_sys.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/httpheaders.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/ring_buffer.h"
#include "azure_c_shared_utility/uhttp.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/wsio.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/list.h"
#include "azure_c_shared_utility/queue.h"
#include "azure_c_shared_utility/condition.h"
#include "azure_c_shared_utility/base64.h"
#include "HttpListener.h"

#if !defined(IOT_CORTANA_NO_COMPANION)
#define ENABLE_COMPANION
#endif

#if defined ENABLE_COMPANION
#include "companion.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CORTANASDK_BUILD_HASH)
    #if defined(_MSC_VER)
        #if defined(_WIN32)
            #define CORTANASDK_BUILD_HASH       "Windows;Win32;"
            #define CORTANASDK_PLATFORM_NAME    "Win32"
        #elif defined(_WIN64)
            #define CORTANASDK_BUILD_HASH       "Windows;Win64;"
            #define CORTANASDK_PLATFORM_NAME    "Win64"
        #elif defined(_WINARM)
            #define CORTANASDK_BUILD_HASH       "Windows;ARM;"
            #define CORTANASDK_PLATFORM_NAME    "ARM"
        #else
            #define CORTANASDK_BUILD_HASH       "Windows;"
        #endif
    #elif defined(__unix__)
        #define CORTANASDK_BUILD_HASH           "Linux;"
    #else
        #define CORTANASDK_BUILD_HASH           ""
    #endif
#endif  // !defined(CORTANASDK_BUILD_HASH)

#if !defined(CORTANASDK_OEM_MANUFACTURER)
#define CORTANASDK_OEM_MANUFACTURER             "Microsoft"
#endif
#if !defined(CORTANASDK_OEM_MODEL) && defined(CORTANASDK_PLATFORM_NAME)
#define CORTANASDK_OEM_MODEL                    CORTANASDK_PLATFORM_NAME
#endif
#if !defined(CORTANASDK_OEM_VERSION) && defined(CORTANASDK_PLATFORM_VERSION)
#define CORTANASDK_OEM_VERSION                  CORTANASDK_PLATFORM_VERSION
#endif

#if !defined(CORTANASDK_PLATFORM)
    #if defined(_MSC_VER)
        #if defined(_WIN32)
            #define CORTANASDK_PLATFORM         "Windows-Win32"
        #elif defined(_WIN64)
            #define CORTANASDK_PLATFORM         "Windows-Win64"
        #elif defined(_WINARM)
            #define CORTANASDK_PLATFORM         "Windows-ARM"
        #else
            #define CORTANASDK_PLATFORM         "Windows"
        #endif
    #elif defined(__unix__)
        #define CORTANASDK_PLATFORM             "Linux"
    #endif
#endif

#if !defined(CORTANASDK_ENVIRONMENT)
    #define CORTANASDK_ENVIRONMENT              "Home"
#endif

// Hardcoding for now, but this will need to come from Cortana login.
#define CORTANASDK_DEFAULT_LANGUAGE             "en-US"

#if !defined(CORTANA_SOUND_VOICE_PATH)
#define CORTANA_SOUND_VOICE_PATH    ""
#endif

#if !defined(CORTANA_SOUND_PATH)
#define CORTANA_SOUND_PATH          ""
#endif

#define USE_SILK_DECODE

#define TRANSPORT_SPEECH_INDEX  0
#define TRANSPORT_REQ_COUNT     1

#if defined _MSC_VER
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, __VA_ARGS__)
#else
#define PROTOCOL_VIOLATION(__fmt, ...)  LogError("ProtocolViolation:" __fmt, ##__VA_ARGS__)
#endif

#define INVALID_STREAM_ID	-1

#define ANSWER_TIMEOUT_MS   15000
#define AUDIO_SAMPLE_RATE   16000

#define TIMEOUT_STREAMCHUNK_READING 5000

// an internal earcon type used to play mandatory earcons regardless of user 
// setting.
#define CORTANA_AUDIO_TYPE_SYSTEM_EARCON    ((enum cortana_audio_type)(CORTANA_AUDIO_TYPE_LAST + 1))

// The CORTANA_PLAY_ALLSOUNDS flag is used to play all Cortana sounds.
#if !defined(EXTERNAL_MUSIC_MANAGER) && !defined(CORTANA_PLAY_ALLSOUNDS)
#define CORTANA_PLAY_ALLSOUNDS
#endif

// Some sounds contain voice and others don't.  The CORTANA_USE_SYS_EFFECTS_ONLY 
// flag is used to default to non-voice earcons.
#if !defined(CORTANA_USE_SYS_EFFECTS_ONLY)
#define MAKESOUNDNAME(__name)   "C_" __name
#else
#define MAKESOUNDNAME(__name)   "S_" __name
#endif

#if defined(UNIT_TESTS)
#define STATIC_UT_EMPTY
#else
#define STATIC_UT_EMPTY static
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(__a) (sizeof(__a) / sizeof(__a[0]))
#endif

static char const kDowntimeTrackerKey[] = "downtime_tracker";
extern const char* kEarconSetting;
extern const char* kDeviceNameKey;

typedef struct _IOBUFFER
{
    LOCK_HANDLE hLocker;
    COND_HANDLE hDataCondition; // signals that new data is available
    BUFFER_HANDLE hBuffer;
    int nReadBytes;
    int nTotalBytes;
    int nCompleted;
    int hasNewCome; // indicates that IOBUFFER_Write() was called and new data is available
    int nRef; // reference count for this IO Buffer.  The buffer is released when the count hits 0.
    int nActiveReaders;  // number of threads currently waiting on hDataCondition
} IOBUFFER;

IOBUFFER* IOBUFFER_NEW();
void IOBUFFER_DELETE(IOBUFFER* pIoBuffer);
void IOBUFFER_AddRef(IOBUFFER* pIoBuffer);
void IOBUFFER_Reset(IOBUFFER* pIoBuffer);

int IOBUFFER_GetTotalBytes(IOBUFFER* pIoBuffer);
int IOBUFFER_GetUnReadBytes(IOBUFFER* pIoBuffer);
int IOBUFFER_WaitForNewBytes(IOBUFFER* pIoBuffer, int timeout);

int IOBUFFER_PeekRead(IOBUFFER* pIoBuffer, void* pDstBuff, int nOffset, int nBytes, int timeout);
int IOBUFFER_Read(IOBUFFER* pIoBuffer, void* pDstBuff, int nOffset, int nBytes, int timeout);
int IOBUFFER_Write(IOBUFFER* pIoBuffer, const void* pSrcBuff, int nOffset, int nBytes);

typedef struct _ContentBuffers
{
    BUFFER_HANDLE hBufferHandler;
    IOBUFFER* pIoBuffer;
} ContentBuffers;

void cortana_system_fsm_dowork();
struct NETDIAG_CONTEXT;
typedef struct NETDIAG_CONTEXT* NETDIAG_HANDLE;

NETDIAG_HANDLE  netdiag_create();
void            netdiag_destroy(NETDIAG_HANDLE handle);
int             netdiag_time_synced(NETDIAG_HANDLE handle);

///////////////////////////////////////////////////////////////////////////////

/**
 * Describes a callback for an agent event.  These are mapped to callbacks in
 * skills/agent.c.
 */
typedef enum _CORTANA_EVENT
{
    CORTANA_EVENT_HEARTBEAT,
    CORTANA_EVENT_BLUETOOTH_PAIR,
    CORTANA_EVENT_DELIGHT_ME,
    CORTANA_EVENT_COMMUNICATION_INCOMING_CALL,
    CORTANA_EVENT_AGENT_CALLBACK,
    CORTANA_EVENT_NOTIFICATION_TAP,
    CORTANA_EVENT_NOTIFICATION_REGISTER_OR_RENEW,
    CORTANA_EVENT_MUSIC,
    CORTANA_EVENT_SPOTIFY_LOGIN_STATE_CHANGED,
    CORTANA_EVENT_SPEECH_CANCEL_OUTPUT,
    CORTANA_EVENT_SPEECH_CANCEL_LISTENING,
    CORTANA_EVENT_DEVICE_START,
} CORTANA_EVENT;

#ifdef ENABLE_RESOURCE_MONITOR
/*
 * Summarizes the resource usage of a process.
 */
typedef struct _PROCESS_RESOURCE_USAGE
{
    // The timestamp at which this usage was queried, in milliseconds.
    uint64_t time;

    // Virtual memory size in kilobytes.
    unsigned long vsize;

    // Number of kilobytes that the process has resident in memory.
    long rsize;

    // Cumulative CPU usage in milliseconds.
    uint64_t cputime;
} PROCESS_RESOURCE_USAGE;

/*
 * Queries the resource usage of the current process.
 * @param usage On success, this is filled in with the resource usage.
 * @return Zero for success, non-zero for failure.
 */
int get_process_resource_usage(PROCESS_RESOURCE_USAGE* usage);

typedef struct _RESOURCE_MONITOR_CONTEXT
{
    // The last time when a resource usage report was logged to telemetry.
    uint64_t last_report_time;

    // A snapshot of the resource usage of this process.
    PROCESS_RESOURCE_USAGE self_usage;

    // The time at which memory was first found to be in its bucket.
    uint64_t mem_bucket_entry_time;

    // The time at which CPU was first found to be in its bucket.
    uint64_t cpu_bucket_entry_time;

    // The CPU usage in percent.  This doesn't account for the number of CPU
    // cores, so fully using 2 CPU cores would result in 200% here.
    int cpu_usage_percent;
} RESOURCE_MONITOR_CONTEXT;

/*
 * The workhorse for the resource monitor.  This should be called periodically,
 * as described in resource_monitor.c.
 * @param handle The Cortana handle.
 * @return Zero if successful; non-zero if not.
 */
int resource_monitor_dowork(CORTANA_HANDLE handle);

// This is the bucket size for virtual memory size, in bytes.  See
// resource_monitor.c.
#ifndef RESOURCE_MONITOR_VSIZE_BUCKET
#define RESOURCE_MONITOR_VSIZE_BUCKET (100 * 1000) // 100 MB
#endif

// If this is defined, then the resource monitor will crash the process if it
// sees a memory size above this threshold.
#ifdef RESOURCE_MONITOR_VSIZE_CRASH_THRESHOLD
#endif

// This is the bucket size for CPU usage, in bytes.  See resource_monitor.c.
#ifndef RESOURCE_MONITOR_CPU_PERCENT_BUCKET
#define RESOURCE_MONITOR_CPU_PERCENT_BUCKET 20
#endif
#endif // ENABLE_RESOURCE_MONITOR

/**
 * Location Info  represents a structure used for location information sent down to the device during OOBE.
 */
typedef struct _LOCATION_INFO
{
    double latitude;
    double longitude;
#define MAX_TIMEZONE_LEN  100
    char   timezone[MAX_TIMEZONE_LEN];
} LOCATION_INFO;

/*
 * Spotify Context represents a structure used to represent the spotify context sent during login action
 */
typedef struct _SPOTIFY_CONTEXT
{
    unsigned short blobReceived;
#define MAX_SPOTIFY_LOGIN_BLOB_SIZE 200
    char cachedBlob[MAX_SPOTIFY_LOGIN_BLOB_SIZE];
} SPOTIFY_CONTEXT;

/**
 * These timestamps are snapped all at once during a health check.
 */
typedef struct _CORTANA_HEALTH_CONTEXT
{
    // This tick count is updated each time our message loop iterates to
    // process a message. 
    uint64_t                    LastMsgCheckTime;
    uint64_t                    LastListenTime;

    // If this variable is nonzero, then this is the tick count at which CDP
    // initialization started, and CdpInitThread is valid.
    uint64_t                    CdpInitStartTime;

    THREADAPI_ID                MsgLoopThread;
    THREADAPI_ID                AudioThread;
    THREADAPI_ID                CdpInitThread; // see CdpInitStartTime.
} CORTANA_HEALTH_CONTEXT;

struct _AGENT_QUEUED_EVENT;
typedef struct _AGENT_QUEUED_EVENT AGENT_QUEUED_EVENT;

/**
 * This structure summarizes state for Cortana Agent events.
 */
typedef struct _CORTANA_EVENT_CONTEXT
{
    LIST_HANDLE                 PendingEvents;

    // This event that is being sent now.  The event is tracked so that we can
    // retry if sending fails.
    AGENT_QUEUED_EVENT*  InProgressEvent;

#ifdef AGENT_EVENT_UNIT_TEST
    // The unit tests define their own event serializers and callback
    // functions.
    struct _CORTANA_AGENT_EVENT_INFO const* EventInfo;
    size_t EventInfoCount;
#endif // AGENT_EVENT_UNIT_TEST
} CORTANA_EVENT_CONTEXT;

/*
 * Store information for handling notifications.  Notifications are managed in
 * notification_flow.c and skills/notifications.c.
 */
typedef struct _NOTIFICATION_CONTEXT
{
    STRING_HANDLE LastMessageId;
} NOTIFICATION_CONTEXT;

/**
*enum to set the earcon setting state.
*/
typedef enum _EARCON_SETTING
{
    EARCON_SETTING_DEFAULT = 0,
    EARCON_SETTING_ENABLED,
    EARCON_SETTING_DISABLED
} EARCON_SETTING;

/**
* enum ID for each KWS instance
*/
typedef enum KWS_ID
{
    KWS_CORTANA = 0,
    KWS_SKYPE   = 1
} KWS_ID;


typedef struct KWS_CONTEXT
{
    SPEECH_HANDLE mKWS;
    int           fKWSReset;
    int           AudioSamples_KWS;
} KWS_CONTEXT;

/**
 * The SPEECH_CONTEXT type represents a structure used for all Cortana+Speech related context.
 */
typedef struct _SPEECH_CONTEXT
{
    // DataLock protects some fields of this structure that may be read and
    // written to on multiple threads.  Only hold the lock while modifying
    // data.  Do not hold it while making call-outs to other functions.
    LOCK_HANDLE                 DataLock;
    CORTANA_CALLBACKS*          mCallbacks;
    void*                       mContext;
#define MAX_DEVICES             ((int)CORTANA_AUDIO_TYPE_LAST)
    struct
    {
        AUDIO_SYS_HANDLE    hDevice;
        uint8_t             index;
        long                initvol;
    } mAudioConfig[MAX_DEVICES];
    size_t                      mAudioOffset;
    LOCK_HANDLE                 mSpeechRequestLock;
    TRANSPORT_HANDLE            mTransportRequest[TRANSPORT_REQ_COUNT];
    DNS_CACHE_HANDLE            hDnsCache;

    // KWS instances
    KWS_CONTEXT*                pKWS;
    // number of KWS instances
    size_t                      numKWS;
    int                         fKWSListening;
    int                         KWTriggered;

    int16_t*                    dumpBuffer;
    double                      kw_confidence;

    // lock between KWS detection callbacks
    LOCK_HANDLE                 mDetectionLock;

    struct
    {
        enum cortana_telephony_state state; // current telephony state
    } telephony;

    // AudioContext for each channel we capture
    ring_buffer_t**             pAudioContext;

    double                      kw_startoffset;
    double                      kw_duration;

    uint32_t                    mShuttingDown;
    int                         TTSCount; // TODO: Remove me once we have full parity with old system.
    AUDIO_STATE                 AudioOutputState;
    int                         TTSOutput;
    TIMER_HANDLE                hTurnTimeout;
	int                         PendingListen;
    uint32_t                    AudioSamples_Dropped;
    uint32_t                    AudioSamples_Reco;
    // Timer used for firing the KWS active earcon.  
    // For instance this timer will not fire in the OneShot query case.
    TIMER_HANDLE                hThinkingEarcon;
    CORTANA_HEALTH_CONTEXT      HealthContext;
    LOCK_HANDLE                 HealthLock; // Protects HealthContext.
#if defined(IOT_CORTANA_CAPTURE_AUDIO)
    FILE*                       fpAudioCapture;
#endif
    CORTANA_EVENT_CONTEXT       EventContext;
    int                         muted;
    // The current turn may include audio responses.
    int                         AudioTurn;
    // The current turn includes speech input.
    int                         SpeechTurn;
    LIST_HANDLE                 hPlayList;
    LOCK_HANDLE                 PlaylistLock;
    // This tick count is set when the SPEECH_CONTEXT is created.  It is used
    // for metrics.
    uint64_t                    CreateTime;
    LOCATION_INFO               LocationInfo;
    CORTANA_ERROR               lastCortanaError;
    int                         ResponseRecv;
    EARCON_SETTING              EarconSetting;
    // signal to skip next idle state transition for async events.
    int                         IgnoreNextIdleState;
#ifdef ENABLE_RESOURCE_MONITOR
    RESOURCE_MONITOR_CONTEXT    ResourceMonitorContext;
#endif
    SPOTIFY_STATE_INFO          SpotifyState;
    SPOTIFY_CONTEXT             SpotifyContext;
    NETDIAG_HANDLE              hNetDiag;
    char                        deviceName[100];
    NOTIFICATION_CONTEXT        NotificationContext;
    TIMER_HANDLE                hVolumeTimer;
    STRING_HANDLE               CallbackCookie;
    struct
    {
        enum cortana_bluetooth_state state;
    } bluetooth;

    struct
    {
        uint32_t                current_volume;
    } volume;
    
    struct
    {
        CORTANA_STATE           last_cortana_state;
        int                     audio_control_states;
    } ux;
    EventQueue event_queue; 
    TokenStore token_store; 

    /* The following will be removed and placed outside of the core soon */
    CortanaAuth cortana_auth; 
#if defined ENABLE_COMPANION
    CompanionApp companion_app;
#endif
} SPEECH_CONTEXT;

#define mSpeechRequest              mTransportRequest[TRANSPORT_SPEECH_INDEX]
#define CORTANA_IN_TURN(__sc)       (NULL != (__sc)->hTurnTimeout)
int tts_is_playing(SPEECH_CONTEXT* pSC);

struct _TRANSPORT_STREAM;
typedef struct _TRANSPORT_STREAM TRANSPORT_STREAM;
struct _TRANSPORT_PACKET;
typedef struct _TRANSPORT_PACKET TRANSPORT_PACKET;
typedef enum _TRANSPORT_STATE
{
    TRANSPORT_STATE_CLOSED = 0,
    TRANSPORT_STATE_NETWORK_CHECK,
    TRANSPORT_STATE_NETWORK_CHECKING,
    TRANSPORT_STATE_OPENING,
    TRANSPORT_STATE_CONNECTED,
    TRANSPORT_STATE_SENT, // only used for HTTP
    // The request is being closed and will be re-opened.
    TRANSPORT_STATE_RESETTING,
} TRANSPORT_STATE;

typedef enum _TRANSPORT_STATE TRANSPORT_STATE;

/**
* The TRANSPORT_REQUEST type represents a structure used to for all transport
* related context.
*/
typedef struct _TRANSPORT_REQUEST
{
    union
    {
        struct
        {
            HTTP_CLIENT_HANDLE      hRequest;
            TLSIO_CONFIG            tls_io_config;
        } _http;

        struct
        {
            CONCRETE_IO_HANDLE      hWS;
            size_t                  pszPathLen;
            WSIO_CONFIG             config;
            bool                    chunksent;
            TRANSPORT_STREAM*       pStreamHead;
            BUFFER_HANDLE           hMetricData;
            size_t                  metricOffset;
            LOCK_HANDLE             metricLock;
        } _ws;
    } u1;

    const char*                  pszPath;
    PTRANSPORT_RESPONSE_CALLBACK OnRecvCallback;
    PTRANSPORT_ERROR             OnTransportError;
    HTTP_HEADERS_HANDLE          hHeaders;
    BUFFER_HANDLE                hResponseContent;
    bool                         fWS;
    bool                         fCompleted;
    bool                         fOpen;
    char*                        pszUrl;
    void*                        pContext;
    char                         requestId[37];
    char                         connectionId[37];
    uint8_t                      bRequestId[16];
    uint32_t                     streamId;
    bool                         fNeedAuthenticationHeader;
    TRANSPORT_STATE              state;
    LIST_HANDLE                  hQueue;
    DNS_CACHE_HANDLE             hDnsCache;
    uint64_t                     nConnectionTime;
    TokenStore                   token_store;
} TRANSPORT_REQUEST;

void transport_create_request_id(
    TRANSPORT_HANDLE hTransport);

const char* transport_get_request_id(
    TRANSPORT_HANDLE hTransport);

void transport_set_dns_cache(
    TRANSPORT_HANDLE hTransport,
    DNS_CACHE_HANDLE hDnsCache);

#define CONV_CONTEXT    SPEECH_CONTEXT
#define SPEECH_STREAM   SPEECH_CONTEXT

#define kPath           "Path"

/**
* enum to indicate the current state of skype call
*/
enum cortana_skype_state
{
    /**
    * State used to ensure that no call is active.
    */
    SKYPE_STATE_NONE,
    /**
    * State used to ensure that a skype call is ringing in.
    */
    SKYPE_STATE_RINGING,
    /** 
    *State used when an outgoing skype call is connecting (ie) before the callee hears the ring.
    */
    SKYPE_STATE_CONNECTING,
    /**
    * State used when an outgoing skype call is ringing (ie) after the callee receives the ring.
    */
    SKYPE_STATE_DIALING,
    /**
    * State used to ensure that a skype call is answered.
    */
    SKYPE_STATE_ANSWER,
    /**
    * State used to ensure that a skype call is ended.
    */
    SKYPE_STATE_ENDED,
    /**
    * State used to ensure that a skype call is failed.
    */
    SKYPE_STATE_FAILED,
    /**
    * State used when an ongoing call gets dropped
    */
    SKYPE_STATE_DROPPED,
    /**
    * State used to ensure that skype sounds are suspended.
    */
    SKYPE_STATE_SUSPENDED,
    /**
    * State used to ensure that skype sounds are resumed.
    */
    SKYPE_STATE_RESUMED,
    /**
    * State used when there are not enough skype credits to make a PSTN call
    */
    SKYPE_STATE_FAILED_PSTN_INSUFFICIENT_FUNDS,
    /**
    * State used to indicate that the PSTN call failed due to an invalid number
    */
    SKYPE_STATE_FAILED_PSTN_INVALID_NUMBER,
    /**
    * State uesd to indicate that the PSTN call failed due to the accout be locked
    */
    SKYPE_STATE_FAILED_PSTN_SKYPEOUT_ACCOUNT_BLOCKED,
    /**
    * State used when an unanswered incoming call ends or an incoming call is ignored
    */
    SKYPE_STATE_IGNORED,
    /**
    * State used when an outgoing call is not answered on the callee or ended by the callee
    */
    SKYPE_STATE_MISSED,
    /**
    * State used when an incoming call is answered on a different endpoint
    */
    SKYPE_STATE_IGNORED_EARCON,
    /**
    * State used to the indicate that the PSTN call failed because the subscription doesn't cover the destination
    */
    SKYPE_STATE_FAILED_PSTN_NO_SUBSCRIPTION_COVER,
};

/**
* cortana sdk callback for OnTelephonyState
*/
void cortana_OnTelephonyState(SPEECH_HANDLE handle, enum cortana_telephony_state state);

/**
* Sets the desired skype ringer state.
* @return A return code or zero if successful.
*/
int cortana_skype_setstate(SPEECH_HANDLE pContext, enum cortana_skype_state state);

/**
* Sends an incoming call event up to the Cortana cloud
* @param handle The Cortana handle.
*/
void cortana_incoming_call_event(
    CORTANA_HANDLE handle, const char* userId);

/**
* Sends an incoming call event up to the Cortana cloud
* @param handle The Cortana handle.
* @param hProperty Handle to parameters (calling platform, caller id, etc.).
*/
void cortana_incoming_call_event_with_propertybag(CORTANA_HANDLE handle, PROPERTYBAG_HANDLE hProperty);

SPEECH_RESULT Speech_Initialize(SPEECH_CONTEXT* pContext);

typedef void(*PCONTENT_ASYNCCOMPLETE_CALLBACK)(
    void*           pContext);

typedef SPEECH_RESULT(*PCONTENT_HANDLER_CALLBACK)(
    void*           pContext,
    const char *    pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext);

#define USE_BUFFER_SIZE	((size_t)-1)

SPEECH_RESULT
Content_Dispatch(
    void*           pContext,
    const char*     pszPath,
    const char*     pszMime,
    IOBUFFER*       pIoBuffer,
    BUFFER_HANDLE   hResponseContent,
    size_t          responseSize
);

SPEECH_RESULT Content_DispatchBuffer(
    void*        pContext,
    const char*  pszPath,
    const char*  pszMime,
    const void*  pBuffer,
    size_t       bufferSize);

SPEECH_RESULT Json_ResponseHandler(
    void*           pContext,
    const char*     pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext);

SPEECH_RESULT Text_ResponseHandler(
    void*           pContext,
    const char*     pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext);

SPEECH_RESULT AsyncAudioDecode(
    void*           pContext,
    const char*     pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext);

int AudioInput_Write(void* pContext, uint8_t* pBuffer, size_t byteToWrite);
int AudioInput_Write_Skype(void* pContext, uint8_t* pBuffer, size_t byteToWrite);
void AudioInput_StateChanged(void* pContext, AUDIO_STATE state);
void AudioOutput_StateChanged(void* pContext, AUDIO_STATE state);
void Audio_Error(void* pContext, AUDIO_ERROR error);

extern const char* kContent_Type;
extern const char  kContent_Json[];
extern const char* kString_version;

extern const char kApiPath_Speech_Hypothesis[];
extern const char kApiPath_Speech_Phrase[];
extern const char kApiPath_TurnStart[];
extern const char kApiPath_TurnEnd[];
extern const char kApiPath_SpeechEndDetected[];
extern const char kApiPath_SpeechStartDetected[];
extern const char kApiPath_Response[];

void OnSpeechPartialResult(
    SPEECH_CONTEXT* pSC,
    const char*     pszUtf8PartialResult);

int hexchar_to_int(char c);

/*
 * Returns zero if the tokens in the headers were already up to date, and
 * non-zero otherwise.
 */
int Authentication_AddCommonHeaders(
    HTTP_HEADERS_HANDLE hHeaders,
    void* pContext);

int HTTPHeaders_ParseHeaders(
    HTTP_HEADERS_HANDLE  hHeaders,
    const unsigned char* buffer,
    size_t               size);

void UpdateTurnTimeout(SPEECH_CONTEXT* pSC);

/*
 * Compares two timer handles and NULLs out the target timer if they match.
 * @return Returns 1 if the handles matched, and zero if the didn't.
 */
int CheckAndCancelTimer(
    TIMER_HANDLE    hTimer,
    TIMER_HANDLE*   phCurrentTimer);

// core skills
int skill_alarms(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_timers(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_timers_getcontext(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_alarms_getcontext(PROPERTYBAG_HANDLE   hProperty, void* pContext);
int skill_speechRecognizer(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_audioPlayer(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_deviceSettings(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_EndOfSpeech(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_TurnEnd(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_call(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_call_getcontext(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_diagnostics(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_text(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_agent(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_notification(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_bluetooth(PROPERTYBAG_HANDLE hProperty, void* pContext);
int skill_spotify(PROPERTYBAG_HANDLE hProperty, void* pContext);

STRING_HANDLE skill_serialize_context(void *pContext, int sendEvents);

int card_deserialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext);
int card_serialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext);
int audioplayer_serialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext);
int bluetooth_serialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext);
int speech_serialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext);
int spotify_serialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext);
// CDPInitialized is global variable store wether CDP is Initialized
extern bool CDPInitialized;

// transfer code is used as OOBE session id for telemetry
extern STRING_HANDLE gTransferCode;

void cortana_onerror(SPEECH_HANDLE handle, CORTANA_ERROR error);

/**
 * Called when external sounds need to adjust their volume because either
 * Cortana is listening or has something pertinent to say.
 */
void cortana_duck_setstate(SPEECH_HANDLE handle, enum cortana_ducking_state duckState);

/*
 * UX_KWS_THRESHOLD defines the local threshold that the client uses with the 
 * local keyword score to determine whether to use on-device keyword detection
 * or the cloud when handling some voice activation scenarios.
 *
 * The supported values for UX_KWS_THRESHOLD are a numeric range between 0 and 1. 
 * A value of 0 means the client shall always use the local keyword score to 
 * drive voice activation.  A value of 1 means that the second stage keyword 
 * detection in the cloud shall always be used drive voice activation.
 *
 * Having the threshold set too low will may cause some devices to activate too
 * often, also known as a 'false accept.'  Having the threshold set too high could 
 * be subjected to higher latencies due to the overall cost of connecting to the
 * cloud which may affect normal activations, also known as a 'correct accept.'
 * The current value defined for UX_KWS_THRESHOLD is one that has been
 * established as an ideal balance between the two modes of operation.
 */
#define UX_KWS_THRESHOLD    0.88f

/* The different modes that the listening state can be triggered.
 */
enum ux_listen_mode
{
    UX_LISTEN_USER = 0, /*!< Listening state has been activated manually by the user. */
    UX_LISTEN_KWS,      /*!< Listening state has been triggered by the local keyword spotter. */
    UX_LISTEN_SERVICE,  /*!< Listening state has been triggered by the service. */
};

/* Drives the UX required for the listening state.
 * @param handle The Cortana handle.
 * @param mode The listening mode.
 * @param reason The listening mode reason.
 */
void cortana_ux_onlisten(
    SPEECH_HANDLE        handle,
    enum ux_listen_mode  mode,
    CORTANA_REASON_STATE reason);

 /**
 * Initializes the cdp component.
 * @param handle The Cortana handle.
 */
void cortana_cdp_init(SPEECH_HANDLE handle, TokenStore token_store);

/**
 * Deinitializes the cdp.
 */
void cortana_cdp_shutdown();

/**
* Initializes the skype library and logs in to the user account.
* @param handle The cortana handle.
*/
void cortana_skype_init(SPEECH_HANDLE handle);

/**
* Logs out and shuts down skype
*/
void cortana_skype_shutdown();

/**
 * Starts a heartbeat that will tell the Cortana service that this device is active.
 */
int cortana_start_heartbeat(SPEECH_HANDLE handle);

/**
* Reset the heartbeat according to current system state.
*/
int cortana_reset_heartbeat(SPEECH_HANDLE handle);

/** 
 * Called when Cortana is starting up or after OOBE.
 */
int cortana_device_start(CORTANA_HANDLE hCortana);

/**
 * Reset the request ID on the connection.  We will ignore responses with the
 * old request ID.
 */
void cortana_reset_speech_request_id(SPEECH_CONTEXT* pSC);

/**
 * Gets a string for the given OEM property.  The string might come from the
 * Cortana callbacks or from defines.
 * @param handle The Cortana handle.
 * @param oemProperty The OEM property to query.
 * @param result The string result.
 * @return Zero if successful, non-zero otherwise.
 */
int cortana_get_oem_property(SPEECH_HANDLE handle, enum cortana_oem_property oemProperty, STRING_HANDLE result);

/**
 * Gets the current mute state
 * @param handle The Cortana handle.
 * @return 1 if Cortana is muted, or 0 if not muted.
 */
int cortana_get_mute(SPEECH_HANDLE handle);


int cortana_create_audio(
    SPEECH_HANDLE           handle,
    const char*             pszName);

AUDIO_RESULT audiomgr_get_volume(
    SPEECH_HANDLE           handle,
    enum cortana_audio_type group,
    long*                   volume);

AUDIO_RESULT audiomgr_duck_group(
    SPEECH_HANDLE               hCortana,
    enum cortana_ducking_state  duckState,
    enum cortana_audio_type     group
);

int audiomgr_adjust_volume(
    CORTANA_HANDLE                  hCortana,
    enum cortana_volume_change_type type,
    long                            volume);

int global_toggle_mute(
    CORTANA_HANDLE  hCortana);

#define INVALID_VOLUME -1

extern const char kAudioSetting_InitVolume[];
/**
* Runs Cortana to completion.  Note: this is a blocking call.
*/
void cortana_run_internal(CORTANA_HANDLE handle);

int cortana_setstate_internal(SPEECH_HANDLE        handle,
                              CORTANA_STATE        state,
                              CORTANA_REASON_STATE reason);

/**
 * Check if there are any events on the agent queue.
 * @param handle The Cortana handle.
 * @param state The system state.  Most events are only sent when we are in
 *  online mode.
 * @return Non-zero if there is an event in the queue, and zero otherwise.
 */
int agent_queue_has_event(
    CORTANA_HANDLE handle,
    cortana_system_state state);

/**
 * Set the skill ID and name of an event.
 * @param hProperty The property bag representing the event
 * @param pszEventId The skill ID, or NULL to ignore this argument.
 * @param pszEventName The event name, or NULL to ignore this argument.
 */
void agent_event_set_info(
    PROPERTYBAG_HANDLE hProperty,
    char const* pszEventId,
    char const* pszEventName);

/**
 * This callback is invoked when preparing an agent event, to fill the property
 * bag with any extra properties needed for the agent event.  The agent event
 * ID and name have been filled into the property bag already when this is
 * called.
 *
 * @param handle The Cortana handle.
 * @param hProperty The property bag to fill in.
 * @param pContext The context passed to agent_queue_event.  Note that some
 *  events may reuse this context if there is a failure -- if memory is
 *  allocated, it should be cleared in the PAGENT_EVENT_COMPLETE_CALLBACK.
 * @return Zero for success, something else for failure.
 */
typedef int (*PAGENT_EVENT_SERIALIZER)(
    CORTANA_HANDLE handle,
    PROPERTYBAG_HANDLE hProperty,
    void* pContext);

/**
 * This callback is invoked when an agent event is complete, either due to
 * enough failures to give up, or due to a successful response.
 *
 * @param handle The Cortana handle.
 * @param event The event that has completed.
 * @param pContext The context passed to agent_queue_event.
 * @param result A value summarizing whether or not the event was sent
 *  successfully.
 */
typedef void (*PAGENT_EVENT_COMPLETE_CALLBACK)(
    CORTANA_HANDLE handle,
    CORTANA_EVENT event,
    void* pContext,
    CORTANA_ERROR result);

/**
 * Calls agent_queue_event_with_data with a NULL data parameter.
 * @param handle The Cortana handle.
 * @param event The event to queue.
 * @return Zero for success, something else for failure.
 */
int agent_queue_event(
    CORTANA_HANDLE handle,
    CORTANA_EVENT event);

/**
 * Queues an event to be sent to the agent.
 * @param handle The Cortana handle.
 * @param event The event to queue.
 * @param data A context that will be passed to the serialization function and
 *  to the completion callback.
 * @return Zero for success, something else for failure.
 */
int agent_queue_event_with_data(
    CORTANA_HANDLE handle,
    CORTANA_EVENT event,
    void* data);

/**
 * Adds an event from the agent event queue to the given property bag.
 * @param handle The Cortana handle.
 * @param state The system state. It's used similarly to agent_queue_has_event.
 * @param hProperty Property bag to add the event to.
 */
void agent_send_queued_event(
    CORTANA_HANDLE handle,
    cortana_system_state state,
    PROPERTYBAG_HANDLE hProperty);

/**
 * This should be called when we receive a response for an agent event.
 * @param handle The Cortana handle.
 * @param result An error code summarizing the success or failure.
 */
void agent_handle_response(CORTANA_HANDLE handle, CORTANA_ERROR result);

typedef enum _AGENT_EVENT_ENVIRONMENT
{
    // The event will only be sent when Cortana is online and ready for
    // queries.
    ONLINE_ONLY = 0,

    // The event will be sent when Cortana is online or when it's in offline
    // mode.
    ONLINE_OR_OFFLINE,
} AGENT_EVENT_ENVIRONMENT;

typedef struct _CORTANA_AGENT_EVENT_INFO
{
    CORTANA_EVENT event;
    AGENT_EVENT_ENVIRONMENT environment;

    // These may be NULL if the property bag object callback uses
    // agent_event_set_info to set the ID or event name.
    char const* pszEventId;
    char const* pszEventName;

    // The callback always receives the Cortana handle as its context.  This is
    // called before the event has been sent to the service.
    PAGENT_EVENT_SERIALIZER pfnSerializer;

    // If sending the event fails, we will retry up to this retry limit.
    int retryLimit;

    // This callback is invoked once the event's send is complete -- either due
    // to a successful response, or enough failed responses.  This is a good
    // place to free memory that was allocated for a context.
    PAGENT_EVENT_COMPLETE_CALLBACK pfnCompleteCallback;
} CORTANA_AGENT_EVENT_INFO;

/**
 * Initializes the agent event queue.
 * @param handle The Cortana handle.
 * @param eventInfo (unit test only) An array that describes the supported
 *  agent events, to override the defaults.  This is meant to be used only in
 *  unit tests.
 * @param count (unit test only) The number of structures in eventInfo.
 * @return Zero on success, non-zero on failure.
 */
int agent_initialize_events(
    CORTANA_HANDLE handle
#ifdef AGENT_EVENT_UNIT_TEST
    , CORTANA_AGENT_EVENT_INFO const* eventInfo
    , size_t count
#endif // AGENT_EVENT_UNIT_TEST
    );

/**
 * Clears the agent event queue to prepare for shutdown.  This is intended for
 * unit tests.
 * @param handle The Cortana handle.
 */
void agent_shutdown_events(CORTANA_HANDLE handle);

/**
 * Register notification callback to WNS client.
 * @param handle The Cortana handle.
 */
void register_notification_callback(void* pContext);

/**
 * Register a new WNS channel for notification flow.
 * @param handle The Cortana handle.
 */
int start_notification_flow(void* pContext, TokenStore token_store);

/**
 * Shutdown WNS channel of notification flow and clean up other resource.
 */
void shutdown_notification_flow();

int OnTimerChanged(
    SPEECH_HANDLE              handle,
    enum cortana_timer_type    type,
    enum cortana_timer_state   state);
/**
 * Check whether DeviceSettings exist and not empty.
 */
bool should_get_device_settings();

/**
 * Get device settings from cache.
 */
int get_device_setting(const char* name, char* buf, size_t bufSize);

/**
 * Delete device settings cache.
 */
void delete_device_settings();

/**
 * Parse and save device settings from Companion App.
 */
void save_device_settings(PROPERTYBAG_HANDLE hProperty);

/**
 * Parse and save device settings from Companion App.
 */
void write_device_settings_to_storage(const char* settings);

/**
 * Initialze device info module.
 */
void initialize_device_info(SPEECH_HANDLE speechHandle);

/**
 *  Loads the location Info from the device storage.
 */
void getLocationInfo(CORTANA_HANDLE handle);

/**
* gets the device's latitude as set by the companion app
*/
LOCATION_INFO* get_device_location_info(CORTANA_HANDLE handle);

/**
 * Get mac address of wlan0.
 */
int get_wifi_mac_address(STRING_HANDLE hResult);

/*
* Converts a dashed GUID to a no-dash guid string.
*/
void GuidDToNFormat(char *pszGuidString);

/**
 * Return the pointer to const string of SDK version.
 */
const char* get_cortana_sdk_version();

/**
 * Return device thumbprint generated from CDP
 */
const char* get_cdp_device_thumbprint();

TokenStore cortana_get_token_store(CORTANA_HANDLE handle);
CompanionApp cortana_get_companion_app(CORTANA_HANDLE handle);
CortanaAuth cortana_get_auth(CORTANA_HANDLE handle);
NETDIAG_HANDLE cortana_get_network_diagnostics(CORTANA_HANDLE handle);

/**
 * serialize spotifylogin state info
 * @return return code
 */
int serialize_event_login_state_changed(PROPERTYBAG_HANDLE hProperty, void* pContext);

#define FILEPATH_SIZE 256

typedef struct
{
    cortana_system_state st;
    cortana_system_state_event ev;
    void(*fn)(void);
} cortana_system_state_transition;

///////////////////////////////////////////////////////////////////////////////

#define AUDIO_STATE_MASK_ALARM              0x00000001
#define AUDIO_STATE_MASK_TIMER              0x00000002
#define AUDIO_STATE_MASK_SKYPE              0x00000004
#define AUDIO_STATE_MASK_MUSIC              0x00000008
#define AUDIO_STATE_MASK_SKYPE_INCOMING     0x00000010
#define AUDIO_STATE_MASK_PLUGGEDIN          0x00000020
#define AUDIO_STATE_MASK_TTS                0x00000040
// bit used to determine whether to play the plugged-in audio or not.
#define AUDIO_STATE_MASK_PLAYED_PLUGGEDIN   0x10000000

void cortana_ux_setstate(
    CORTANA_HANDLE hCortana,
    int state);

void cortana_ux_clearstate(
    CORTANA_HANDLE hCortana,
    int state);

int cortana_ux_getstate(
    CORTANA_HANDLE hCortana,
    int state);

///////////////////////////////////////////////////////////////////////////////

enum speech_cancel_type
{
    SPEECH_CANCEL_TYPE_LISTENING = 1,
    SPEECH_CANCEL_TYPE_SPEAKING = 2,
    SPEECH_CANCEL_TYPE_ALL = (SPEECH_CANCEL_TYPE_LISTENING | SPEECH_CANCEL_TYPE_SPEAKING)
};

void speech_cancel(
    CORTANA_HANDLE          hCortana,
    enum speech_cancel_type cancelationType);

#ifdef __linux__
#define localtime_s(__tm, __timet)  localtime_r(__timet, __tm)
#define gmtime_s(__tm, __timet)     gmtime_r(__timet, __tm)
#define scanf_s						scanf
#define sscanf_s					sscanf
#endif

#ifdef __cplusplus
}
#endif

#endif /* PRIVATE_IOT_CORTANA_SDK_H */
