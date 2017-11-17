// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "private-iot-cortana-sdk.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/urlencode.h"
#include <assert.h>

#ifdef __linux__
#include <unistd.h>
#endif

#define AUDIO_SAMPLE_RATE_HZ         16000
#define AUDIO_SAMPLES_TO_MS(x)       (int)((x) / (AUDIO_SAMPLE_RATE_HZ / 1000))

#ifdef __linux__
#define MAX_AUDIO_BUFFER_SIZE        (25 * AUDIO_SAMPLE_RATE_HZ)
#else
#define MAX_AUDIO_BUFFER_SIZE        (5 * AUDIO_SAMPLE_RATE_HZ)
#endif // __linux__ 
#define KEYWORD_BACK_PADDING_SAMPLES (8000)     // 500ms of back padding in front of keyword
#define SILK_FRAME_SIZE              (320)      // 20ms
#define SILENCE_THRESHOLD            0x800
#define SILENCE_SAMPLE_THRESHOLD     AUDIO_SAMPLE_RATE_HZ // 1 second

const uint16_t kKWSFrameCount       = 720;

#define SPEECH_STALL_THRESHOLD_MS   ((10 * kKWSFrameCount * 1000) / AUDIO_SAMPLE_RATE_HZ)

#define DEFAULT_VOLUME              50

typedef void(*PRESPONSE_PATH_HANDLER)(
    TRANSPORT_HANDLE     hTransport,
    const char*          pszPath,
    const char*          pszMime,
    const unsigned char* buffer,
    size_t               size,
    void*                pContext);

const char kApiPath_Speech_Hypothesis[] = "speech.hypothesis";
const char kApiPath_Speech_Phrase[] = "speech.phrase";
const char kApiPath_TurnStart[] = "turn.start";
const char kApiPath_TurnEnd[] = "turn.end";
const char kApiPath_SpeechStartDetected[] = "speech.startDetected";
const char kApiPath_SpeechEndDetected[] = "speech.endDetected";
const char kApiPath_Response[] = "response";
const char kSpeechEndpoint[] = "wss://speech.platform.bing.com/cortana/api/v1?environment=" CORTANASDK_ENVIRONMENT "&language=" CORTANASDK_DEFAULT_LANGUAGE;
const char kUserAgent[] = "CortanaSDK (" CORTANASDK_BUILD_HASH "DeviceType=Near;SpeechClient=" CORTANA_SDK_VERSION ")";
const char* const kString_audioincludeskeyword = "audioincludeskeyword";
const char* const kString_keywordconfidence = "keywordconfidence";
const char* const kString_keywordstartoffset = "keywordstartoffset";
const char* const kString_keywordduration = "keywordduration";


// Zhou: stub functions to resolve dependency
void cortana_system_setstate(cortana_system_state_event evt)
{
    (void)evt;
    return;
}

int cortana_setstate(SPEECH_HANDLE        handle,
    CORTANA_STATE        state)
{
    (void)handle;
    (void)state;
    return 0;
}


uint64_t telemetry_gettime()
{
    return 0;
}


// Zhou: Callback for AudioEncode Ondata, probably not needed.
static void AudioEncode_OnData(const uint8_t* pBuffer, size_t byteToWrite, void *pContext)
{
    metrics_encoder_data(byteToWrite);

    if (!byteToWrite)
    {
        (void)audiostream_flush((SPEECH_HANDLE)pContext);
    }
    else
    {
        (void)audiostream_write(
            (SPEECH_HANDLE)pContext,
            pBuffer,
            (uint32_t)byteToWrite,
            NULL);
    }
}

// Zhou: called by AudioInput_Write(). Just check the audio thread is unchanged and write to log if timeout exceeds. No need.
static void audio_input_health_check(SPEECH_CONTEXT* pSC)
{
    uint64_t const now = cortana_gettickcount();

    Lock(pSC->HealthLock);
    if (pSC->HealthContext.AudioThread == 0)
    {
        pSC->HealthContext.AudioThread = ThreadAPI_GetCurrentId();
    }
    else
    {
        // We don't expect the audio thread to change.
        assert(pSC->HealthContext.AudioThread == ThreadAPI_GetCurrentId());
    }

    int64_t const delayMs =
        (pSC->HealthContext.LastListenTime != 0) ?
            (now - pSC->HealthContext.LastListenTime) : 0;

    pSC->HealthContext.LastListenTime = now;
    Unlock(pSC->HealthLock);

    if (delayMs > SPEECH_STALL_THRESHOLD_MS)
    {
        metrics_stall("kws", delayMs);
    }
}

//Zhou: part of audio call back.
void Audio_Error(void* pContext, AUDIO_ERROR error)
{
    (void)pContext;

    metrics_audio_stack_error(error);
}

//ZHou: called by AudioInput_Write()
size_t ProcessAudioInput(void* pContext, KWS_ID kwsID, uint8_t* pBuffer, size_t byteToWrite)
{
    size_t          ret           = 0;
    SPEECH_CONTEXT* pSC           = (SPEECH_CONTEXT*)pContext;
    KWS_CONTEXT*    pKC           = &pSC->pKWS[kwsID];
    SPEECH_HANDLE   pKWS          = pKC->mKWS;
    size_t          sampleCount   = byteToWrite / sizeof(int16_t);


    if (pSC->fKWSListening && pKWS)
    {
        // The keyword spotter assumes that it runs on a single thread.  Calls
        // to the keyword spotter after initialization should happen here to
        // ensure that they are serialized.

        if (pKC->fKWSReset)
        {
            // encoder is not thread safe, call flush only from primary audio thread
            if (kwsID == KWS_CORTANA)
            {
                Lock(pSC->mSpeechRequestLock);
                audio_encoder_flush();
                Unlock(pSC->mSpeechRequestLock);
            }

            // keyword_spotter_reset(pKWS);
            metrics_kws_reset(kwsID);
            metrics_audio_sentkws(pKC->AudioSamples_KWS);

            pKC->fKWSReset = 0;
        }

        pKC->AudioSamples_KWS += (uint32_t)(sampleCount);
        // keyword_spotter_write(pKWS, pBuffer, byteToWrite);
        ret = byteToWrite;

        // samples were KWS consumed, buffered audio will be sent through detection callback if needed
        goto exit;
    }
    else
    {
        // Audio isn't going to the keyword spotter anymore.  Reset the keyword
        // spotter next time it runs to clear out stale audio samples.
        pKC->fKWSReset = 1;
    }

exit:
    (void)pBuffer;
    return ret;
}

// Zhou: A part of Audio callbacks.
int AudioInput_Write(void* pContext, uint8_t* pBuffer, size_t byteToWrite)
{
    SPEECH_CONTEXT* pSC           = (SPEECH_CONTEXT*)pContext;
    size_t          sampleCount   = byteToWrite / sizeof(int16_t);
    int             ret           = 0;

    ring_buffer_process(pSC->pAudioContext[KWS_CORTANA], (int16_t*)pBuffer, sampleCount);
   
#if defined(IOT_CORTANA_CAPTURE_AUDIO)
#define kCaptureFile      "capture.wav"   
#define fwstring(__s) fwrite(__s, 1, sizeof(__s) - 1, pSC->fpAudioCapture)
#define fwuint32(__n) u32 = __n; fwrite(&u32, 1, 4, pSC->fpAudioCapture)
#define fwuint16(__n) u16 = __n; fwrite(&u16, 1, 2, pSC->fpAudioCapture)

    if (!pSC->fpAudioCapture)
    {
#if defined(WIN32)
        fopen_s(&pSC->fpAudioCapture, kCaptureFile, "wb");
#else
        pSC->fpAudioCapture = fopen(kCaptureFile, "wb");
#endif

        uint32_t u32 = 0;
        uint16_t u16 = 0;

        // write out the RIFF, WAVEFORMAT, and DATA header
        fwstring("RIFF");
        fwuint32(0);
        fwstring("WAVEfmt ");
        fwuint32(2 + 2 + 4 + 4 + 2 + 2);
        fwuint16(1);
        fwuint16(1);
        fwuint32(16000);
        fwuint32(16000 * 2 * 1);
        fwuint16(2);
        fwuint16(16);
        fwstring("data");
        fwuint32(0);
    }

    fwrite(pBuffer, 1, byteToWrite, pSC->fpAudioCapture);
#endif

    audio_input_health_check(pSC);

    // auidio was KWS processed
    if (ProcessAudioInput(pContext, KWS_CORTANA, pBuffer, byteToWrite) != 0)
    {
        goto exit;
    }

    if (pSC->AudioOutputState == AUDIO_STATE_RUNNING &&
        pSC->TTSOutput)
    {
        // drop audio when we're using the speaker and recognizing.
        pSC->AudioSamples_Dropped += (uint32_t)(sampleCount);
        goto exit;
    }

    // don't start sending audio before we complete sending HC buffer
    Lock(pSC->mDetectionLock);
    if (pSC->fKWSListening)
    {
        // we shouldn't analyze and send to service at the same time
        pSC->AudioSamples_Dropped += (uint32_t)(sampleCount);
    }
    else
    {
        pSC->AudioSamples_Reco += (uint32_t)(sampleCount);

        Lock(pSC->mSpeechRequestLock);
        ret = audio_encoder_write(
            pBuffer,
            byteToWrite);
        Unlock(pSC->mSpeechRequestLock);
    }
    Unlock(pSC->mDetectionLock);
exit:
    return ret;
}

#ifdef ENABLE_SKYPE_CHANNEL_KWS
int AudioInput_Write_Skype(void* pContext, uint8_t* pBuffer, size_t byteToWrite)
{
    SPEECH_CONTEXT* pSC          = (SPEECH_CONTEXT*)pContext;
    KWS_CONTEXT*    pKC          = &pSC->pKWS[KWS_SKYPE];
    size_t          sampleCount  = byteToWrite / sizeof(int16_t);
    int             ret          = 0;

    ring_buffer_process(pSC->pAudioContext[KWS_SKYPE], (int16_t*)pBuffer, sampleCount);

    if (pSC->telephony.state == CORTANA_TELEPHONY_IN_CALL)
    {
        // Don't process negative channel during skype call
        pKC->fKWSReset = 1;
        goto exit;
    }

    if (ProcessAudioInput(pContext, KWS_SKYPE, pBuffer, byteToWrite) != 0)
    {
        goto exit;
    }

exit:
    return ret;
}
#endif

// Zhou: A part of Audio callbacks
void AudioInput_StateChanged(void* pContext, AUDIO_STATE state)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;

    LogInfo("AudioInput_StateChanged: %d\n", state);

    if (pSC->mShuttingDown)
    {
        return;
    }

    if (pSC->mCallbacks && pSC->mCallbacks->OnAudioInputState)
    {
        pSC->mCallbacks->OnAudioInputState(pSC->mContext, (SPEECH_STATE)state);
    }
}

// Zhou: a part of Audio callbacks
void AudioOutput_StateChanged(void* pContext, AUDIO_STATE state)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;

    pSC->AudioOutputState = state;
    if (pSC->mCallbacks && pSC->mCallbacks->OnAudioOutputState)
    {
        pSC->mCallbacks->OnAudioOutputState(pSC->mContext, state == AUDIO_STATE_RUNNING ? SPEECH_STATE_RUNNING : SPEECH_STATE_STOPPED);
    }

    if (pSC->TTSOutput)
    {
        pSC->TTSOutput = 0;
        switch (state)
        {
        case AUDIO_STATE_CLOSED:
            break;

        case AUDIO_STATE_RUNNING:
            pSC->TTSOutput = 1;
            cortana_setstate(pSC, CORTANA_STATE_SPEAKING);
            break;

        case AUDIO_STATE_STOPPED:
            cortana_setstate(pSC, CORTANA_STATE_READY);
            break;
        }
    }
    else if (state == AUDIO_STATE_RUNNING)
    {
        metrics_earcon_start();
    }
    else if (state == AUDIO_STATE_STOPPED)
    {
        metrics_earcon_end();
        /*if (cortana_ux_getstate(pSC, AUDIO_STATE_MASK_PLUGGEDIN))
        {
            cortana_ux_clearstate(pSC, AUDIO_STATE_MASK_PLUGGEDIN);
        }*/
    }
}

// Zhou: write the audio data into a file, mainly for log or debug purpose.
static void write_wave_file(const char* filename, const int16_t* pAudioBuffer, size_t sampleSize)
{

    void*    pFile = NULL;

#define appendString(__s) file_write(pFile, __s, sizeof(__s) - 1)
#define appendUINT32(__n) u32 = __n; file_write(pFile, &u32, 4)
#define appendUINT16(__n) u16 = __n; file_write(pFile, &u16, 2)


    pFile = file_open(filename, "wb");
    if (!pFile)
    {
        LogInfo("Cannot open wav file\n");
        return;
    }

    uint32_t u32 = 0;
    uint16_t u16 = 0;

    // write out the RIFF, WAVEFORMAT, and DATA header
    appendString("RIFF");
    appendUINT32(0);
    appendString("WAVEfmt ");
    appendUINT32(2 + 2 + 4 + 4 + 2 + 2);
    appendUINT16(1);
    appendUINT16(1);
    appendUINT32(16000);
    appendUINT32(16000 * 2 * 1);
    appendUINT16(2);
    appendUINT16(16);
    appendString("data");
    assert(sampleSize < (UINT32_MAX / sizeof(int16_t)));
    appendUINT32((uint32_t)(sampleSize * sizeof(int16_t)));

    file_write(pFile, pAudioBuffer, sizeof(int16_t) * sampleSize);
    file_close(pFile);
}

// Zhou: write metadata into the log file. mainly for debug purpose (if WRITE_DUMP_PATH is defined when keyword is rejected.
static void write_meta_file(char const* logName, KWS_STATUS const* pStatus)
{
    STRING_HANDLE const metaName = STRING_construct(logName);
    if (STRING_concat(metaName, "-meta") == 0)
    {
        FILE* const pMetaFile = fopen(STRING_c_str(metaName), "w");
        if (pMetaFile != NULL)
        {
            fprintf(pMetaFile, "rejectReason:%d ", pStatus->rejectReason);
            fprintf(pMetaFile, "confidence:%f ", pStatus->confidence);
            fprintf(pMetaFile, "kwsLanguage:%d ", pStatus->kwsLanguage);
            fprintf(pMetaFile, "kwsModel:%d.%d.%d ",
                pStatus->kwsModelFormatMajor,
                pStatus->kwsModelFormatMinor,
                pStatus->kwsModelSourcesVersion);
            fprintf(pMetaFile, "kwsThreshold:%f ", pStatus->kwsThreshold);

            fclose(pMetaFile);
        }
    }

    if (metaName != NULL)
    {
        STRING_delete(metaName);
    }
}

//Zhou: write the audio buffer into file for debug
void cortana_dump_audio_buffer(
    CORTANA_HANDLE speechHandle,
    const char* pszPath)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)speechHandle;

    size_t windowSize = pSC->pAudioContext[KWS_CORTANA]->size;

    ring_buffer_window(pSC->pAudioContext[KWS_CORTANA], windowSize, pSC->dumpBuffer);

    write_wave_file(pszPath, pSC->dumpBuffer, windowSize);

#ifdef ENABLE_SKYPE_CHANNEL_KWS
    char path[1024];

    strcpy(path, "skype-");
    strcat(path, pszPath);

    windowSize = pSC->pAudioContext[KWS_SKYPE]->size;
    ring_buffer_window(pSC->pAudioContext[KWS_SKYPE], windowSize, pSC->dumpBuffer);
    write_wave_file(path, pSC->dumpBuffer, windowSize);
#endif
}

// Zhou: called by KwsONStatus: KEYWORD_DETECTED, send audio. It bascially encodes the data into internal buffer, but not really send out. 
// Only when audio_encoder_flush() is called, the data is sent out by calling EncodeOnData call back.
static void AudioEncoderWrite(SPEECH_CONTEXT* pSC, const KWS_STATUS* pStatus)
{
    // Using mono PCM bitrate as AUDIO_SAMPLE_RATE_HZ * 16 * 1
    // This metric must be after generating a request id and before start of audio.
    int bufferSamples = (int)(pStatus->detectionBufferSize / sizeof(int16_t));
    int startIndex = bufferSamples + pStatus->startSampleOffset;
    int endIndex = bufferSamples + pStatus->endSampleOffset;

    int triggerStartTimeOffsetInMilliSeconds = AUDIO_SAMPLES_TO_MS(-pStatus->startSampleOffset);
    int initialSilenceTimeOffsetInMilliSeconds = AUDIO_SAMPLES_TO_MS(bufferSamples);

    assert(endIndex >= startIndex);
    int kwsSamples = endIndex - startIndex;

    metrics_keywordspotter_acceptedkeyword(triggerStartTimeOffsetInMilliSeconds, initialSilenceTimeOffsetInMilliSeconds);

    pSC->kw_startoffset = (double)startIndex / (double)(AUDIO_SAMPLE_RATE_HZ);
    pSC->kw_duration = (double)kwsSamples / (double)(AUDIO_SAMPLE_RATE_HZ);

    audio_encoder_write(
        (const uint8_t*)pStatus->detectionBuffer,
        pStatus->detectionBufferSize);
}

// Zhou: callbacks for KwsCallbacks
static void KwsOnStatus(void* pContext, const KWS_STATUS* pStatus)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    Lock(pSC->mDetectionLock);

    if(pSC->KWTriggered)
    {
        LogInfo("Kws already triggered on Negative, ignore Positive callback");
        goto exit;
    }

    char kwVersion[64];

    sprintf_s(kwVersion, sizeof(kwVersion), "%d.%d.%d",
        pStatus->kwsModelFormatMajor,
        pStatus->kwsModelFormatMinor,
        pStatus->kwsModelSourcesVersion);

    LogInfo("KwsOnStatus: %d %lf", pStatus->result, pStatus->confidence);

    metrics_kws_confidence(pStatus->confidence);
    metrics_kws_threshold(pStatus->kwsThreshold);
    metrics_kws_version(kwVersion);

    switch (pStatus->result)
    {
        case KEYWORD_DETECTED:
            metrics_kws_detected(KWS_CORTANA);

            pSC->kw_confidence = pStatus->confidence;
            if (pSC->mCallbacks && pSC->mCallbacks->OnKeywordState)
            {
                pSC->mCallbacks->OnKeywordState(pSC->mContext, SPEECH_STATE_STOPPED);
            }

            // Keyword spotter detection indicates the start of a turn
            
            // Lock to prevent parallel speech requests.
            Lock(pSC->mSpeechRequestLock);
            // prevents races for agent events in progress.
            pSC->KWTriggered = 1;
            // cancel any pending listen signals (typically barge-in), the service will reprompt if needed.
            pSC->PendingListen = 0;
            Unlock(pSC->mSpeechRequestLock);

            // cortana state doesn't touch the network, and not required to be 
            // behind the speech request lock.
            // cortana_setstate_internal(pSC, CORTANA_STATE_LISTENING, CORTANA_REASON_KWS_PRIMARY);

            // Lock to protect again transmission and ensure request id syncronization.
            Lock(pSC->mSpeechRequestLock);

            // Reset request ID for a new turn
            cortana_reset_speech_request_id(pSC);

            AudioEncoderWrite(pSC, pStatus);

            Unlock(pSC->mSpeechRequestLock);

#if defined(IOT_CORTANA_CAPTURE_AUDIO)
            write_wave_file("capture_kws.wav", pStatus->detectionBuffer, bufferSamples);
            LogInfo("write_wave_file: start %.6lf s duration %.6lf s", pSC->kw_startoffset, pSC->kw_duration);
#endif
            break;

        case KEYWORD_REJECTED:
        {

            metrics_kws_rejected(KWS_CORTANA);
#ifdef WRITE_DUMP_PATH
            char const logPrefix[] = WRITE_DUMP_PATH "/kws-reject";
            char const logSuffix[] = ".wav";
            size_t const guidSize = 37;
            char logName[sizeof(logPrefix) + guidSize + sizeof(logSuffix)];

            strncpy(logName, logPrefix, sizeof(logName));
            char* const guidName =
                logName + sizeof(logPrefix) - 1 /* exclude null terminator */;

            if (UniqueId_Generate(guidName, guidSize) == UNIQUEID_OK)
            {
                strncpy(logName + sizeof(logPrefix) + guidSize - 2,
                    logSuffix, sizeof(logSuffix));

                // Write the meta file before the WAV file because our uploader
                // scripts will upload if a WAV file exists.  We want the
                // uploader to see the meta file for every WAV file.
                write_meta_file(logName, pStatus);

                write_wave_file(logName, pStatus->detectionBuffer, pStatus->detectionBufferSize / sizeof(int16_t));
            }
#endif
            break;
        }

        default:
            goto exit;
    }

exit:
    Unlock(pSC->mDetectionLock);
}
#ifdef ENABLE_SKYPE_CHANNEL_KWS
static void KwsOnStatusSkype(void* pContext, const KWS_STATUS* pStatus)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    Lock(pSC->mDetectionLock);

    if(pSC->KWTriggered)
    {
        LogInfo("Kws already triggered on Positive, ignore Negative callback");
        goto exit;
    }

    char kwVersion[64];

    sprintf_s(kwVersion, sizeof(kwVersion), "%d.%d.%d",
              pStatus->kwsModelFormatMajor,
              pStatus->kwsModelFormatMinor,
              pStatus->kwsModelSourcesVersion);

    LogInfo("KwsOnStatus Skype: %d %lf", pStatus->result, pStatus->confidence);

    metrics_kws_confidence(pStatus->confidence);
    metrics_kws_threshold(pStatus->kwsThreshold);
    metrics_kws_version(kwVersion);

    switch (pStatus->result)
    {
        case KEYWORD_DETECTED:
            metrics_kws_detected(KWS_SKYPE);

            pSC->kw_confidence = pStatus->confidence;
            if (pSC->mCallbacks && pSC->mCallbacks->OnKeywordState)
            {
                pSC->mCallbacks->OnKeywordState(pSC->mContext, SPEECH_STATE_STOPPED);
            }

            // Keyword spotter detection indicates the start of a turn

            // Lock to prevent parallel speech requests.
            Lock(pSC->mSpeechRequestLock);
            // prevents races for agent events in progress.
            pSC->KWTriggered = 1;
            // cancel any pending listen signals (typically barge-in), the service will reprompt if needed.
            pSC->PendingListen = 0;
            Unlock(pSC->mSpeechRequestLock);

            // cortana state doesn't touch the network, and not required to be 
            // behind the speech request lock.
            cortana_setstate_internal(pSC, CORTANA_STATE_LISTENING, CORTANA_REASON_KWS_SECONDARY);

            // Lock to protect again transmission and ensure request id syncronization.
            Lock(pSC->mSpeechRequestLock);

            // Reset request ID for a new turn
            transport_create_request_id(pSC->mSpeechRequest);

            AudioEncoderWrite(pSC, pStatus);

            Unlock(pSC->mSpeechRequestLock);

#if defined(IOT_CORTANA_CAPTURE_AUDIO)
            write_wave_file("capture_kws.wav", pStatus->detectionBuffer, bufferSamples);
            LogInfo("write_wave_file: start %.6lf s duration %.6lf s", pSC->kw_startoffset, pSC->kw_duration);
#endif
            break;

        case KEYWORD_REJECTED:
        {
            metrics_kws_rejected(KWS_SKYPE);
#ifdef WRITE_DUMP_PATH
            char const logPrefix[] = WRITE_DUMP_PATH "/kws-reject-skype";
            char const logSuffix[] = ".wav";
            size_t const guidSize = 37;
            char logName[sizeof(logPrefix) + guidSize + sizeof(logSuffix)];

            strncpy(logName, logPrefix, sizeof(logName));
            char* const guidName =
                logName + sizeof(logPrefix) - 1 /* exclude null terminator */;

            if (UniqueId_Generate(guidName, guidSize) == UNIQUEID_OK)
            {
                strncpy(logName + sizeof(logPrefix) + guidSize - 2,
                        logSuffix, sizeof(logSuffix));

                // Write the meta file before the WAV file because our uploader
                // scripts will upload if a WAV file exists.  We want the
                // uploader to see the meta file for every WAV file.
                write_meta_file(logName, pStatus);

                write_wave_file(logName, pStatus->detectionBuffer, pStatus->detectionBufferSize / sizeof(int16_t));
            }
#endif
            break;
        }

        default:
            goto exit;
    }

exit:
    Unlock(pSC->mDetectionLock);
}


KWS_CALLBACKS kwsCallbacksSkype = {
    sizeof(KWS_CALLBACKS),
    KWS_CALLBACKS_VERSION,
    NULL,
    KwsOnStatusSkype,
};

#endif

KWS_CALLBACKS kwsCallbacks = {
    sizeof(KWS_CALLBACKS),
    KWS_CALLBACKS_VERSION,
    NULL,
    KwsOnStatus,
};

// Zhou: Callback when Speech.TurnEnd message is received.
int skill_TurnEnd(PROPERTYBAG_HANDLE hProperty, void* pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;

    (void)hProperty;
    
    if (pSC->hTurnTimeout)
    {
        // cortana_timer_destroy(pSC->hTurnTimeout);
        pSC->hTurnTimeout = NULL;
    }

    if (pSC->hThinkingEarcon)
    {
        // cortana_timer_destroy(pSC->hThinkingEarcon);
        pSC->hThinkingEarcon = NULL;
    }

    Lock(pSC->mSpeechRequestLock);
    (void)audio_encoder_flush();
    Unlock(pSC->mSpeechRequestLock);

    // we only set the state to ready from a turn.end on an audio turn.
    if (pSC->AudioTurn)
    {
        (void)cortana_setstate(pSC, CORTANA_STATE_READY);
    }

    Lock(pSC->mSpeechRequestLock);
    cortana_reset_speech_request_id(pSC);
    Unlock(pSC->mSpeechRequestLock);

    return 0;
}

void OnRecognitionStarted(SPEECH_CONTEXT* pSC)
{
    (void)pSC;
}

//zhou: Creaet a new request id.
void cortana_reset_speech_request_id(SPEECH_CONTEXT* pSC)
{
    transport_create_request_id(pSC->mSpeechRequest);

    pSC->ResponseRecv = 0;

    // If the request ID changes while an agent event is in progress (e.g. due
    // to a barge-in of speech), then we won't see the response for the agent
    // event.  Mark any in-progress agent events as completed.  They will be
    // retried if they haven't hit their limit.
    //
    // In most cases, agent_handle_response would already have been called by
    // this point.
    // agent_handle_response(pSC, CORTANA_ERROR_GENERIC);
}

//ZHou: callback for Speech.EndDetected
int skill_EndOfSpeech(PROPERTYBAG_HANDLE hProperty, void* pContext)
{
    (void)hProperty;

    OnRecognitionStarted((SPEECH_CONTEXT*)pContext);

    // state transition to thinking, update our turn timer.
    UpdateTurnTimeout((SPEECH_CONTEXT*)pContext);

    (void)cortana_setstate(pContext, CORTANA_STATE_THINKING);
    return 0;
}


//zhou: called when Speech.TurnEnd is received, or turn is cancelled.
static void TurnEnd(void* pContext)
{
    skill_dispatch(kApiPath_TurnEnd, NULL, pContext);
}

// Zhou: called when turn is cancelled due to timeout, error in respose or 
static void TurnCancelled(CORTANA_ERROR error, void* pContext)
{
    // Only proceed if we are in a turn or agent event.  
    // Network errors where the connection drops unexepectedly 
    // and we aren't expecting a response can be dropped on 
    // the floor.  Let CORTANA_ERROR_TIMEOUT through, because
    // it isn't driven from a network based signal.
    if ((error == CORTANA_ERROR_TIMEOUT) || 
        CORTANA_IN_TURN(((SPEECH_CONTEXT*)pContext)))
    {
        // cortana_onerror(pContext, error);
        TurnEnd(pContext);

        // When any bad thing happended for the speech transport, we treat it as heartbeat offline
        cortana_system_setstate(EV_HEART_BEAT_OFFLINE);
    }
}

// Zhou: called by timer when timeout. see below
static void SpeechSessionTimeout(
    TIMER_HANDLE        hTimer,
    void*               pContext
)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    if (pSC->hTurnTimeout != hTimer)
    {
        return;
    }

    pSC->hTurnTimeout = NULL;

    LogError("Speech session timed out");
    TurnCancelled(CORTANA_ERROR_TIMEOUT, pContext);
}

// Zhou: set the timer for timeout
void UpdateTurnTimeout(
    SPEECH_CONTEXT* pSC)
{
    (void)pSC;
    /*if (pSC)
    {
        if (pSC->hTurnTimeout)
        {
            cortana_timer_destroy(pSC->hTurnTimeout);
        }

        pSC->hTurnTimeout = cortana_timer_create(
            ANSWER_TIMEOUT_MS,
            SpeechSessionTimeout,
            "Speech Session Timeout",
            pSC);
    }*/
}


// ZHou: create SPEECH_CONTEXT, and dns_cache
SPEECH_RESULT
speech_open(
    SPEECH_HANDLE*  ppHandle,
    uint32_t        reserved,
    void*           pReserved
    )
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)malloc(sizeof(SPEECH_CONTEXT));

    (void)reserved;
    (void)pReserved;

    memset(pSC, 0, sizeof(SPEECH_CONTEXT));
    pSC->AudioOutputState = AUDIO_STATE_STOPPED;
    pSC->CreateTime = cortana_gettickcount();
    pSC->EarconSetting = EARCON_SETTING_DISABLED;
    pSC->hDnsCache = dns_cache_create();
    if (pSC->hDnsCache)
    {
        *ppHandle = pSC;

        return 0;
    }

    free(pSC);

    return -1;
}

// Zhou: clean up and free the speech context
SPEECH_RESULT
speech_close(
    SPEECH_HANDLE hSpeech
    )
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)hSpeech;

    int i;

    if (!pSC)
    {
        return -1;
    }

    pSC->mShuttingDown = 1;
    metrics_microphone_stop();
    for (i = 0; i < MAX_DEVICES; i++)
    {
        if (!pSC->mAudioConfig[i].hDevice)
        {
            continue;
        }

        audio_input_stop(pSC->mAudioConfig[i].hDevice);
        audio_output_stop(pSC->mAudioConfig[i].hDevice);
        audio_destroy(pSC->mAudioConfig[i].hDevice);
        pSC->mAudioConfig[i].hDevice = NULL;
        pSC->mAudioConfig[i].index = 0;
    }

    free(pSC->dumpBuffer);

    ring_buffer_delete(pSC->pAudioContext[KWS_CORTANA]);

    if (pSC->pKWS)
    {
        for(size_t idx = 0; idx < pSC->numKWS; ++idx)
        {
            // keyword_spotter_close(pSC->pKWS[idx].mKWS);
        }

        free(pSC->pKWS);
    }

#ifdef ENABLE_SKYPE_CHANNEL_KWS
    ring_buffer_delete(pSC->pAudioContext[KWS_SKYPE]);
#endif

    for (i = 0; i < TRANSPORT_REQ_COUNT; i++)
    {
        transport_request_destroy(pSC->mTransportRequest[i]);
    }

    if (pSC->mSpeechRequestLock)
    {
        Lock_Deinit(pSC->mSpeechRequestLock);
    }

    if (pSC->DataLock)
    {
        // agent_shutdown_events(pSC);
        Lock_Deinit(pSC->DataLock);
    }

    if (pSC->PlaylistLock)
    {
        Lock_Deinit(pSC->PlaylistLock);
    }

    if (pSC->HealthLock)
    {
        Lock_Deinit(pSC->HealthLock);
    }

    if (pSC->mDetectionLock)
    {
        Lock_Deinit(pSC->mDetectionLock);
    }

    if (pSC->hPlayList)
    {
        list_destroy(pSC->hPlayList);
    }

    if (pSC->hDnsCache)
    {
        dns_cache_destroy(pSC->hDnsCache);
    }

    free(pSC);

    audio_encoder_uninitialize();
    audio_decoder_uninitialize();

    return 0;
}

// Zhou: Set callbacks for the specific SpeechContext
SPEECH_RESULT
SPEECH_DLL_EXPORT
speech_setcallbacks(
    SPEECH_HANDLE    hSpeech,
    uint32_t         uiReserved,
    CORTANA_CALLBACKS *pCallbacks,
    void*            pContext
    )
{
    SPEECH_CONTEXT* pSC;

    (void)uiReserved;

    if (!hSpeech)
    {
        return -1;
    }

    pSC = (SPEECH_CONTEXT*)hSpeech;
    pSC->mCallbacks = pCallbacks;
    pSC->mContext = pContext;

    return 0;
}

// Zhou: Create an audio entry in the AUdioConfig, called from  speech_initialize() and evtl. audio_manager.c
int cortana_create_audio(
    SPEECH_HANDLE           handle,
    const char*             pszName)
{
    SPEECH_CONTEXT*  pSC = (SPEECH_CONTEXT*)handle;
    int              i;
    STRING_HANDLE hName;
    BUFFER_HANDLE hBuffer;

    if (!pSC)
    {
        return -1;
    }

    for (i = 0; i < MAX_DEVICES; i++)
    {
        if (pSC->mAudioConfig[i].hDevice)
        {
            continue;
        }

        pSC->mAudioConfig[i].hDevice = audio_create();
        if (pSC->mAudioConfig[i].hDevice)
        {
            if (NULL != pszName && audio_set_options(pSC->mAudioConfig[i].hDevice, AUDIO_OPTION_DEVICENAME, pszName))
            {
                LogError("cortana_create_audio(%s) failed", pszName);
                audio_destroy(pSC->mAudioConfig[i].hDevice);
                return -1;
            }

            // set the initial audio
            pSC->mAudioConfig[i].initvol = INVALID_VOLUME;

            // attempt to read the initial system volume from storage.
            hName = STRING_construct(pszName);
            if (NULL != hName)
            {
                STRING_concat(hName, ".initvol"); // kAudioSetting_InitVolume);
                hBuffer = NULL; // cortana_storage_read(STRING_c_str(hName));
                if (NULL != hBuffer)
                {
                    if (BUFFER_length(hBuffer) >= sizeof(long))
                    {
                        pSC->mAudioConfig[i].initvol = *(long*)BUFFER_u_char(hBuffer);
                        audio_output_set_volume(
                            pSC->mAudioConfig[i].hDevice,
                            pSC->mAudioConfig[i].initvol);
                    }

                    BUFFER_delete(hBuffer);
                }

                // volume wasn't initialized.  Set it to the default.
                if (pSC->mAudioConfig[i].initvol == INVALID_VOLUME)
                {
                    pSC->mAudioConfig[i].initvol = DEFAULT_VOLUME;
                    audio_output_set_volume(
                        pSC->mAudioConfig[i].hDevice,
                        pSC->mAudioConfig[i].initvol);
                }

                STRING_delete(hName);
            }

            (void)audio_setcallbacks(
                pSC->mAudioConfig[i].hDevice,
                AudioOutput_StateChanged,
                handle,
                AudioInput_StateChanged,
                handle,
                AudioInput_Write,
                handle,
                Audio_Error,
                handle);
            return i;
        }
    }

    LogError("Failed to initialize audio input");
    return -1;
}


// Zhou Speech initialize: creaet audio, initialize encoder, Speech_Initialize().
SPEECH_RESULT
SPEECH_DLL_EXPORT
speech_initialize(
    SPEECH_HANDLE hSpeech,
    TokenStore token_store,
    const char* keyword_table_path
)
{
    SPEECH_RESULT   ret = 0;
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)hSpeech;

    //Todo: remove the parameter
    (void)keyword_table_path;

    // initialize the default audio device.
    ret = cortana_create_audio(hSpeech, NULL);
    if (ret)
    {
        LogInfo("WARNING: Failed to initialize default audio");
        ret = 0;
    }

    pSC->token_store = token_store;

#if defined(USE_KEYWORD_SPOTTER)
    if (ret == 0)
    {
        ret = keyword_spotter_initialize();
    }

    if (ret == 0)
    {
        // initialize audio buffering
        pSC->dumpBuffer       = (int16_t*) malloc (sizeof(int16_t) * MAX_AUDIO_BUFFER_SIZE);
        if (pSC->dumpBuffer == NULL)
        {
            return -1;
        }

#ifdef ENABLE_SKYPE_CHANNEL_KWS
        pSC->numKWS = 2;
#else
        pSC->numKWS = 1;
#endif // ENABLE_SKYPE_CHANNEL_KWS
        pSC->pKWS = (KWS_CONTEXT*) malloc (sizeof(KWS_CONTEXT) * pSC->numKWS);
        if (pSC->pKWS == NULL)
        {
            return -1;
        }

        pSC->pAudioContext = (ring_buffer_t**) malloc (sizeof(ring_buffer_t*) * pSC->numKWS);
        if (pSC->pAudioContext == NULL)
        {
            return -1;
        }

#if defined(GB_DEBUG_ALLOC)
        keyword_spotter_setalloc(gballoc_malloc, gballoc_free);
#endif // GB_DEBUG_ALLOC
 
        for(size_t i = 0; i < pSC->numKWS; ++i)
        {
            ring_buffer_new(&pSC->pAudioContext[i], MAX_AUDIO_BUFFER_SIZE);

            KWS_CONTEXT* pKC = &pSC->pKWS[i];

            if (keyword_spotter_open(&pKC->mKWS, keyword_table_path))
            {
                LogInfo("WARNING: Keyword spotter for skype failed to open.  Check that you have the right models installed.\n");
            }

            pKC->fKWSReset = 0;
            pKC->AudioSamples_KWS = 0;

            switch((KWS_ID)i)
            {
                case KWS_CORTANA:
                {
                    if (ret == 0 && pKC->mKWS)
                    {
                        ret = keyword_spotter_setcallbacks(pKC->mKWS, &kwsCallbacks, pSC);
                    }
                    break;
                }
#ifdef ENABLE_SKYPE_CHANNEL_KWS
                case KWS_SKYPE:
                {
                    if (ret == 0 && pKC->mKWS)
                    {
                        ret = keyword_spotter_setcallbacks(pKC->mKWS, &kwsCallbacksSkype, pSC);
                    }
                    break;
                }
#endif // ENABLE_SKYPE_CHANNEL_KWS
                default:
                {
                    LogError("Unknown KWS ID");
                    ret = -1;
                }
            }
        }
    }
#endif // USE_KEYWORD_SPOTTER

    if (ret == 0)
    {
        // setup a default audio route
        ret = audio_encoder_initialize(AudioEncode_OnData, pSC);
    }

    // prepare each component
    if (ret == 0)
    {
        ret = Speech_Initialize(pSC);
    }

    return ret;
}

// Zhou: set audio device based on state: start/stop audio input.
//SPEECH_RESULT
//SPEECH_DLL_EXPORT
//conversation_audio_setstate(
//    SPEECH_HANDLE hConversation,
//    SPEECH_STATE  state
//    )
//{
//    CONV_CONTEXT* pCC = (CONV_CONTEXT*)hConversation;
//    SPEECH_RESULT ret = -1;
//    AUDIO_SYS_HANDLE hSpeechDevice;
//
//    hSpeechDevice = cortana_getaudiodevice(hConversation, CORTANA_AUDIO_TYPE_SPEECH);
//    if (!hSpeechDevice)
//    {
//        return -1;
//    }
//#ifdef ENABLE_SKYPE_CHANNEL_KWS
//    AUDIO_SYS_HANDLE hSkypeDevice;
//    hSkypeDevice  = cortana_getaudiodevice(hConversation, CORTANA_AUDIO_TYPE_TELEPHONY_INPUT);
//    if (!hSkypeDevice)
//    {
//        return -1;
//    }
//#endif
//
//    switch (state)
//    {
//        case SPEECH_STATE_RUNNING:
//            if (pCC->pKWS)
//            {
//                pCC->fKWSListening = 1;
//                pCC->KWTriggered = 0;
//                metrics_kws_enabled();
//            }
//
//            metrics_microphone_start();
//            audio_set_options(hSpeechDevice, "buff_frame_cnt", &kKWSFrameCount);
//            ret = audio_input_start(hSpeechDevice);
//
//#ifdef ENABLE_SKYPE_CHANNEL_KWS
//            audio_set_options(hSkypeDevice, "buff_frame_cnt", &kKWSFrameCount);
//            audio_set_options(hSkypeDevice, "write_cb", AudioInput_Write_Skype);
//            ret = audio_input_start(hSkypeDevice);
//#endif
//            break;
//        case SPEECH_STATE_STOPPED:
//            metrics_microphone_stop();
//            ret = audio_input_stop(hSpeechDevice);
//#ifdef ENABLE_SKYPE_CHANNEL_KWS
//            ret = audio_input_stop(hSkypeDevice);
//#endif
//            break;
//        default:
//            break;
//    }
//
//    return ret;
//}

// Zhou: called by AudioEncode_OnData callback: write audio stream to the service: call transport to write/flush stream
SPEECH_RESULT
SPEECH_DLL_EXPORT
audiostream_write(
    SPEECH_HANDLE hStream,
    const void *  pData,
    uint32_t      ui32Size,
    uint32_t *    pui32Written
    )
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)hStream;
    const char* httpArgs = "/audio";
    int ret = -1;

    if (!hStream || !pData || !ui32Size)
    {
        return -1;
    }

    Lock(pSC->mSpeechRequestLock);

    metrics_audiostream_data(ui32Size);
    if (!ui32Size)
    {
        ret = transport_stream_flush(pSC->mSpeechRequest);
    }
    else
    {
        if (!pSC->mAudioOffset)
        {
            metrics_audiostream_init();
            // user initiated listening and multi-turn require the request id to 
            // be re-created here to ensure barge-in scenerios work.
            if (!pSC->KWTriggered)
            {
                cortana_reset_speech_request_id(pSC);
            }

            metrics_audio_start();
            pSC->AudioTurn = pSC->SpeechTurn = 1;

            ret = transport_stream_prepare(pSC->mSpeechRequest, httpArgs);
            if (ret)
            {
                Unlock(pSC->mSpeechRequestLock);
                return ret;
            }

            UpdateTurnTimeout(pSC);
        }

        ret = transport_stream_write(
            pSC->mSpeechRequest, 
            (uint8_t*)pData, 
            ui32Size);
    }

    Unlock(pSC->mSpeechRequestLock);

    pSC->mAudioOffset += ui32Size;

    if (!ret && NULL != pui32Written)
    {
        *pui32Written = ui32Size;
    }

    return ret;
}

// Zhou: called by AudioEncode_OnData callback: write audio stream to the service: call transport:stream_flush
SPEECH_RESULT
SPEECH_DLL_EXPORT
audiostream_flush(
    SPEECH_HANDLE hStream
    )
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)hStream;
    int ret;
    if (!hStream)
    {
        return -1;
    }
    else if (!pSC->mAudioOffset)
    {
        return 0;
    }

    ret = transport_stream_flush(pSC->mSpeechRequest);
    pSC->mAudioOffset = 0;
    metrics_audiostream_flush();
    metrics_audio_end();

    return ret;
}

// Zhou: callback for transport error
static void TransportError(
    TRANSPORT_HANDLE        hTransport, 
    enum transport_error    reason,
    void*                   pContext)
{
    (void)hTransport;
    CORTANA_ERROR cortanaError;

    switch (reason)
    {
    default:
        cortanaError = CORTANA_ERROR_GENERIC;
        break;

    case transport_error_authentication:
        // Before oauth token be exchanged, the heartbeat will fail with this error, but at this time, we still want to trigger the online event
        cortana_system_setstate(EV_HEART_BEAT_ONLINE);
        // We should not treat temp authentication error as NO_TOKEN as this will tell user do OOBE again.
        cortanaError = CORTANA_ERROR_AUTH_ERROR;
        break;

    case transport_error_connection_failure:
    case transport_error_dns_failure:
    case transport_error_remoteclosed:
        cortanaError = CORTANA_ERROR_NOT_ONLINE;
        break;
    }

    TurnCancelled(cortanaError, pContext);
}

//zhou: callback for Speech.EndDetected handler
static void SpeechEnd_Handler(
    TRANSPORT_HANDLE     hTransport,
    const char*          pszPath,
    const char*          pszMime,
    const unsigned char* buffer,
    size_t               size,
    void*                pContext)
{
    (void)hTransport;
    (void)pszMime;
    (void)buffer;
    (void)size;

    skill_dispatch(pszPath, NULL, pContext);
}

// zhou: callback for Speech.TurnEnd 
static void TurnEnd_PathHandler(
    TRANSPORT_HANDLE     hTransport,
    const char*          pszPath,
    const char*          pszMime,
    const unsigned char* buffer,
    size_t               size,
    void*                pContext)
{
    (void)hTransport;
    (void)pszPath;
    (void)pszMime;
    (void)buffer;
    (void)size;

    // raise an error if we didn't get a cortana response at the end of a turn 
    // and we were triggered via voice.
    if (((SPEECH_CONTEXT*)pContext)->AudioTurn && 
        !((SPEECH_CONTEXT*)pContext)->ResponseRecv)
    {
        // cortana_onerror(pContext, CORTANA_ERROR_NO_RESPONSE);
    }
    else
    {
        metrics_transport_platformerror(CORTANA_SUCCESS, 0 /* audible */);

        // Mark the response as successful as long as we receive a response,
        // even if that response contains an error.  We do this for simplicity,
        // to avoid retrying bad requests.
        // agent_handle_response(pContext, CORTANA_SUCCESS);
    }

    telemetry_flush();

    TurnEnd(pContext);
}


// Zhou: callback for Speech.TurnStart, Speech.Hypothesis, Speech.Phrase, and also for Response.
static void Content_PathHandler(
    TRANSPORT_HANDLE     hTransport,
    const char*          pszPath,
    const char*          pszMime,
    const unsigned char* buffer,
    size_t               size,
    void*                pContext)
{
    (void)hTransport;
    (void)pszPath;

    if (size == 0)
    {
        PROTOCOL_VIOLATION("response contains no body");
        return;
    }

    (void)Content_DispatchBuffer(
        pContext,
        pszPath,
        pszMime,
        buffer,
        size);
}

// Zhou: callback for SpeechStartDetected.
static void SpeechStart_Handler(
    TRANSPORT_HANDLE     hTransport,
    const char*          pszPath,
    const char*          pszMime,
    const unsigned char* buffer,
    size_t               size,
    void*                pContext)
{
    (void)hTransport;
    (void)pszPath;
    (void)pszMime;
    (void)buffer;
    (void)size;

    // let the ux know that the service has entered the listening state.
    // cortana_ux_onlisten(pContext, UX_LISTEN_SERVICE, CORTANA_REASON_NONE);

    OnRecognitionStarted((SPEECH_CONTEXT*)pContext);
}

// Zhou: Callback for http response
static void CortanaResponse_Handler(
    TRANSPORT_HANDLE     hTransport,
    const char*          pszPath,
    const char*          pszMime,
    const unsigned char* buffer,
    size_t               size,
    void*                pContext)
{
    ((SPEECH_CONTEXT*)pContext)->ResponseRecv = 1;

    Content_PathHandler(
        hTransport,
        pszPath,
        pszMime,
        buffer,
        size,
        pContext);
}

// Zhou Dispatch table for PATH message
struct _PATHHANDLER
{
    const char*            pszPath;
    PRESPONSE_PATH_HANDLER handler;
} gPathHandlers[] = {
    { kApiPath_TurnStart,           Content_PathHandler },
    { kApiPath_SpeechStartDetected, SpeechStart_Handler },
    { kApiPath_SpeechEndDetected,   SpeechEnd_Handler   },
    { kApiPath_TurnEnd,             TurnEnd_PathHandler },
    { kApiPath_Speech_Hypothesis,   Content_PathHandler },
    { kApiPath_Speech_Phrase,       Content_PathHandler },
    { kApiPath_Response,            CortanaResponse_Handler },
};


// Zhou: callback for data available on tranport
static void ResponseHandler(
    TRANSPORT_HANDLE     hTransport,
    HTTP_HEADERS_HANDLE  hResponseHeader,
    const unsigned char* buffer,
    size_t               size,
    unsigned int         errorCode,
    void*                pContext)
{
    const char *        pszPath;
    const char *        pszContentType = NULL;

    if (errorCode != 0)
    {
        LogError("Response error %d", errorCode);
        // TODO: Lower layers need appropriate signals
        TurnCancelled(CORTANA_ERROR_GENERIC, pContext);
        return;
    }
    else if (!pContext || NULL == hResponseHeader)
    {
        return;
    }

    pszPath = HTTPHeaders_FindHeaderValue(hResponseHeader, kPath);
    if (!pszPath)
    {
        PROTOCOL_VIOLATION("response missing '" kPath "' header");
        return;
    }

    if (size != 0)
    {
        pszContentType = HTTPHeaders_FindHeaderValue(hResponseHeader, kContent_Type);
        if (NULL == pszContentType)
        {
            PROTOCOL_VIOLATION("response '%s' contains body with no content-type", pszPath);
            return;
        }
    }

    // When it goes to here, it means the speech transport is health(can talk with the server correctly), in this case, we think heartbeat is online
    cortana_system_setstate(EV_HEART_BEAT_ONLINE);

    for (int i = 0; i < sizeof(gPathHandlers) / sizeof(gPathHandlers[0]); i++)
    {
        if (!strcmp(pszPath, gPathHandlers[i].pszPath))
        {
            gPathHandlers[i].handler(
                hTransport,
                pszPath,
                pszContentType,
                buffer,
                size,
                pContext);
            return;
        }
    }

    PROTOCOL_VIOLATION("unhandled response '%s'", pszPath);
    metrics_transport_unhandledresponse();
}

// zhou: called by speech_initialize. Intialize transport and URL
SPEECH_RESULT 
Speech_Initialize(
    SPEECH_CONTEXT* pSC
    )
{
    const char *ep = kSpeechEndpoint;

    // check for overrides
    //BUFFER_HANDLE hSpeechUrl = cortana_storage_read("SPEECHURL");
    //if (hSpeechUrl)
    //{
    //    // null terminate
    //    size_t size = BUFFER_length(hSpeechUrl);
    //    if (!BUFFER_enlarge(hSpeechUrl, 1))
    //    {
    //        ep = (const char*)BUFFER_u_char(hSpeechUrl);
    //        BUFFER_u_char(hSpeechUrl)[size] = 0;
    //    }
    //}

    pSC->mSpeechRequest = transport_request_create(ep, pSC);
    //if (hSpeechUrl)
    //{
    //    BUFFER_delete(hSpeechUrl);
    //}

    if (NULL == pSC->mSpeechRequest)
    {
        return -1;
    }

    transport_set_dns_cache(pSC->mSpeechRequest, pSC->hDnsCache);
    transport_setcallbacks(pSC->mSpeechRequest, TransportError, ResponseHandler);
    transport_request_addrequestheader(pSC->mSpeechRequest, "User-Agent", kUserAgent);
    transport_set_tokenstore(pSC->mSpeechRequest, pSC->token_store);

    pSC->mSpeechRequestLock = Lock_Init();
    if (!pSC->mSpeechRequestLock)
    {
        return -1;
    }

    pSC->DataLock = Lock_Init();
    if (!pSC->DataLock)
    {
        return -1;
    }

    pSC->PlaylistLock = Lock_Init();
    if (!pSC->PlaylistLock)
    {
        return -1;
    }

    pSC->HealthLock = Lock_Init();
    if (!pSC->HealthLock)
    {
        return -1;
    }

    pSC->mDetectionLock = Lock_Init();
    if (!pSC->mDetectionLock)
    {
        return -1;
    }

    pSC->hPlayList = list_create();
    if (!pSC->hPlayList)
    {
        return -1;
    }

    /*if (agent_initialize_events(pSC))
    {
        return -1;
    }*/

    return 0;
}

// zhou: callback for Skill:SpeechRecognizer
int skill_speechRecognizer(PROPERTYBAG_HANDLE hProperty, void* pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    if (!pSC)
    {
        return -1;
    }

    const char *action = propertybag_getstringvalue(hProperty, kEvent_type_skill_action_key);
    if (action && !strcmp(action, "listen"))
    {
        metrics_skill_invoke("speechRecognizer", action);
        pSC->PendingListen = 1;
        return 0;
    }

    LogError("Unhandled action '%s'", action);
    return -1;
}

// Zhou: called by Text_ResponseHandler that is a depreated V1 handler for partial results. Probably not needed anymore.
void OnSpeechPartialResult(
    SPEECH_CONTEXT* pSC,
    const char*     pszUtf8PartialText)
{
    OnRecognitionStarted(pSC);

    // extend the turn timeout when speech is happening.git 
    UpdateTurnTimeout(pSC);

    if (pSC->mCallbacks  && pSC->mCallbacks->OnSpeech)
    {
        pSC->mCallbacks->OnSpeech(
            pSC, 
            pSC->mContext, 
            pszUtf8PartialText, 
            SPEECH_PHRASE_STATE_PARTIAL);
    }
}

// Deprecated V1 handler for partial results.  Remove once the service switched.
SPEECH_RESULT Text_ResponseHandler(
    void*           pContext,
    const char*     pszPath,
    uint8_t*        pBuffer,
    size_t          bufferSize,
    IOBUFFER*       pIoBuffer,
    PCONTENT_ASYNCCOMPLETE_CALLBACK pCB,
    void*           pAsyncContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    STRING_HANDLE   hText;

    (void)pszPath;
    (void)pIoBuffer;
    (void)pCB;
    (void)pAsyncContext;

    if (pSC->mCallbacks  && pSC->mCallbacks->OnSpeech)
    {
        hText = STRING_from_byte_array(pBuffer, bufferSize);
        if (NULL != hText)
        {
            OnSpeechPartialResult(pSC, STRING_c_str(hText));
            STRING_delete(hText);
        }
    }

    return 0;
}

// zhou: called from outside, to reset status?
void speech_cancel(
    CORTANA_HANDLE          hCortana, 
    enum speech_cancel_type cancelationType)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)hCortana;

    if ((cancelationType & SPEECH_CANCEL_TYPE_LISTENING) && 
        // ensure we're listening or about to enter the listening state.
        (pSC->PendingListen != 0 || pSC->fKWSListening == 0))
    {
        pSC->PendingListen = 0;
        // agent_queue_event(hCortana, CORTANA_EVENT_SPEECH_CANCEL_LISTENING);
    }

    if ((cancelationType & SPEECH_CANCEL_TYPE_SPEAKING) && 
        // ensure TTS is actively playing.
        (pSC->TTSCount != 0))
    {
        // TTSCount is reset through audio events, no need to reset here.
        // agent_queue_event(hCortana, CORTANA_EVENT_SPEECH_CANCEL_OUTPUT);
    }
}

// Zhou: callback used by speech_serilize
static int speech_serialize_internal(
    PROPERTYBAG_HANDLE   hProperty,
    void*                pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;
    propertybag_setnumbervalue(hProperty, kString_version, 1.1);
    propertybag_setbooleanvalue(hProperty, kString_audioincludeskeyword, pSC->KWTriggered);
    propertybag_setbooleanvalue(hProperty, "earconsetting" /*kEarconSetting*/, pSC->EarconSetting == EARCON_SETTING_ENABLED);
    if (pSC->KWTriggered)
    {
        propertybag_setnumbervalue(hProperty, kString_keywordconfidence, pSC->kw_confidence);
        propertybag_setnumbervalue(hProperty, kString_keywordstartoffset, pSC->kw_startoffset);
        propertybag_setnumbervalue(hProperty, kString_keywordduration, pSC->kw_duration);
    }
    metrics_audio_kws_triggered(pSC->KWTriggered);
    return 0;
}

// Zhou: called by skills.c to serilize SpeechContext into propertybag.
int speech_serialize(
    PROPERTYBAG_HANDLE  hProperty,
    void*               pContext)
{
    SPEECH_CONTEXT* pSC = (SPEECH_CONTEXT*)pContext;

    if (pSC->SpeechTurn)
    {
        return propertybag_serialize_object(
            hProperty,
            "speech",
            speech_serialize_internal,
            pContext);
    }

    return 0;
}
