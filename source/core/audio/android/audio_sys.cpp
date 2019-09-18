//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sched.h>
#include <linux/sched.h>
#include <assert.h>
#include "spx_namespace.h"
#include "audio_sys.h"
#include "audio_recorder.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "data_buffer.h"

#define MAGIC_TAG_RIFF      0x46464952
#define MAGIC_TAG_WAVE      0x45564157
#define MAGIC_TAG_data      0x61746164
#define MAGIC_TAG_fmt       0x20746d66

#define AUDIO_CHANNELS_MONO     1
#define AUDIO_SAMPLE_RATE       16000
#define AUDIO_SAMPLE_FRAMES     ((AUDIO_SAMPLE_RATE) / 10)
#define AUDIO_BITS              16

#define HISTORY_BUFFER_SIZE         32
#define FAST_LOOP_MAX_COUNT         10
#define FAST_LOOP_THRESHOLD_MS      10
#define OUTPUT_FRAME_COUNT    (int) 768
#define INPUT_FRAME_COUNT     (int) 232
#define CAPTURE_BUFFER_SIZE (AUDIO_SAMPLE_RATE * 5)

#define SND_PCM_STREAM_CAPTURE  1
#define SND_PCM_STREAM_PLAYBACK 2
typedef int snd_pcm_stream_t;

typedef std::shared_ptr<std::vector<uint8_t>> Buffer;

#define N_PLAYBACK_BUFFERS 3

using Microsoft::CognitiveServices::Speech::Impl::DataBuffer;
typedef struct AUDIO_SYS_DATA_TAG
{
    ON_AUDIOERROR_CALLBACK        errorCallback;
    ON_AUDIOOUTPUT_STATE_CALLBACK outputStateCallback;
    ON_AUDIOINPUT_STATE_CALLBACK  inputStateCallback;
    AUDIOINPUT_WRITE              audioWriteCallback;
    AUDIOCOMPLETE_CALLBACK        audioCompleteCallback;
    void* userWriteContext;
    void* userOutputContext;
    void* userInputContext;
    void* userErrorContext;

    bool waveDataDirty;
    AUDIO_STATE currentOutputState;
    AUDIO_STATE currentInputState;

    // current audio capture block for processing
    int16_t*       audioSamples;
    sem_t          audioFramesAvailable;
    LOCK_HANDLE    audioBufferLock;

    bool outputCanceled;
    LOCK_HANDLE outputCanceledLock;

    STRING_HANDLE deviceName;
    uint16_t inputFrameCount;

    ////////////////////////

    AudioRecorder *pcmHandle;

    uint16_t channels;
    uint32_t sampleRate;
    uint16_t bitsPerSample;

    SLmilliHertz fastPathSampleRate;
    uint32_t fastPathFramesPerBuffer;

    SLObjectItf slEngineObject;
    SLEngineItf slEngineInterface;

    SLObjectItf outputMixObjectItf;
    SLObjectItf playerObjectItf;
    SLPlayItf playItf;
    SLAndroidSimpleBufferQueueItf playBufferQueueItf;
    // keep the enqueued buffer, so we can reuse them instead of re-allocating new buffers.
    Buffer bufferStack[N_PLAYBACK_BUFFERS];
    int curretBufferStackIndex;
    std::atomic<int> nEnqueuedBuffers;

    // used when stopping/desposing SLAndroidSimpleBufferQueue
    LOCK_HANDLE stopLock;

} AUDIO_SYS_DATA;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef bool(*audio_recorder_engine_service_function)(void *ctx, uint32_t msg, void *data);

static void create_sl_engine(AUDIO_SYS_DATA *audioData);
static void delete_sl_engine(AUDIO_SYS_DATA *audioData);
static bool create_audio_recorder(AUDIO_SYS_DATA *engine, audio_recorder_engine_service_function audio_recorder_engine_service, void *ctx);
static void delete_audio_recorder(AUDIO_SYS_DATA *engine);
static bool audio_recorder_engine_service(void *ctx, uint32_t msg, void *data);
static int open_wave_data(AUDIO_SYS_DATA* audioData, snd_pcm_stream_t streamType);
static int output_async_read(void* userContext, uint8_t* pBuffer, uint32_t size);
static int output_async(void *p);

static void on_audio_buffer_played_callback(SLAndroidSimpleBufferQueueItf bufferQueue, void* pContext);

static AUDIO_RESULT write_audio_stream(AUDIO_SYS_DATA*audioData, const AUDIO_WAVEFORMAT* outputWaveFmt, AUDIOINPUT_WRITE readCallback, AUDIOCOMPLETE_CALLBACK completedCallback, AUDIO_BUFFERUNDERRUN_CALLBACK bufferUnderrunCallback, void* userContext);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ASYNCAUDIO
{
    AUDIO_SYS_DATA*        audioData;
    FILE *                 fp;
    AUDIO_WAVEFORMAT       format;
    AUDIOINPUT_WRITE       readCallback;
    AUDIOCOMPLETE_CALLBACK completedCallback;
    AUDIO_BUFFERUNDERRUN_CALLBACK bufferUnderrunCallback;
    void*                  userContext;
    uint32_t               dataChunk;
    THREAD_HANDLE          outputThread;
};

AUDIO_SYS_HANDLE audio_input_create_with_parameters(AUDIO_SETTINGS_HANDLE format)
{
    AUDIO_SYS_DATA* result;

    result = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    if (result != nullptr)
    {
        memset(result, 0, sizeof(AUDIO_SYS_DATA));
        result->channels = format->nChannels;
        result->sampleRate = format->nSamplesPerSec;
        result->bitsPerSample = format->wBitsPerSample;
        result->waveDataDirty = true;
        result->inputFrameCount = INPUT_FRAME_COUNT;
        result->currentOutputState = AUDIO_STATE_STOPPED;
        result->currentInputState = AUDIO_STATE_STOPPED;
        result->outputCanceledLock = Lock_Init();
        result->audioBufferLock = Lock_Init();
        sem_init(&result->audioFramesAvailable, 0, 0);

        audio_set_options(result, AUDIO_OPTION_DEVICENAME, STRING_c_str(format->hDeviceName));
    }

    return result;
}

AUDIO_SYS_HANDLE audio_output_create_with_parameters(AUDIO_SETTINGS_HANDLE format)
{
    SPX_DBG_TRACE_VERBOSE("setting up playback audioData");

    // In OpenSL ES for Android, PCM is the only data format you can use with buffer queues.
    // https://developer.android.com/ndk/guides/audio/opensl/opensl-for-android#pcm-data-format
    if (Microsoft::CognitiveServices::Speech::Impl::WAVE_FORMAT_PCM != format->wFormatTag)
    {
        LogError("Unsupported audio format for playing");
        return nullptr;
    }

    AUDIO_SYS_DATA* audioData;

    audioData = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    if (audioData != NULL)
    {
        auto setup_ok = true;
        memset(audioData, 0, sizeof(AUDIO_SYS_DATA));

        audioData->channels = format->nChannels;
        audioData->sampleRate = format->nSamplesPerSec;
        audioData->bitsPerSample = format->wBitsPerSample;

        audioData->currentOutputState = AUDIO_STATE_STOPPED;
        audioData->currentInputState = AUDIO_STATE_STOPPED;

        create_sl_engine(audioData);

        // create the output mix
        auto result = (*audioData->slEngineInterface)->CreateOutputMix(audioData->slEngineInterface, &audioData->outputMixObjectItf, 0, NULL, NULL);
        if (SL_RESULT_SUCCESS != result) {
            LogError("%s, creating output mix failed.", __FUNCTION__);
            setup_ok = false;
        }

        if (setup_ok) {
            // realize the output mix
            result = (*audioData->outputMixObjectItf)->Realize(audioData->outputMixObjectItf, SL_BOOLEAN_FALSE);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, realizing output mix failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            // configure audio source
            SLDataLocator_AndroidSimpleBufferQueue loc_bufq =
            {
                SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                N_PLAYBACK_BUFFERS + 1
            };

            SLAndroidDataFormat_PCM_EX format_pcm;
            memset(&format_pcm, 0, sizeof(format_pcm));
            format_pcm.formatType = SL_DATAFORMAT_PCM;
            // Only support 2 channels
            // For channelMask, refer to wilhelm/src/android/channels.c for details
            if (audioData->channels <= 1) {
                format_pcm.numChannels = 1;
                format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT;
            } else {
                format_pcm.numChannels = 2;
                format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            }
            format_pcm.sampleRate = audioData->sampleRate * 1000;  // in units of milliHz
            format_pcm.bitsPerSample = format_pcm.containerSize = static_cast<uint16_t>(audioData->bitsPerSample);
            format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
            SLDataSource audioSrc = {&loc_bufq, &format_pcm};

            // configure audio sink
            SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, audioData->outputMixObjectItf};
            SLDataSink audioSnk = {&loc_outmix, NULL};
            /*
            * create fast path audio player: SL_IID_BUFFERQUEUE and SL_IID_VOLUME
            * and other non-signal processing interfaces are ok.
            */
            SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
            SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
            result = (*audioData->slEngineInterface)->CreateAudioPlayer(
                audioData->slEngineInterface, &audioData->playerObjectItf, &audioSrc, &audioSnk,
                sizeof(ids) / sizeof(ids[0]), ids, req);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, creating audio player failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            // realize the player
            result = (*audioData->playerObjectItf)->Realize(audioData->playerObjectItf, SL_BOOLEAN_FALSE);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, realizing player failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            // get the play interface
            result = (*audioData->playerObjectItf)
                        ->GetInterface(audioData->playerObjectItf, SL_IID_PLAY, &audioData->playItf);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, getting play interface failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            // get the buffer queue interface
            result = (*audioData->playerObjectItf)
                        ->GetInterface(audioData->playerObjectItf, SL_IID_BUFFERQUEUE, &audioData->playBufferQueueItf);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, getting buffer queue interface failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            // register callback on the buffer queue
            result = (*audioData->playBufferQueueItf)
                        ->RegisterCallback(audioData->playBufferQueueItf, on_audio_buffer_played_callback, audioData);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, registering callback failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            result = (*audioData->playItf)->SetPlayState(audioData->playItf, SL_PLAYSTATE_STOPPED);
            if (SL_RESULT_SUCCESS != result) {
                LogError("%s, setting play state failed.", __FUNCTION__);
                setup_ok = false;
            }
        }

        if (setup_ok) {
            audioData->stopLock = Lock_Init();
        } else {
            free(audioData);
            audioData = nullptr;
        }
    }

    return audioData;
}

AUDIO_SYS_HANDLE audio_create_with_parameters(AUDIO_SETTINGS_HANDLE format)
{
    SPX_DBG_TRACE_VERBOSE("setting up AudioQueue");

     switch (format->eDataFlow)
        {
        case AUDIO_CAPTURE:
            return audio_input_create_with_parameters(format);
            break;

        case AUDIO_RENDER:
            return audio_output_create_with_parameters(format);
            break;

        default:
            LogError("Unknown audio data flow");
            break;
        }
}

void audio_destroy(AUDIO_SYS_HANDLE handle)
{
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

        // check if the audio is stopped
        if (AUDIO_STATE_STOPPED != audioData->currentOutputState)
        {
            LOGW("audio destroyed before stopping.");
            audio_output_stop(handle);
        }

        if (audioData->stopLock != nullptr)
        {
            Lock(audioData->stopLock);
        }

        if (audioData->pcmHandle != nullptr)
        {
            delete_audio_recorder(audioData);
        }

        if (audioData->deviceName)
        {
            STRING_delete(audioData->deviceName);
        }

        if (audioData->outputCanceledLock) 
        {
            Lock_Deinit(audioData->outputCanceledLock);
        }

        if (audioData->audioBufferLock) 
        {
            Lock_Deinit(audioData->audioBufferLock);
        }

        sem_destroy(&audioData->audioFramesAvailable);

        // destroy buffer queue audio player object, and invalidate all associated interfaces
        if (audioData->playerObjectItf != nullptr) {
            (*audioData->playerObjectItf)->Destroy(audioData->playerObjectItf);
            audioData->playerObjectItf = nullptr;
            audioData->playItf = nullptr;
            audioData->playBufferQueueItf = nullptr;
        }

        // Consume all non-completed audio buffers
        for (auto i = 0; i < N_PLAYBACK_BUFFERS; ++i)
        {
            audioData->bufferStack[i].reset();
        }

        // destroy output mix object, and invalidate all associated interfaces
        if (audioData->outputMixObjectItf) {
            (*audioData->outputMixObjectItf)->Destroy(audioData->outputMixObjectItf);
            audioData->outputMixObjectItf = nullptr;
        }

        if (audioData->stopLock != nullptr)
        {
            Unlock(audioData->stopLock);
            Lock_Deinit(audioData->stopLock);
        }

        delete_sl_engine(audioData);
        free(audioData);
    }
}

AUDIO_RESULT audio_setcallbacks(
    AUDIO_SYS_HANDLE              handle,
    ON_AUDIOOUTPUT_STATE_CALLBACK output_cb,
    void*                         output_ctx,
    ON_AUDIOINPUT_STATE_CALLBACK  input_cb,
    void*                         input_ctx,
    AUDIOINPUT_WRITE              audioWriteCallback,
    void*                         audio_write_ctx,
    ON_AUDIOERROR_CALLBACK        errorCallback,
    void*                         error_ctx)
{
    AUDIO_RESULT result;

    if (handle != nullptr && audioWriteCallback != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        audioData->errorCallback = errorCallback;
        audioData->userErrorContext = error_ctx;
        audioData->inputStateCallback = input_cb;
        audioData->userInputContext = input_ctx;
        audioData->outputStateCallback = output_cb;
        audioData->userOutputContext = output_ctx;
        audioData->audioWriteCallback = audioWriteCallback;
        audioData->userWriteContext = audio_write_ctx;
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }

    return result;
}

AUDIO_RESULT audio_output_pause(AUDIO_SYS_HANDLE handle)
{
    (void)handle;

    AUDIO_RESULT result = AUDIO_RESULT_INVALID_ARG;
    return result;
}

AUDIO_RESULT audio_output_restart(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result;
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        if (audioData->currentOutputState != AUDIO_STATE_RUNNING)
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
        else
        {
            result = AUDIO_RESULT_OK;
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_output_stop(AUDIO_SYS_HANDLE handle)
{
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        if (AUDIO_STATE_STOPPED == audioData->currentOutputState) {
            SPX_DBG_TRACE_VERBOSE("audio playback already stopped.");
            return AUDIO_RESULT_OK;
        }
        SLuint32 state;
        auto result = (*audioData->playItf)->GetPlayState(audioData->playItf, &state);
        if (SL_RESULT_SUCCESS != result) {
            LogError("%s, getting play state failed.", __FUNCTION__);
            return AUDIO_RESULT_ERROR;
        }
        if (state == SL_PLAYSTATE_STOPPED) {
            LOGW("Audio state in AUDIO_SYS_HANDLE is STOPPED, while the audio player is not.");
            audioData->currentOutputState = AUDIO_STATE_STOPPED;
            return AUDIO_RESULT_OK;
        }
        Lock(audioData->stopLock);

        auto audioResult = AUDIO_RESULT_OK;
        result = (*audioData->playItf)->SetPlayState(audioData->playItf, SL_PLAYSTATE_STOPPED);
        if (SL_RESULT_SUCCESS != result) {
            LogError("%s, setting play state failed.", __FUNCTION__);
            audioResult = AUDIO_RESULT_ERROR;
        } else {
            (*audioData->playBufferQueueItf)->Clear(audioData->playBufferQueueItf);
            audioData->currentOutputState = AUDIO_STATE_STOPPED;
        }

        Unlock(audioData->stopLock);
        return audioResult;
    }
    else
    {
        return AUDIO_RESULT_INVALID_ARG;
    }
}

STRING_HANDLE audio_output_get_name(AUDIO_SYS_HANDLE handle)
{
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

    if (handle)
    {
        if (audioData->deviceName)
        {
            return STRING_clone(audioData->deviceName);
        }

        if (audioData->pcmHandle)
        {
            return STRING_construct("microphone");
        }
    }

    return nullptr;
}

AUDIO_RESULT  audio_output_startasync(
    AUDIO_SYS_HANDLE handle,
    const AUDIO_WAVEFORMAT* format,
    AUDIOINPUT_WRITE readCallback,
    AUDIOCOMPLETE_CALLBACK completedCallback,
    AUDIO_BUFFERUNDERRUN_CALLBACK bufferUnderrunCallback,
    void* userContext)
{
    if (!handle || !format || !readCallback || !completedCallback)
    {
        return AUDIO_RESULT_INVALID_ARG;
    }

    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

    if (AUDIO_STATE_RUNNING == audioData->currentOutputState || AUDIO_STATE_STARTING == audioData->currentOutputState)
    {
        SPX_DBG_TRACE_VERBOSE("%s: the audio is starting or running, do nothing.", __FUNCTION__);
        return AUDIO_RESULT_OK;
    }
    if (AUDIO_STATE_STOPPED != audioData->currentOutputState)
    {
        return AUDIO_RESULT_INVALID_STATE;
    }

    audioData->audioWriteCallback = readCallback;
    audioData->userWriteContext = userContext;
    audioData->audioCompleteCallback = completedCallback;
    UNUSED(bufferUnderrunCallback);

    auto result = (*audioData->playItf)->SetPlayState(audioData->playItf, SL_PLAYSTATE_STOPPED);
    if (SL_RESULT_SUCCESS != result) {
        LogError("%s, setting play state failed.", __FUNCTION__);
        return AUDIO_RESULT_ERROR;
    }

    audioData->currentOutputState = AUDIO_STATE_STARTING;
    auto audioResult = AUDIO_RESULT_OK;

    // Allocate and enqueue buffers
    int bufferByteSize = audioData->sampleRate * audioData->bitsPerSample * audioData->channels / 8 / 20; // 50ms buffer length
    audioData->nEnqueuedBuffers = 0;
    for (auto bufferIndex = 0; bufferIndex < N_PLAYBACK_BUFFERS; ++bufferIndex)
    {
        if (audioData->bufferStack[bufferIndex] == nullptr)
        {
            audioData->bufferStack[bufferIndex] = std::make_shared<std::vector<uint8_t>>(bufferByteSize, 0);
        }
        auto buffer = audioData->bufferStack[bufferIndex];
        buffer->resize(bufferByteSize);

        // TODO: update this when CanRead method in PullAudioOutputStream is available
        int write_size = audioData->audioWriteCallback(audioData->userWriteContext, reinterpret_cast<uint8_t*>(buffer->data()), 512);
        if (!write_size) {
            LogError("The synthesized audio is too short to play.");
        }
        audioData->bufferStack[bufferIndex] = buffer;
        result = (*audioData->playBufferQueueItf)->Enqueue(audioData->playBufferQueueItf, buffer->data(), write_size);
        if (SL_RESULT_SUCCESS != result) {
            LogError("%s, enqueue buffer failed.", __FUNCTION__);
            audioResult = AUDIO_RESULT_ERROR;
            break;
        }
        ++audioData->nEnqueuedBuffers;
    }

    if (AUDIO_RESULT_OK == audioResult) {
        result = (*audioData->playItf)->SetPlayState(audioData->playItf, SL_PLAYSTATE_PLAYING);
        if (SL_RESULT_SUCCESS != result) {
            LogError("%s, setting play state failed.", __FUNCTION__);
            audioResult = AUDIO_RESULT_ERROR;
        }
    }

    if (AUDIO_RESULT_OK == audioResult) {
        audioData->currentOutputState = AUDIO_STATE_RUNNING;
    } else {
        audioData->currentOutputState = AUDIO_STATE_STOPPED;
    }

    return audioResult;
}

AUDIO_RESULT audio_input_start(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result = AUDIO_RESULT_OK;

    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        if (audioData->currentInputState == AUDIO_STATE_RUNNING)
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
        else
        {
            if (open_wave_data(audioData, SND_PCM_STREAM_CAPTURE) != 0)
            {
                result = AUDIO_RESULT_ERROR;
            }
            else
            {
                audioData->currentInputState = AUDIO_STATE_STARTING;
                if (audioData->inputStateCallback)
                {
                    audioData->inputStateCallback(audioData->userInputContext, audioData->currentInputState);
                }

                audioData->currentInputState = AUDIO_STATE_RUNNING;

                // start the audio recorder
                audioData->pcmHandle->Start();

                if (audioData->inputStateCallback)
                {
                    audioData->inputStateCallback(audioData->userInputContext, AUDIO_STATE_RUNNING);
                }
            }
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_input_stop(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

    if (handle != nullptr)
    {
        if (audioData->currentInputState != AUDIO_STATE_RUNNING)
        {
            return AUDIO_RESULT_INVALID_STATE;
        }

        // stop the audio recorder
        audioData->pcmHandle->Stop();

        audioData->currentInputState = AUDIO_STATE_STOPPED;

        if (audioData->inputStateCallback)
        {
            audioData->inputStateCallback(audioData->userInputContext, AUDIO_STATE_STOPPED);
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_set_options(AUDIO_SYS_HANDLE handle, const char* optionName, const void* value)
{
    AUDIO_RESULT result = AUDIO_RESULT_INVALID_ARG;
    if (handle != nullptr && optionName != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

        if (strcmp("channels", optionName) == 0)
        {
            uint16_t channel = (uint16_t)*((int*)value);
            if (channel == 1 || channel == 2)
            {
                audioData->channels = channel;
                audioData->waveDataDirty = true;
                result = AUDIO_RESULT_OK;
            }
        }
        else if (strcmp("bits_per_sample", optionName) == 0)
        {
            uint16_t bitPerSample = (uint16_t)*((int*)value);
            if (bitPerSample == 8 || bitPerSample == 16)
            {
                audioData->bitsPerSample = bitPerSample;
                audioData->waveDataDirty = true;
                result = AUDIO_RESULT_OK;
            }
        }
        else if (strcmp("sample_rate", optionName) == 0)
        {
            uint32_t sampleRate = *((uint32_t*)value);
            if (sampleRate == 11025 || sampleRate == 16000 || sampleRate == 22050 || sampleRate == 44100 || sampleRate == 96000)
            {
                audioData->sampleRate = sampleRate;
                audioData->waveDataDirty = true;
                result = AUDIO_RESULT_OK;
            }
        }
        else if (strcmp(AUDIO_OPTION_DEVICENAME, optionName) == 0)
        {
            if (!audioData->deviceName)
            {
                audioData->deviceName = STRING_construct((char*)value);
            }
            else
            {
                STRING_copy(audioData->deviceName, (char*)value);
            }

            if (audioData->deviceName)
            {
                result = AUDIO_RESULT_OK;
            }
        }
        else if (strcmp("write_cb", optionName) == 0)
        {
            AUDIOINPUT_WRITE write_cb = (AUDIOINPUT_WRITE)value;

            if (nullptr != write_cb)
            {
                audioData->audioWriteCallback = write_cb;
                result = AUDIO_RESULT_OK;
            }
        }
    }
    return result;
}

AUDIO_RESULT audio_playwavfile(AUDIO_SYS_HANDLE handle, const char* audioFile)
{
    AUDIO_RESULT ret = AUDIO_RESULT_ERROR;
    struct _ASYNCAUDIO *async;

    if (!handle)
    {
        return ret;
    }

    async = (struct _ASYNCAUDIO *)malloc(sizeof(struct _ASYNCAUDIO));
    if (!async)
    {
        return ret;
    }

    async->audioData = (AUDIO_SYS_DATA*)handle;
    async->dataChunk = 0;
    async->fp = fopen(audioFile, "rb");
    if (!async->fp)
    {
        LogError("%s not found", audioFile);
        goto Exit;
    }

    ret = (AUDIO_RESULT)ThreadAPI_Create(&async->outputThread, output_async, async);

Exit:
    if (ret)
    {
        free(async);
    }

    return ret;
}

AUDIO_RESULT audio_output_set_volume(AUDIO_SYS_HANDLE handle, long volume)
{
    (void)handle;
    (void)volume;

    AUDIO_RESULT result = AUDIO_RESULT_ERROR;
    return result;
}

AUDIO_RESULT audio_output_get_volume(AUDIO_SYS_HANDLE handle, long* volume)
{
    (void)handle;
    (void)volume;

    AUDIO_RESULT result = AUDIO_RESULT_ERROR;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void create_sl_engine(AUDIO_SYS_DATA *audioData)
{
    SLresult result;

    audioData->fastPathSampleRate = static_cast<SLmilliHertz>(AUDIO_SAMPLE_RATE) * 1000;
    audioData->fastPathFramesPerBuffer = static_cast<uint32_t>(AUDIO_SAMPLE_FRAMES); // 10 frames per seocond
    audioData->channels = AUDIO_SAMPLE_CHANNELS;
    audioData->bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;

    result = slCreateEngine(&audioData->slEngineObject, 0, nullptr, 0, nullptr, nullptr);
    SLASSERT(result);

    result = (*audioData->slEngineObject)->Realize(audioData->slEngineObject, SL_BOOLEAN_FALSE);
    SLASSERT(result);

    result = (*audioData->slEngineObject)->GetInterface(audioData->slEngineObject, SL_IID_ENGINE, &audioData->slEngineInterface);
    SLASSERT(result);
}

static void delete_sl_engine(AUDIO_SYS_DATA *engine)
{

    if (engine->slEngineObject != NULL)
    {
        (*(engine->slEngineObject))->Destroy(engine->slEngineObject);
        engine->slEngineObject = NULL;
        engine->slEngineInterface = NULL;
    }
}

static bool create_audio_recorder(AUDIO_SYS_DATA *engine, audio_recorder_engine_service_function audio_recorder_engine_service, void *ctx)
{
    SampleFormat sampleFormat;
    memset(&sampleFormat, 0, sizeof(sampleFormat));
    sampleFormat.pcmFormat_ = static_cast<uint16_t>(engine->bitsPerSample);

    // SampleFormat.representation_ = SL_ANDROID_PCM_REPRESENTATION_SIGNED_INT;
    sampleFormat.channels_ = engine->channels;
    sampleFormat.sampleRate_ = engine->fastPathSampleRate;
    sampleFormat.framesPerBuf_ = engine->fastPathFramesPerBuffer;

    const char *deviceName = STRING_c_str(engine->deviceName);

    engine->pcmHandle = new AudioRecorder(deviceName ? deviceName  : "", &sampleFormat, engine->slEngineObject, engine->slEngineInterface);
    if (!engine->pcmHandle)
    {
        return false;
    }

    engine->pcmHandle->RegisterCallback(audio_recorder_engine_service, ctx);
    return true;
}

static void delete_audio_recorder(AUDIO_SYS_DATA *engine)
{
    if (engine->pcmHandle) delete engine->pcmHandle;
    engine->pcmHandle = nullptr;
}

static bool audio_recorder_engine_service(void *ctx, uint32_t msg, void *data)
{
    AUDIO_SYS_DATA *audioData = (AUDIO_SYS_DATA *)ctx;
    if (!audioData) return false;

    switch (msg) {
    case ENGINE_SERVICE_MSG_RETRIEVE_DUMP_BUFS:
    {
        if (data)
        {
            *(static_cast<uint32_t *>(data)) = 0;
        }
        break;
    }

    case ENGINE_SERVICE_MSG_RECORDED_AUDIO_AVAILABLE:
    {
        DataBuffer* buf = static_cast<DataBuffer*>(data);

        if (audioData->currentInputState == AUDIO_STATE_RUNNING) {
            if (audioData->audioWriteCallback)
            {
                audioData->audioWriteCallback(
                    audioData->userWriteContext,
                    (uint8_t*)buf->data,
                    (size_t)buf->size);
            }
        }
        break;
    }

    default:
        assert(false); // debugging aid.
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static AUDIO_RESULT write_audio_stream(
    AUDIO_SYS_DATA*                 audioData,
    const AUDIO_WAVEFORMAT*         outputWaveFmt,
    AUDIOINPUT_WRITE                readCallback,
    AUDIOCOMPLETE_CALLBACK          completedCallback,
    AUDIO_BUFFERUNDERRUN_CALLBACK   bufferUnderrunCallback,
    void*                           userContext)
{
    (void)readCallback;
    (void)bufferUnderrunCallback;
    (void)outputWaveFmt;

    AUDIO_RESULT result = AUDIO_RESULT_ERROR;

    audioData->currentOutputState = AUDIO_STATE_STOPPED;

    // we must always call the completion callback.
    if (completedCallback)
    {
        completedCallback(userContext);
    }

    if (audioData->outputStateCallback != nullptr)
    {
        audioData->outputStateCallback(audioData->userOutputContext, audioData->currentOutputState);
    }

    return result;
}

static int open_wave_data(AUDIO_SYS_DATA* audioData, snd_pcm_stream_t streamType)
{
    int result = 0;

    switch (streamType)
    {
    case SND_PCM_STREAM_CAPTURE:
        if (!audioData->pcmHandle)
        {
            create_sl_engine(audioData);

            if (false == create_audio_recorder(audioData, audio_recorder_engine_service, audioData))
            {
                result = __LINE__;
            }
        }
        break;

    case SND_PCM_STREAM_PLAYBACK:
        LogError("SND_PCM_STREAM_PLAYBACK not supported yet.", __FUNCTION__);
        result = __LINE__;
        break;
    }

    return result;
}

static int output_async(void *p)
{
    struct _ASYNCAUDIO *async = (struct _ASYNCAUDIO *)p;
    if (async != nullptr)
    {
        uint32_t magic;
        uint32_t chunkSize;
        AUDIO_WAVEFORMAT outputWaveFmt;

        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)async->audioData;
        if (audioData->currentOutputState == AUDIO_STATE_RUNNING)
        {
            goto Exit;
        }
        else
        {
            if (audioData->waveDataDirty)
            {
                if (open_wave_data(audioData, SND_PCM_STREAM_PLAYBACK) != 0)
                {
                    LogError("open_wave_data");
                    goto Exit;
                }
            }

            // read up to the format tag
            while (sizeof(magic) == fread(&magic, 1, sizeof(magic), async->fp))
            {
                switch (magic)
                {
                case MAGIC_TAG_RIFF: // RIFF
                    if (sizeof(chunkSize) != fread(&chunkSize, 1, sizeof(chunkSize), async->fp))
                    {
                        goto Exit;
                    }
                    // we don't care about the RIFF size
                    break;

                case MAGIC_TAG_WAVE: // WAVE no length
                    break;

                case MAGIC_TAG_fmt:  // fmt - WAVEFORMAT
                    if (sizeof(chunkSize) != fread(&chunkSize, 1, sizeof(chunkSize), async->fp))
                    {
                        goto Exit;
                    }
                    else if (chunkSize < sizeof(AUDIO_WAVEFORMAT))
                    {
                        goto Exit;
                    }
                    else if (sizeof(outputWaveFmt) != fread(&outputWaveFmt, 1, sizeof(outputWaveFmt), async->fp))
                    {
                        goto Exit;
                    }
                    chunkSize -= sizeof(AUDIO_WAVEFORMAT);
                    if (chunkSize)
                    {
                        fseek(async->fp, chunkSize, SEEK_CUR);
                    }
                    (void)write_audio_stream(audioData, &outputWaveFmt, output_async_read, nullptr, nullptr, async);
                    // we're done.  All processing has completed within the write_audio_stream call.
                    goto Exit;

                default:
                    if (sizeof(chunkSize) != fread(&chunkSize, 1, sizeof(chunkSize), async->fp))
                    {
                        goto Exit;
                    }
                    fseek(async->fp, chunkSize, SEEK_CUR);
                    break;
                }
            }
        }
    Exit:

        fclose(async->fp);
        ThreadAPI_Join(async->outputThread, nullptr);
        free(async);
    }

    return 0;
}

static int output_async_read(void* userContext, uint8_t* pBuffer, uint32_t size)
{
    struct _ASYNCAUDIO *async = (struct _ASYNCAUDIO *)userContext;
    uint32_t magic;
    uint32_t chunkSize;

    while (async->dataChunk == 0)
    {
        if ((sizeof(magic) != fread(&magic, 1, sizeof(magic), async->fp)) ||
            (sizeof(chunkSize) != fread(&chunkSize, 1, sizeof(chunkSize), async->fp)))
        {
            return -1;
        }

        switch (magic)
        {
        case MAGIC_TAG_data: // data
            async->dataChunk = chunkSize;
            break;

        default:
            fseek(async->fp, chunkSize, SEEK_CUR);
            break;
        }
    }

    if (size > async->dataChunk)
    {
        size = async->dataChunk;
    }

    if (size != fread(pBuffer, 1, size, async->fp))
    {
        return -1;
    }

    async->dataChunk -= size;

    return (int)size;
}

STRING_HANDLE get_input_device_nice_name(AUDIO_SYS_HANDLE handle)
{
    auto audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

    if (handle)
    {
        return STRING_clone(audioData->deviceName);
    }

    return nullptr;
}

static void on_audio_buffer_played_callback(SLAndroidSimpleBufferQueueItf bufferQueue, void* pContext)
{
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(pContext);

    Lock(audioData->stopLock);

    auto currentBuffer = audioData->bufferStack[audioData->curretBufferStackIndex];
    audioData->curretBufferStackIndex = (audioData->curretBufferStackIndex + 1) % N_PLAYBACK_BUFFERS;

    if (currentBuffer->size() > 0)
    {
        // call the read audio buffer callback
        int write_size = audioData->audioWriteCallback(audioData->userWriteContext, reinterpret_cast<uint8_t*>(currentBuffer->data()), currentBuffer->size());
        if (write_size > 0) {
            // Re-enqueue used buffer
            auto result = (*bufferQueue)->Enqueue(bufferQueue, currentBuffer->data(), write_size);
            if (SL_RESULT_SUCCESS != result) {
                audioData->currentOutputState = AUDIO_STATE_STOPPED;

                    LogError("%s, enqueue failed.", __FUNCTION__);

                    // call audio complete callback
                    audioData->audioCompleteCallback(audioData->userWriteContext);
            }
        }
        else {
            // all audio data is read.
            --audioData->nEnqueuedBuffers;
            if (0 == audioData->nEnqueuedBuffers) // all audio buffers are played
            {
                // stop the audio
                auto result = (*audioData->playItf)->SetPlayState(audioData->playItf, SL_PLAYSTATE_STOPPED);
                SLASSERT(result);
                audioData->currentOutputState = AUDIO_STATE_STOPPED;
                audioData->audioCompleteCallback(audioData->userWriteContext);
            }
        }
    }
    Unlock(audioData->stopLock);
}
