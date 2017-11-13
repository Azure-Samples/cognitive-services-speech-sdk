// cOPYRIght (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOT_CORTANA_SDK_H
#define IOT_CORTANA_SDK_H
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/audio_sys.h"
#include "SpeechAPI.h"
#include "audio_manager.h"
#include "storage.h"
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CORTANA_SDK_VERSION "2.0.4"

/**
 * The AUDIO_ENCODER_ONENCODEDDATA type represents an application-defined 
 * status callback function used for to processing raw PCM data.
 * @param pBuffer A pointer to a raw PCM buffer.
 * @param byteToWrite The length of pBuffer in bytes.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*AUDIO_ENCODER_ONENCODEDDATA)(const uint8_t* pBuffer, size_t byteToWrite, void *pContext);

typedef void*           PROPERTYBAG_HANDLE;
typedef void*           TIMER_HANDLE;
typedef int             CORTANA_ERROR;
typedef SPEECH_HANDLE   CORTANA_HANDLE;

// Reserved error codes
#define CORTANA_SUCCESS                     ((CORTANA_ERROR)0)
#define CORTANA_ERROR_PENDING               ((CORTANA_ERROR)1)
#define CORTANA_ERROR_BASE                  ((CORTANA_ERROR)(0x8E200000))
#define CORTANA_ERROR_TIMEOUT               ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x3))
#define CORTANA_ERROR_GENERIC               ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x9))
#define CORTANA_ERROR_STOP                  ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0xc))
#define CORTANA_ERROR_SKYPE_CALL_FAILED     ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0xf))
#define CORTANA_ERROR_NOT_ONLINE            ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x10))
#define CORTANA_ERROR_NO_TOKENS             ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x11))
#define CORTANA_ERROR_OOBE_FAILED           ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x12))
#define CORTANA_ERROR_NO_RESPONSE           ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x13))
#define CORTANA_ERROR_WIFI_SETUP            ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x14))
#define CORTANA_ERROR_AP_CONNECTED          ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x15))
#define CORTANA_ERROR_FOTA_RUNNING          ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x16))
#define CORTANA_ERROR_DEVICE_READY          ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x17))
#define CORTANA_ERROR_WIFI_CONNECTED        ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x18))
#define CORTANA_ERROR_WAIT_FOR_TOKEN        ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x19))
#define CORTANA_ERROR_AUTH_ERROR            ((CORTANA_ERROR)(CORTANA_ERROR_BASE + 0x1a))

#if !defined(CORTANA_BUILD_ID)
#define CORTANA_BUILD_ID "0"
#endif

                                                   
enum cortana_oem_property
{
    CORTANA_OEM_PROPERTY_MANUFACTURER,
    CORTANA_OEM_PROPERTY_MODEL,
    CORTANA_OEM_PROPERTY_VERSION,
    CORTANA_OEM_PROPERTY_SERIAL_NUMBER,
    CORTANA_OS_EDITION,
    CORTANA_SHORT_OS_VERSION,
    CORTANA_FIRMWARE_VERSION,
    CORTANA_FIRMWARE_UPDATE_DATE,
    CORTANA_WIFI_MAC_ADDRESS,
};

/**
 * Spotify state info  represents a structure used to represent the current state of spotifiy eSDK on the device.
 */
typedef struct _SPOTIFY_STATE_INFO
{
    //Spotify User login state
    unsigned int  loggedIn;
#define SPOTIFY_MAX_DEVICE_ID_LEN  100
    //spotify device Id Corresponds to the SpZeroConfVars::device_id field returned by SpZeroConfGetVars() method of the eSDK.
    char   deviceId[SPOTIFY_MAX_DEVICE_ID_LEN];
#define SPOTIFY_MAX_USER_ID_LEN  100
    //Logged in Spotify user ID. Corresponds to the SpZeroConfVars::active_user field returned by SpZeroConfGetVars() method of the eSDK.
    char   activeUser[SPOTIFY_MAX_USER_ID_LEN];
} SPOTIFY_STATE_INFO;

/**
 * The PCORTANA_INIT_CALLBACK type represents an application-defined 
 * status callback function used for signaling that the Cortana platform 
 * has been initialized.  This event occurs before audio device selection 
 * has occurred.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PCORTANA_INIT_CALLBACK)(CORTANA_HANDLE handle, void* pContext);

/**
 * The PCORTANA_SHUTDOWN_CALLBACK type represents an application-defined 
 * status callback function used for signaling that Cortana is about to 
 * be shutdown.  This is the stage where the application can clean up 
 * additional resources that have been allocated.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PCORTANA_SHUTDOWN_CALLBACK)(CORTANA_HANDLE handle, void* pContext);

/**
 * The PCORTANA_ERROR_CALLBACK type represents an application-defined 
 * status callback function used for signaling an error.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param error An error code.
 */
typedef void(*PCORTANA_ERROR_CALLBACK)(CORTANA_HANDLE handle, void* pContext, CORTANA_ERROR error);

/**
 * The PCORTANA_OEM_GETPROP_CALLBACK type represents an application-defined 
 * callback function used for reading OEM specific properties.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param prop The property to read
 * @param hValue The returned string value.
 * @return Returns zero if successful.
 */
typedef int(*PCORTANA_OEM_GETPROP_CALLBACK)(
    CORTANA_HANDLE              handle, 
    void*                       pContext, 
    enum cortana_oem_property   prop, 
    STRING_HANDLE               hValue);

/**
 * The PCORTANA_SKILL_CALLBACK type represents an application-defined 
 * status callback function used for signaling skill information.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param pszUTF8SkillId A pointer to a UTF8 string skill identifier.
 * @param hProperty A property bag handle.
 */
typedef void(*PCORTANA_SKILL_CALLBACK)(CORTANA_HANDLE handle, void* pContext, const char* pszUTF8SkillId, PROPERTYBAG_HANDLE hProperty);

/**
 * The PCORTANA_STATE_CALLBACK type represents an application-defined 
 * status callback function used for signaling the Cortana state.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param state The Cortana state.
 * @param state The Cortana Reason state.
 */
typedef void(*PCORTANA_STATE_CALLBACK)(CORTANA_HANDLE handle, void* pContext, CORTANA_STATE state, CORTANA_REASON_STATE);

typedef struct _CORTANA_RESPONSE_DATA
{
    // The user's query text to which Cortana is responding
    const char *queryText;
    
    // Cortana's response text
    const char *responseText;

    // SSML for Cortana's response
    const char *ssml;

    /* Contains the JSON view object from the response */
    const char *viewJSON;
} CORTANA_RESPONSE_DATA;

/**
* The PCORTANA_RESPONSE_CALLBACK type represents an application-defined
* status callback function used for signaling HTML responses.
* @param handle The Cortana handle.
* @param pContext A pointer to the application-defined callback context.
* @param hProperty A pointer to a property bag.
*/
typedef void(*PCORTANA_RESPONSE_CALLBACK)(CORTANA_HANDLE handle, void* pContext, CORTANA_RESPONSE_DATA *data);

/**
 * The PCORTANA_IDLE_CALLBACK type represents an application-defined 
 * status callback function used to give the application an opportunity to 
 * perform any outstanding work.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.  Any non-zero value will terminate Cortana.
 */
typedef int(*PCORTANA_IDLE_CALLBACK)(CORTANA_HANDLE handle, void* pContext);

enum cortana_timer_state
{
    /**
     * State used to ensure that no timers will be processed.
     */
    TIMER_STATE_STOPPED,
    /**
     * State used to ensure that no timers will be processed.
     */
    TIMER_STATE_SUSPENDED,
    /**
     * State used to ensure any active timers will be processed.
     */
    TIMER_STATE_RUNNING,
};

enum cortana_timer_type
{
    TIMER_TYPE_ALARM,
    TIMER_TYPE_TIMER,
};

/**
 * The PCORTANA_TIMER_CALLBACK type represents an application-defined 
 * status callback function used to give the application to handle alarm 
 * or timer events.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param type The timer type.
 * @param state The alarm state.
 * @return A return code or zero if successful.  Any non-zero value will abort further
 * alarm processing.
 */
typedef int(*PCORTANA_TIMER_CALLBACK)(
    CORTANA_HANDLE             handle, 
    void*                      pContext, 
    enum cortana_timer_type    type,
    enum cortana_timer_state   state);                

enum cortana_bluetooth_state
{
    CORTANA_BLUETOOTH_DISCONNECTED = 0,
    CORTANA_BLUETOOTH_PAIRING = 1,
    CORTANA_BLUETOOTH_PAIRED,
};

/**
 * The PCORTANA_BLUETOOTH_CALLBACK type represents a status callback function 
 * used to notify the caller of bluetooth events.
 * @param handle The Cortana handle.
 * @param state The current bluetooth state.  
 * When the state is set to CORTANA_BLUETOOTH_PAIRING, the caller must call 
 * cortana_bluetooth_setstate with the updated state to transition the 
 * bluetooth state manager appropriately.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PCORTANA_BLUETOOTH_CALLBACK)(
    CORTANA_HANDLE               handle, 
    enum cortana_bluetooth_state state,
    void*                        pContext);

enum cortana_telephony_state
{
    CORTANA_TELEPHONY_IDLE          = 0, // No active skype call
    CORTANA_TELEPHONY_INCOMING_CALL = 10, // Incoming 
    CORTANA_TELEPHONY_CALLING       = 20, // Outgoing call in progress
    CORTANA_TELEPHONY_IN_CALL       = 21, // Active call in progress
};

/**
 * The PCORTANA_TELEPHONY_CALLBACK type represents a status callback function 
 * used to notify the caller of telephony state events.
 * @param handle The Cortana handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param mediaEvent The media control event.
 */
typedef void(*PCORTANA_TELEPHONY_CALLBACK)(
    CORTANA_HANDLE               handle, 
    void*                        pContext, 
    enum cortana_telephony_state state);

/**
 * The PCORTANA_TIMEZONE_CALLBACK type represents a status callback function 
 * used to set timezone on the device.
 * @param handle The Cortana handle.
 */
typedef void(*PCORTANA_TIMEZONE_CALLBACK)(
    CORTANA_HANDLE               handle,
    const char*                  timezone);

/**
 * The PCORTANA_SPOTIFY_LOGIN_CALLBACK type represents a function 
 * used to login to the spotify esdk on the device.
 * @param handle The Cortana handle.
 * @param outhToken The auth token to login.
 */
typedef void(*PCORTANA_SPOTIFY_LOGIN_CALLBACK)(
    CORTANA_HANDLE               handle,
    const char*                  outhToken);

/**
 * The PCORTANA_SPOTIFY_LOGOUT_CALLBACK type represents a function 
 * used to logout of  the spotify esdk on device.
 * @param handle The Cortana handle.
 */
typedef void(*PCORTANA_SPOTIFY_LOGOUT_CALLBACK)(
    CORTANA_HANDLE               handle);

/**
 * The PCORTANA_SPOTIFY_GET_STATE_CALLBACK type represents a function 
 * that triggers a query of Spotify state on the device.  When this
 * function is called, it should make a best effort to call
 * cortana_set_spotify_state with the supplied Cortana handle.
 * @param handle The Cortana handle with which to call
 *  cortana_set_spotify_state.
 * @return 0 if successful, non-zero otherwise.
 */
typedef int(*PCORTANA_SPOTIFY_GET_STATE_CALLBACK)(
    CORTANA_HANDLE               handle);

// This enum is passed to the UPLOAD_LOG_CALLBACK to customize logging
// behavior.  These are bit flags.
typedef enum _UPLOAD_LOG_OPTIONS
{
    UPLOAD_LOG_OPTION_NONE = 0x0,

    // An audio dump has been placed in a file named "audio-dump.wav" in the
    // directory used by the file_open API.  This may be used to diagnose
    // issues with the keyword spotter.
    UPLOAD_LOG_OPTION_AUDIO_DUMP = 0x1,
} UPLOAD_LOG_OPTIONS;

/**
 * Upload logs, and tag them so that they can be correlated by the logId.  This
 * callback is used for diagnosing issues with the client.
 *
 * @param handle The Cortana handle.
 * @param logId An identifier that will be used to look up the logs.
 * @param options Customization options for the log.
 * @return Negative if failed, otherwise defined by OEM.
 */
typedef int(*PCORTANA_UPLOAD_LOG_CALLBACK)(
    CORTANA_HANDLE               handle,
    const char*                  logId,
    UPLOAD_LOG_OPTIONS           options);

/**
 * The PCORTANA_DEVICE_NAME_SET_CALLBACK type represents a status callback function 
 * used to set the device name.
 * @param handle The Cortana handle.
 * @param deviceName The device name to be set.
 */
typedef void(*PCORTANA_DEVICE_NAME_CHANGED_CALLBACK)(
    CORTANA_HANDLE               handle,
    const char*                  deviceName);

/**
* System FSM(Finite State Machine) enums.
* Please keep the order unchanged. For new state, please add to the tail. Otherwise telemetry mess.
*/
typedef enum
{
    // Initial state for the Cortana when it gets initialized. 
    ST_INIT = 0,
    // When oobe condition check not satisfied, then go to this state to wait for the user to do OOBE
    ST_AGENT_SETUP = 1,
    // When all the OOBE condition check are satisfied, then we move to this state.
    ST_NORMAL = 2,
    // When we have internet access after system go to normal, we go to this state
    ST_WIFI_CONNECTED = 3,
    // When OTA is needed, then we go to this state
    ST_OTA = 4,
    // When cortana detect that there is no internet, we go to this state
    ST_OFFLINE_RUN = 5,
    // When cortana is health, we are in this state
    ST_ONLINE_RUN = 6,
    // When any unrepairable error happens, we go to this state
    ST_ERROR = 7,

    ST_ANY = -1,
    ST_NONE = -2,
} cortana_system_state;

typedef enum
{
    EV_OOBE_NEEDED= 0,
    EV_OOBE_NOT_NEEDED = 1,
    EV_OOBE_FINISHED = 2,
    EV_OOBE_TIMEOUT= 3,
    EV_DEVICE_CONFIRM = 4,
    EV_NETWORK_CONNECTED = 5,
    EV_NETWORK_NO_INTERNET = 6,
    EV_OTA_NEEDED = 7,
    EV_OTA_NOT_NEEDED = 8,
    EV_OTA_FAILED = 9,
    EV_TOKEN_INVALID = 10,
    EV_HEART_BEAT_ONLINE = 11,
    EV_HEART_BEAT_OFFLINE = 12,
    EV_USER_CHANGED = 13,
    EV_USER_TRIGGERED_OOBE = 14,
    EV_STOP_FSM = -1,
} cortana_system_state_event;

// Define the start flag type
typedef enum _CORTANA_START_FLAG
{
    HTTP_LISTENER_MODE_FLAG = 1,
} CORTANA_START_FLAG;

// Define the flag value for corresponding type defined above
typedef enum
{
    // Below 2 values are used to set the HTTP listener behavior after set up
    HTTP_LISTENER_STOP_AFTER_SETUP = 0,

    HTTP_LISTENER_STAY_AFTER_SETUP = (1 << HTTP_LISTENER_MODE_FLAG),
} cortana_start_flags;

/**
* The PCORTANA_SYSTEM_FSM_CALLBACK type represents a status callback function
* used to notify the caller of system state events.
* @param handle The Cortana handle.
* @param pContext A pointer to the application-defined callback context.
* @param state The current state of the state machine.
*/
typedef void(*PCORTANA_SYSTEM_FSM_CALLBACK)(
    CORTANA_HANDLE               handle,
    void*                        pContext,
    cortana_system_state state);

int cortana_system_state_settransfercode(const char* code);
void cortana_system_on_error(CORTANA_ERROR error);

/**
 * The current Cortana callback version.
 */
#define CORTANA_CALLBACKS_VERSION    1

/**
 * The CORTANA_CALLBACKS type represents an application-defined 
 * structure used to register all various Cortana events.
 */
typedef struct _CORTANA_CALLBACKS
{
    uint16_t                                size;
    uint16_t                                version;
    PCORTANA_INIT_CALLBACK                  OnInitialized;
    PCORTANA_SHUTDOWN_CALLBACK              OnShutdown;
    PCORTANA_ERROR_CALLBACK                 OnError;
    PSPEECH_RECOGNITION_CALLBACK            OnSpeech;
    PSPEECH_AUDIOSTATE_CALLBACK             OnAudioInputState;
    PSPEECH_AUDIOSTATE_CALLBACK             OnAudioOutputState;
    PSPEECH_AUDIOSTATE_CALLBACK             OnKeywordState;
    PCORTANA_OEM_GETPROP_CALLBACK           OnOEMGetProperty;
    PCORTANA_STATE_CALLBACK                 OnCortanaState;
    PCORTANA_RESPONSE_CALLBACK              OnResponse;
    PCORTANA_SKILL_CALLBACK                 OnSkill;
    PCORTANA_IDLE_CALLBACK                  OnIdle;
    PCORTANA_VOLUME_CALLBACK                OnVolumeChanged;
    PCORTANA_TIMER_CALLBACK                 OnTimerChanged;
    PCORTANA_AUDIODUCK_CALLBACK             OnAudioDuck;
    PCORTANA_TELEPHONY_CALLBACK             OnTelephonyState;
    PCORTANA_SYSTEM_FSM_CALLBACK            OnSystemStateChanged;
    PCORTANA_GET_VOLUME_CALLBACK            OnGetVolume;
    PCORTANA_BLUETOOTH_CALLBACK             OnBluetoothEvent;
    PCORTANA_TIMEZONE_CALLBACK              OnTimezoneSet;
    PCORTANA_DEVICE_NAME_CHANGED_CALLBACK   OnDeviceNameChanged;
    PCORTANA_SPOTIFY_LOGIN_CALLBACK         SpotifyLogin;
    PCORTANA_SPOTIFY_LOGOUT_CALLBACK        SpotifyLogout;
    PCORTANA_SPOTIFY_GET_STATE_CALLBACK     SpotifyGetState;
    PCORTANA_UPLOAD_LOG_CALLBACK            UploadLog;
} CORTANA_CALLBACKS;

/**
* Runs Cortana state machine.  Note: this is a blocking call.
* @param pCallbacks A pointer to a CORTANA_CALLBACKS structure.
* @param startFlags The cortana start flags, can use the | to set multiple flags.
* @param pContext An application-defined context to be returned along with each event.
*/
void cortana_run(CORTANA_CALLBACKS*pCallbacks, cortana_start_flags startFlags, void* pContext);

/**
* Handle the event for the state machine and transfer its state.
* @param evt The event received.
*/
void cortana_system_setstate(cortana_system_state_event evt);

/**
* Get the current cortana system state.
*/
cortana_system_state cortana_system_current_state();

/**
* Reset some persit state for system state
*/
void cortana_system_reset();

/**
 * Initializes Cortana.
 * @param pCallbacks A pointer to a CORTANA_CALLBACKS structure.
 * @param pContext An application-defined context to be returned along with each event.
 */
SPEECH_HANDLE cortana_initialize(
    CORTANA_CALLBACKS *pCallbacks,
    void*             pContext
);

/**
 * Processes any Cortana related operations that need attention.
 * @param handle The Cortana handle.
 */
void cortana_dowork(
    CORTANA_HANDLE handle
);

/**
 * Shuts down the current Cortana instance.
 * @param hCortana The speech handle of cortana.
 */
void cortana_shutdown(SPEECH_HANDLE hCortana);

/**
 * Handles navigation related events.  Override your implementation to process navigation related events.
 * @param latitude The latitude.
 * @param longitude The longitude.
 * @param pszLocationName The location name.
 */
void cortana_nav_display(double latitude, double longitude, const char* pszLocationName);

/**
 * Parse and save location settings from Companion App.
 * @param latitude The latitude .
 * @param longitude The longitude.
 * @param timezone The timezone(in windows format)
 */
void cortana_setlocation(CORTANA_HANDLE handle ,double latitude, double longitude, const char* timezone);

/**
 * Sets the timezone on the device
 * @param handle The Cortana handle.
 * @param name The IANA timezone name.
 */
void cortana_set_timezone(CORTANA_HANDLE handle, const char* pszIANATimeZone);

/**
 * Plays an audio file from disk.
 * @param handle The Cortana handle.
 * @param type The audio device that should be rendered.
 * @param pszFileName A NULL terminated file path.
 * @return A return code or zero if successful.
 */
int  cortana_playfile(
    CORTANA_HANDLE          handle, 
    enum cortana_audio_type type,
    const char*             pszFileName);

/**
 * Sets the current media control state.
 * @param handle The Cortana handle.
 * @param mediaEvent The media event to set. Refer to the cortana_mediactrl_state 
 * enumeration.
 * @param pContext Any additional context for the event
 */
void cortana_mediacontrol_setstate(
    CORTANA_HANDLE handle, 
    enum cortana_mediactrl_state mediaEvent);

/**
 * Sets media control options for players.
 * @param handle The Cortana handle.
 * @param option The media option. Refer to the cortana_media_option 
 * enumeration.
 */
void cortana_mediacontrol_setoption(
    CORTANA_HANDLE handle, 
    CORTANA_MEDIA_OPTION option);

/**
 * Gets the current volume level for the requested audio group.
 * @param handle The Cortana handle.
 * @param audioGroup The audio group.
 * @return The current audio level.  A value from 0 to 100 or < 0 on failure.
 */
int cortana_getvolume(CORTANA_HANDLE handle, enum cortana_audio_type audioGroup);

/**
 * Sets the Cortana state
 * @param handle The Cortana handle.
 * @state The state to set
 * @return A return code or zero if successful.
 */
int cortana_setstate(
    CORTANA_HANDLE handle, 
    CORTANA_STATE  state
);

/**
 * Initializes the audio encoder.
 * @param onDataCallback An application defined callback.
 * @param pContext An application defined callback context.
 * @return A return code or zero if successful.
 */
int audio_encoder_initialize(
    AUDIO_ENCODER_ONENCODEDDATA onDataCallback,
    void* pContext);

/**
 * Encodes raw PCM data.
 * @param pBuffer The PCM data.
 * @param byteToWrite The length pBuffer.
 * @return A return code or zero if successful.
 */
int audio_encoder_write(
    const uint8_t* pBuffer,
    size_t         byteToWrite);

/**
 * Flushes the encoder.
 * @return A return code or zero if successful.
 */
int audio_encoder_flush(void);

/**
 * Unitializes the encoder.
 */
void audio_encoder_uninitialize(void);

/**
 * Returns a string representing the format of the TTS format to use.
 * @return The format string of TTS.
 */
const char *audio_decoder_tts_format(void);

/**
 * Unitializes the decoder.
 */
void audio_decoder_uninitialize(void);

/**
 * Sets the current bluetooth state.
 * @param handle The Cortana handle.
 * @param state The current bluetooth state.
 * Callers can set the state to CORTANA_BLUETOOTH_PAIRING to trigger a Cortana 
 * response.
 * @return A return code or zero if successful.
 */
int cortana_bluetooth_setstate(
    CORTANA_HANDLE               handle,
    enum cortana_bluetooth_state state);
    
/**
 * Inform the Cortana SDK about Spotify state.  This function should be called
 * in response to the SpotifyGetState callback, and before calling
 * cortana_send_spotify_logged_in_event.
 * @param handle The Cortana handle.
 * @param state The Spotify state.
 */
void cortana_set_spotify_state(
    CORTANA_HANDLE handle,
    const SPOTIFY_STATE_INFO* state);

/**
 * Inform the Cortana SDK about an error processing a SpotifyGetState,
 * SpotifyLogin, or SpotifyLogout callback.
 * @param handle The Cortana handle.
 * @param pszApiName Optional, the API name that failed.
 * @param errorCode The error code.
 */
void cortana_send_spotify_error(
    CORTANA_HANDLE handle,
    const char* pszApiName,
    uint32_t errorCode);

/**
 * The PPROPERTYBAG_OBJECT_CALLBACK type represents an application-defined
 * callback function for handling property object nodes.
 * @param hProperty The property node handle.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.  In property bag enumeration calls,
 * any non-zero value will termination the enumeration.
 */
typedef int(*PPROPERTYBAG_OBJECT_CALLBACK)(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext
    );

/**
 * Initializes the propertybag adapter.
 * @return A return code or zero if successful.
 */
int propertybag_initialize();

/**
 * Shutsdown the propertybag adapter.
 */
void propertybag_shutdown();

/**
 * Deserializes a JSON data string.
 * @param pszJSON The JSON string to deserialize.
 * @param size The size of pszJSON
 * @param pfncallback A callback to handle the root object.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.
 */
int propertybag_deserialize_json(
    const char*                     pszJSON,
    size_t                          size,
    PPROPERTYBAG_OBJECT_CALLBACK    pfncallback,
    void*                           pContext
);

/**
 * Enumerates an array object.
 * @param hProperty The parent property node handle.
 * @param pszName The name of the array element.
 * @param pfncallback A callback to handle the enumerated object.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.
 */
int propertybag_enumarray(
    PROPERTYBAG_HANDLE              hProperty,
    const char*                     pszName,
    PPROPERTYBAG_OBJECT_CALLBACK    pfncallback,
    void*                           pContext
    );

/**
 * Retrieves a property object.
 * @param hProperty The parent property node.
 * @param pszName The name of the object element.
 * @param pfncallback A callback to handle the enumerated object.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.
 */
int propertybag_getchildvalue(
    PROPERTYBAG_HANDLE              hProperty,
    const char*                     pszName,
    PPROPERTYBAG_OBJECT_CALLBACK    pfncallback,
    void*                           pContext
);

/**
* Returns a property object.
* @param hProperty The parent property node.
* @param pszName The name of the object element.
* @return The child property node or NULL on error.
*/
PROPERTYBAG_HANDLE propertybag_getchildvalue2(
    PROPERTYBAG_HANDLE              hProperty,
    const char*                     pszName
);

/**
 * Retrieves a string value of an object.
 * @param hProperty The parent property node.
 * @param pszName The name of the object element.
 * @return The string value of the property element or NULL if not found.
 */
const char* propertybag_getstringvalue(
    PROPERTYBAG_HANDLE               hProperty,
    const char*                      pszName
);

/**
 * Retrieves a numerical value of an object.
 * @param hProperty The parent property node.
 * @param pszName The name of the object element.
 * @return The numerical value of the property element or NAN if not found.
 */
double propertybag_getnumbervalue(
    PROPERTYBAG_HANDLE               hProperty,
    const char*                      pszName
);

/**
* Retrieves a numerical value of an object.
* @param hProperty The parent property node.
* @param pszName The name of the object element.
* @return The boolean value of the property where 1 is True and 0 is False
*/
unsigned int propertybag_getbooleanvalue(
    PROPERTYBAG_HANDLE               hProperty,
    const char*                      pszName
);

/**
 * Returns a serialize JSON data string.
 * @param pfncallback A callback to handle the enumerated object.
 * @param pContext A pointer to the application-defined callback context.
 * @return A handle to the string object.
 */
STRING_HANDLE propertybag_serialize(
    PPROPERTYBAG_OBJECT_CALLBACK    pfncallback,
    void*                           pContext
);

/**
 * Serializes an object.
 * @param hProperty The parent property node.
 * @param pszName The name of the object element.
 * @param pfncallback A callback to handle the enumerated object.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.
 */
int propertybag_serialize_object(
    PROPERTYBAG_HANDLE           hProperty,
    const char*                  pszName,
    PPROPERTYBAG_OBJECT_CALLBACK pfncallback,
    void*                        pContext
);

/**
 * Serializes an array.
 * @param hProperty The parent property node handle.
 * @param pszName The name of the array element.
 * @param pfncallback A callback to handle the enumerated object.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.
 */
int propertybag_serialize_array(
    PROPERTYBAG_HANDLE              hProperty,
    const char*                     pszName,
    PPROPERTYBAG_OBJECT_CALLBACK    pfncallback,
    void*                           pContext
);

/**
 * Serializes a number value.
 * @param hProperty The parent property node.
 * @param pszName The name of the object element.
 * @param value The value to set.
 */
void propertybag_setnumbervalue(
    PROPERTYBAG_HANDLE   hProperty,
    const char*          pszName,
    double               value
);

/**
* Serializes a boolean value.
* @param hProperty The parent property node.
* @param pszName The name of the object element.
* @param value The value to set.
*/
void propertybag_setbooleanvalue(
    PROPERTYBAG_HANDLE   hProperty,
    const char*          pszName,
    int                  value
);

/**
 * Serializes a string value.
 * @param hProperty The parent property node.
 * @param pszName The name of the object element.
 * @param value The value to set.
 */
void propertybag_setstringvalue(
    PROPERTYBAG_HANDLE   hProperty,
    const char*          pszName,
    const char*          value
);

/**
 * Executes a skill handler.
 * @param pszSkillId The skill identifier to invoke.
 * @param hProperty The skill specific property bag.
 * @param pContext A pointer to the application-defined callback context.
 * @return A return code or zero if successful.
 */
int skill_dispatch(
    const char*        pszSkillId,
    PROPERTYBAG_HANDLE hProperty,
    void*              pContext);

/**
 * The PTIMER_CALLBACK type represents an application-defined
 * status callback function used for signaling when the timer has fired.
 * @param hTimer The timer handle.
 * @param pContext A pointer to the application-defined callback context.
 */
typedef void(*PTIMER_CALLBACK)(
    TIMER_HANDLE        hTimer,
    void*               pContext
);

/*Initializes the alarm system.
 * @return A return code or zero if successful.
 */
int cortana_alarms_initialize(CORTANA_HANDLE handle);

/**
 * Shuts down the alarm system.
 */
void cortana_alarms_shutdown(void);

/**
* Sets the desired alarm ringer state.
* @param handle The Cortana handle.
* @return A return code or zero if successful.
*/
int cortana_alarms_setstate(CORTANA_HANDLE handle, enum cortana_timer_state state);

/**
 * Initializes the timer system.
 * @return A return code or zero if successful.
 */
int cortana_timers_initialize(void);

/**
 * Shuts down the timer system.
 */
void cortana_timers_shutdown(void);

/**
 * Sets the desired timer manager state.
 * @param handle The Cortana handle.
 * @param state The timer state.
 * @return A return code or zero if successful.
 */
int cortana_timers_setstate(CORTANA_HANDLE handle, enum cortana_timer_state state);

/**
 * Creates a new timer.
 * @param milliseconds The number of milliseconds from now when the timer is to fire.
 * @param pfnCallback The callback to invoke when the timer has fired.
 * @param pszName An optional name to attach to the timer.  Used for identifying
 * the timer in log messages.
 * @param pContext A pointer to the application-defined callback context.
 * @return A handle to the timer.
 */
TIMER_HANDLE cortana_timer_create(
    uint32_t        milliseconds,
    PTIMER_CALLBACK pfnCallback,
    const char*     pszName,
    void*           pContext
);

/**
 * Destroys a new timer.
 * @param hTimer The handle to the timer.
 */
void cortana_timer_destroy(TIMER_HANDLE hTimer);

/**
 * Performs any outstanding work on all registered timers.
 */
void cortana_timer_dowork(void);

/**
 * Puts Cortana into delight mode.  Prepare to be delighted 
 * and beware of flying unicorns and dancing leprechauns.
 * @param handle The Cortana handle.
 */
void cortana_delight_me(CORTANA_HANDLE hCortana);

/**
 * Cancels the active Cortana conversation.
 * @param handle The Cortana handle.
 */
void cortana_cancel(CORTANA_HANDLE hCortana);

enum cortana_action
{
    CORTANA_ACTION_SHORT_PRESS,
    CORTANA_ACTION_LONG_PRESS,
    CORTANA_ACTION_VOICE_MUTE,
    CORTANA_ACTION_VOICE_UNMUTE,
    CORTANA_ACTION_VOLUME_UP,
    CORTANA_ACTION_VOLUME_DOWN,
    CORTANA_ACTION_DIAGNOSTIC,
};

/**
 * Triggers a Cortana action to execute.
 * @param hCortana The Cortana handle.
 * @param action The Cortana action to execute.
 */
int cortana_onaction(CORTANA_HANDLE hCortana, enum cortana_action action);

/**
* Answers an incoming call
**/
void cortana_telephony_answercall();

/**
* Either rejects an incoming call
* or ends an ongoing call
*/
void cortana_telephony_endcall();

/**
* Open a named file.
* @param name The name of the file.
* @param mode Kind of access.
* @return A return nonzero code if successful.
*/
void* file_open(const char* name, const char* mode);

/**
* Write data to a file.
* @param hFile The pointer to file.
* @param data The data to be written.
* @param size The size of 'data'
* @return A return code or zero if successful.
*/
int file_write(void* hFile, const void* data, size_t size);

/**
* Read data to a file.
* @param hFile The pointer to file.
* @param data The data to be read.
* @param size The size of 'data'
* @return A return code or zero if successful.
*/
int file_read(void* hFile, void* data, size_t size);

/**
* Get the size of a file.
* @param hFile The pointer to file.
* @return A return code or zero if successful.
*/
int file_size(void* handle);

/**
* Close a file.
* @param hFile The pointer to file.
*/
void file_close(void* hFile);

/**
* Remove file.
* @param name The name of the file.
* @return A return nonzero code if successful.
*/
int file_remove(const char* name);

/**
* @param handle The cortana Handle
* @param deviceName the returned device name. 
* @return A return code or zero if successful.
*/
int cortana_get_device_name(CORTANA_HANDLE handle, STRING_HANDLE deviceName);

/**
* @param action The action done. eg: mic button pressed
* @param state_info a json string indicating the device state, can be empty.
*/
void cortana_log_device_physical_input(const char* action, const char* state_info);

/**
 * Collects various diagnostics on the system and sends the data up to a 
 * collection service.
 * @param hCortana The cortana Handle
 * @param logId An optional pointer to a log identifier.  This may be NULL.
 * @param options The logging options to use.
 * @return A return nonzero code if successful.
 */
int cortana_collect_diagnostics(
    CORTANA_HANDLE      hCortana,
    char const*         logId,
    UPLOAD_LOG_OPTIONS  options);

#ifdef __cplusplus
}
#endif

//The following are here only because 
//iot-cortana-sdk should be the only 
//file you must include to access the 
//public interface
#include "auth.h"
#include "storage.h"
#include "dns_cache.h"
#include "transport.h"

#endif /* IOT_CORTANA_SDK_H */
