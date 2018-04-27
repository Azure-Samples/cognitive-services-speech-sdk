#pragma once
#include <stdint.h>
#if defined(__unix__) || defined(__APPLE__)
#include <stddef.h> // for size_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SPEECH_DLL_EXPORT               extern
#define SPEECH_STATE_RUNNING                 1
#define SPEECH_STATE_STOPPED                 2
#define SPEECH_INIT_DATA_ASCII      0x00000001
#define SPEECH_INIT_DATA_FILEPATH   0x00000002
#define SPEECH_CALLBACKS_VERSION             1
#define KWS_CALLBACKS_VERSION                1
#define KEYWORD_DETECTED                     1
#define KEYWORD_REJECTED                     2
#define KWS_REJECTREASON_NONE             0x00
#define KWS_REJECTREASON_LEADINGSILENCE   0x01
#define KWS_REJECTREASON_THRESHOLD        0x02
#define TTS_STATE_RUNNING                    1
#define TTS_STATE_STOPPED                    2

typedef void *  SPEECH_HANDLE;
typedef int     SPEECH_RESULT;
typedef int     SPEECH_STATE;
typedef int     TTS_STATE;
typedef int     KWS_RESULT_STATE;
typedef int     KWS_REJECT_REASON;

typedef void(*PSPEECH_DISPLAYTEXT_CALLBACK)(void* pContext, const char* pUTF8String);
typedef void(*PSPEECH_RECOGNITION_CALLBACK)(void* pContext, const char* pUTF8String);
typedef void(*PSPEECH_ERROR_CALLBACK)(void* pContext, int error);
typedef void(*PSPEECH_INTENT_CALLBACK)(void* pContext, const char* pUTF8Intent);
typedef void(*PSPEECH_LOG_CALLBACK)(const char* pszMessage);
typedef void(*PSPEECH_AUDIOSTATE_CALLBACK)(void* pContext, SPEECH_STATE state);

typedef struct _SPEECH_CALLBACKS
{
    uint16_t                     size;
    uint16_t                     version;
    PSPEECH_DISPLAYTEXT_CALLBACK OnDisplayText;
    PSPEECH_RECOGNITION_CALLBACK OnRecognition;
    PSPEECH_ERROR_CALLBACK       OnError;
    PSPEECH_INTENT_CALLBACK      OnIntent;
    PSPEECH_LOG_CALLBACK         OnMessage;
    PSPEECH_AUDIOSTATE_CALLBACK  OnAudioState;
} SPEECH_CALLBACKS;

typedef struct _KWS_STATUS
{
//  event info
    KWS_RESULT_STATE       result;
    KWS_REJECT_REASON      rejectReason;
	double                 confidence;
    int                    startSampleOffset;
    int                    endSampleOffset;
    const int16_t*         detectionBuffer;
    size_t                 detectionBufferSize;

//  model metadata
    int                    kwsLanguage;
    int                    kwsModelFormatMajor;
    int                    kwsModelFormatMinor;
    int                    kwsModelSourcesVersion;
    float                  kwsThreshold;
} KWS_STATUS;

typedef void(*PKWS_STATUS_CALLBACK)(void* pContext, const KWS_STATUS* pStatus);
typedef void*(*PKWS_MALLOC_OVERRIDE)(size_t);
typedef void(*PKWS_FREE_OVERRIDE)(void*);

typedef struct _KWS_CALLBACKS
{
    uint16_t                     size;
    uint16_t                     version;
    PSPEECH_AUDIOSTATE_CALLBACK  OnStateChanged;
    PKWS_STATUS_CALLBACK         OnStatus;
} KWS_CALLBACKS;

typedef void*(*PKWS_MALLOC_OVERRIDE)(size_t);
typedef void(*PKWS_FREE_OVERRIDE)(void*);

void keyword_spotter_setalloc(PKWS_MALLOC_OVERRIDE pfnMalloc, PKWS_FREE_OVERRIDE pfnFree);

SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_initialize(
    );

SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_open(
    SPEECH_HANDLE *phKWSHandle,
    const char*    pszModelPath
    );

SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_close(
    SPEECH_HANDLE hKWSHandle
    );

SPEECH_RESULT
SPEECH_DLL_EXPORT
keyword_spotter_setcallbacks(
    SPEECH_HANDLE    hKWSHandle,
    KWS_CALLBACKS*   pCallbacks,
    void*            pContext
    );

SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_write(
    SPEECH_HANDLE hKWSHandle,
    const void *pBuffer,
    size_t size
    );

SPEECH_RESULT 
SPEECH_DLL_EXPORT
keyword_spotter_reset(
    SPEECH_HANDLE hKWSHandle
    );

#ifdef __cplusplus
} // extern "C" 
#endif
