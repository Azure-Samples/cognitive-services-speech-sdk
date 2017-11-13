// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <stdint.h>
#if defined(__unix__) || defined(TARGET_OS_IPHONE) || defined (TARGET_OS_MAC)
#include <stddef.h> // for size_t
#endif

#include "token_store.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPEECH_DLL_EXPORT           extern

/**
 * The current keyword spotter callback version.
 */
#define KWS_CALLBACKS_VERSION       1

/**
 * Enumeration for all Cortana states.
 */
typedef enum
{
    /**
     * Cortana is ready for the next action.
     */
    CORTANA_STATE_READY = 1,
    /**
     * Cortana is in listening mode.
     */
    CORTANA_STATE_LISTENING,
    /**
     * Cortana is thinking.
     */
    CORTANA_STATE_THINKING,
    /**
     * Cortana is speaking.
     */
    CORTANA_STATE_SPEAKING,
} CORTANA_STATE;


/**
* Enumeration for all Cortana Reason states.
* Specifies reason for state change
*/
typedef enum
{
    CORTANA_REASON_NONE = 1,
    /**
     * Main KWS Activaton event
     */
    CORTANA_REASON_KWS_PRIMARY,
    /**
     * Secondary KWS Activation event
     */
    CORTANA_REASON_KWS_SECONDARY,
    /**
     * Manual Cortana state event
     */
    CORTANA_REASON_MANUAL,
    /**
     * Multiturn Cortana state event
     */
    CORTANA_REASON_MULTITURN,
} CORTANA_REASON_STATE;

/**
 * An opaque handle used by the Speech APIs.
 */
typedef void * SPEECH_HANDLE;
typedef void * PROPERTYBAG_HANDLE;

/**
 * A numerical result value used by the Speech APIs.
 */
typedef int     SPEECH_RESULT;

/**
 * The current Speech state used by the Speech APIs.
 */
typedef enum _SPEECH_STATE
{
    /**
     * The Speech component is in a running state.
     */
    SPEECH_STATE_RUNNING    = 1,

    /**
     * The Speech component is in a stopped state.
     */
    SPEECH_STATE_STOPPED    = 2,
} SPEECH_STATE;

/**
* The current speech phrase state used by the Speech APIs.
*/
typedef enum _SPEECH_PHRASE_STATE
{
    /**
     * The user is still speaking.  Recognized text will change as the user speaks
     * more words and as previous utterances get re-interpreted.
     */
    SPEECH_PHRASE_STATE_PARTIAL = 1,

    /**
     * The user is believed to have stopped speaking.  The recognized text is final.
     */
    SPEECH_PHRASE_STATE_FINAL = 2,
} SPEECH_PHRASE_STATE;


/**
 * Voice activation result states
 */
typedef enum _KWS_RESULT_STATE
{
    /**
     * No voice activation event generated
     */
    KEYWORD_NOTHING                                 = 0,
    /**
     * Voice Activation Keyword detection event
     */
    KEYWORD_DETECTED                                = 1,
    /**
     * Voice Activation keyword rejection event
     */
    KEYWORD_REJECTED                                = 2,
} KWS_RESULT_STATE;

/**
* Numerical value for voice activation reject event reason
*/
typedef int KWS_REJECT_REASON;

/**
 * The PSPEECH_DISPLAYTEXT_CALLBACK type represents an application-defined 
 * status callback function used for signaling display text.
 * @param hSpeech The Speech handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param pUTF8String A pointer to a UTF8 string.
 */
typedef void(*PSPEECH_DISPLAYTEXT_CALLBACK)(SPEECH_HANDLE hSpeech, void* pContext, const char* pUTF8String);

/**
 * The PSPEECH_RECOGNITION_CALLBACK type represents an application-defined 
 * status callback function used for signaling recognition of text.
 * @param hSpeech The Speech handle.
 * @param pContext A pointer to the application-defined callback context.
 * @param pUTF8String A pointer to a UTF8 string.
 * @param state The speech recognition state.
 */
typedef void(*PSPEECH_RECOGNITION_CALLBACK)(SPEECH_HANDLE hSpeech, void* pContext, const char* pUTF8String, SPEECH_PHRASE_STATE state);

/**
 * The PSPEECH_AUDIOSTATE_CALLBACK type represents an application-defined 
 * status callback function used for signaling audio state.
 * @param pContext A pointer to the application-defined callback context.
 * @param state The audio state.
 */
typedef void(*PSPEECH_AUDIOSTATE_CALLBACK)(void* pContext, SPEECH_STATE state);

typedef struct _CORTANA_CALLBACKS CORTANA_CALLBACKS;
/**
 * The KWS_STATUS type represents various state of the Keyword Spotter.
 */
typedef struct _KWS_STATUS
{
    KWS_RESULT_STATE       result;
    KWS_REJECT_REASON      rejectReason;
    double                 confidence;
    int                    startSampleOffset;
    int                    endSampleOffset;
    const int16_t*         detectionBuffer;
    size_t                 detectionBufferSize;

    int                    kwsLanguage;
    int                    kwsModelFormatMajor;
    int                    kwsModelFormatMinor;
    int                    kwsModelSourcesVersion;
    float                  kwsThreshold;
} KWS_STATUS;

/**
 * The PKWS_STATUS_CALLBACK type represents an application-defined 
 * status callback function used for signaling keyword spotter state.
 * @param pContext A pointer to the application-defined callback context.
 * @param pStatus A pointer to a KWS_STATUS structure.
 */
typedef void(*PKWS_STATUS_CALLBACK)(void* pContext, const KWS_STATUS* pStatus);

/**
 * The KWS_CALLBACKS type represents an application-defined
 * structure used to register all various keyword spotter events.
 */
typedef struct _KWS_CALLBACKS
{
    uint16_t                     size;
    uint16_t                     version;
    PSPEECH_AUDIOSTATE_CALLBACK  OnStateChanged;
    PKWS_STATUS_CALLBACK         OnStatus;
} KWS_CALLBACKS;

/**
 * Opens a new Speech instance.
 * @param ppHandle A pointer to a Speech handle to be returned back to the 
 * caller.
 * @param reserved Reserved, do not use.
 * @param pReserved Reserved, do not use.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
speech_open(
    SPEECH_HANDLE*  ppHandle,
    uint32_t        reserved,
    void*           pReserved
    );

/**
 * Closes a Speech instance.
 * @param hSpeech The Speech handle to close.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
speech_close(
    SPEECH_HANDLE hSpeech
    );

/**
 * Initializes a Speech instance.
 * @param hSpeech The Speech handle to initialize.
 * @param token_store The token_store to get authentication
 * tokens from
 * @param keyword_table_path The path to the keyword table
 * to use for keyword spotting.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
speech_initialize(
    SPEECH_HANDLE hSpeech,
    TokenStore token_store,
    const char* keyword_table_path
);

/**
 * Sets the event handlers for the Speech instance
 * @param hSpeech The Speech handle.
 * @param uiReserved Reserved for future use.
 * @param pCallbacks A pointer to a CORTANA_CALLBACKS structure.
 * @param pContext An application-defined context to be returned along with each event.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
speech_setcallbacks(
    SPEECH_HANDLE    hSpeech,
    uint32_t         uiReserved,
    CORTANA_CALLBACKS *pCallbacks,
    void*             pContext
    );

/**
 * Sets the audio state for the conversation.
 * @param hConversation The conversation handle.
 * @param state The audio state.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
conversation_audio_setstate(
    SPEECH_HANDLE hConversation,
    SPEECH_STATE  state
    );

/**
 * Writes an audio segment to the service.
 * @param hStream The audio stream handle.
 * @param pData The audio data to be sent.  Audio data must be aligned on the
 * audio sample boundary.
 * @param ui32Size The length of the audio data.
 * @param pui32Written A returned pointer to the amount of data that was sent 
 * to the service.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
audiostream_write(
    SPEECH_HANDLE hStream,
    const void *pData,
    uint32_t    ui32Size,
    uint32_t *  pui32Written
    );

/**
 * Flushes any pending audio to be sent to the service.
 * @param hStream The audio stream handle.
 * @param pData The audio data to be sent.  Audio data must be aligned on the 
 * audio sample boundary.
 * @param ui32Size The length of the audio data.
 * @param pui32Written A returned pointer to the amount of data that was sent 
 * to the service.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
audiostream_flush(
    SPEECH_HANDLE hStream
    );

typedef void*(*PKWS_MALLOC_OVERRIDE)(size_t);
typedef void(*PKWS_FREE_OVERRIDE)(void*);

void keyword_spotter_setalloc(PKWS_MALLOC_OVERRIDE pfnMalloc, PKWS_FREE_OVERRIDE pfnFree);

/**
 * Initializes the keyword spotter.  This must be called before using any 
 * keyword spotter functions.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_initialize(void);

/**
 * Opens a new keyword spotter instance.
 * @param phKWSHandle The returned keyword spotter handle.
 * @param pszModelPath The keyword model path.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_open(
    SPEECH_HANDLE *phKWSHandle,
    const char*    pszModelPath
    );

/**
 * Closes a keyword spotter instance.
 * @param hKWSHandle The handle to a keyword spotter instance.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_close(
    SPEECH_HANDLE hKWSHandle
    );

/**
 * Closes a keyword spotter instance.
 * @param hKWSHandle The handle to a keyword spotter instance.
 * @param pCallbacks A pointer to a application-defined callback.
 * @param pContext An application-defined context to be sent with all keyword spotter events.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT
SPEECH_DLL_EXPORT
keyword_spotter_setcallbacks(
    SPEECH_HANDLE    hKWSHandle,
    KWS_CALLBACKS*   pCallbacks,
    void*            pContext
    );

/**
 * Sends an audio segment to the keyword spotter.
 * @param hKWSHandle The handle to a keyword spotter instance.
 * @param pBuffer The audio data to be sent to the keyword spotter.  The data 
 * is set as 16-bit mono and endian-ness is platform specific.
 * @param size The size of pBuffer.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_write(
    SPEECH_HANDLE hKWSHandle,
    const void *pBuffer,
    size_t size
    );

/**
 * Resets all state of the keyword spotter.
 * @param hKWSHandle The handle to a keyword spotter instance.
 * @return A SPEECH_RESULT.
 */
SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_reset(
    SPEECH_HANDLE hKWSHandle
    );

#ifdef __cplusplus
} // extern "C" 
#endif