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
#include "audio_sys.h"
#include "audio_recorder.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"


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

typedef struct AUDIO_SYS_DATA_TAG
{
    ON_AUDIOERROR_CALLBACK        errorCallback;
    ON_AUDIOOUTPUT_STATE_CALLBACK outputStateCallback;
    ON_AUDIOINPUT_STATE_CALLBACK  inputStateCallback;
    AUDIOINPUT_WRITE              audioWriteCallback;
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

    AudioQueue *freeBufferQueue;  // Owner of the queue
    AudioQueue *receiveBufferQueue;   // Owner of the queue

    sample_buf *buffers;
    uint32_t bufferCount;
    uint32_t frameCount;
} AUDIO_SYS_DATA;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef bool(*audio_recorder_engine_service_function)(void *ctx, uint32_t msg, void *data);

static void create_sl_engine(AUDIO_SYS_DATA *audioData);
static bool create_audio_recorder(AUDIO_SYS_DATA *engine, audio_recorder_engine_service_function audio_recorder_engine_service, void *ctx);
static void delete_audio_recorder(AUDIO_SYS_DATA *engine);
static bool audio_recorder_engine_service(void *ctx, uint32_t msg, void *data);
static int open_wave_data(AUDIO_SYS_DATA* audioData, snd_pcm_stream_t streamType);
static int output_async_read(void* userContext, uint8_t* pBuffer, size_t size);
static int output_async(void *p);
static int output_async_read(void* userContext, uint8_t* pBuffer, size_t size);
static int output_write_async(void *p);

static AUDIO_RESULT write_audio_stream(AUDIO_SYS_DATA*audioData, const AUDIO_WAVEFORMAT*outputWaveFmt, AUDIOINPUT_WRITE readCallback, AUDIOCOMPLETE_CALLBACK completedCallback, AUDIO_BUFFERUNDERRUN_CALLBACK bufferUnderrunCallback, void* userContext);

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

AUDIO_SYS_HANDLE audio_create()
{
    AUDIO_SYS_DATA* result;

    result = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    if (result != nullptr)
    {
        memset(result, 0, sizeof(AUDIO_SYS_DATA));
        result->channels = AUDIO_CHANNELS_MONO;
        result->sampleRate = AUDIO_SAMPLE_RATE;
        result->bitsPerSample = AUDIO_BITS;
        result->waveDataDirty = true;
        result->inputFrameCount = INPUT_FRAME_COUNT;
        result->currentOutputState = AUDIO_STATE_STOPPED;
        result->currentInputState = AUDIO_STATE_STOPPED;
        result->outputCanceledLock = Lock_Init();
        result->audioBufferLock = Lock_Init();
        sem_init(&result->audioFramesAvailable, 0, 0);
    }
    return result;
}

void audio_destroy(AUDIO_SYS_HANDLE handle)
{
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        if (audioData->pcmHandle != nullptr)
        {
            delete_audio_recorder(audioData);
        }

        if (audioData->deviceName)
        {
            STRING_delete(audioData->deviceName);
        }

        Lock_Deinit(audioData->outputCanceledLock);
        Lock_Deinit(audioData->audioBufferLock);
        sem_destroy(&audioData->audioFramesAvailable);

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
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
    AUDIO_RESULT result = AUDIO_RESULT_INVALID_ARG;
    AUDIO_STATE currentOutputState;

    if (handle != nullptr)
    {
        do
        {
            // copy out the thread to cancel any outstanding work.
            Lock(audioData->outputCanceledLock);

            audioData->outputCanceled = true;
            currentOutputState = audioData->currentOutputState;

            Unlock(audioData->outputCanceledLock);

            // set result only once.
            if (result == AUDIO_RESULT_INVALID_ARG)
            {
                if (currentOutputState != AUDIO_STATE_RUNNING)
                {
                    result = AUDIO_RESULT_INVALID_STATE;
                }
                else
                {
                    result = AUDIO_RESULT_OK;
                }
            }

            // continue until the state has been reset.
        } while (currentOutputState == AUDIO_STATE_RUNNING);
    }
    return result;
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
    AUDIO_RESULT ret = AUDIO_RESULT_ERROR;
    struct _ASYNCAUDIO *async;

    if (!handle || !format || !readCallback || !completedCallback)
    {
        return ret;
    }

    if (handle->waveDataDirty)
    {
        if (open_wave_data(handle, SND_PCM_STREAM_PLAYBACK) != 0)
        {
            LogError("open_wave_data");
            return ret;
        }
    }

    async = (struct _ASYNCAUDIO *)malloc(sizeof(struct _ASYNCAUDIO));
    if (!async)
    {
        return ret;
    }

    async->audioData = (AUDIO_SYS_DATA*)handle;
    async->fp = nullptr;
    async->format = *format;
    async->readCallback = readCallback;
    async->completedCallback = completedCallback;
    async->bufferUnderrunCallback = bufferUnderrunCallback;
    async->userContext = userContext;
    ret = (AUDIO_RESULT)ThreadAPI_Create(&async->outputThread, output_write_async, async);

    if (ret)
    {
        free(async);
    }

    return ret;
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

    uint32_t bufSize = audioData->fastPathFramesPerBuffer * audioData->channels * audioData->bitsPerSample;
    bufSize = (bufSize + 7) >> 3;
    audioData->bufferCount = BUF_COUNT;

    audioData->buffers = allocateSampleBufs(audioData->bufferCount, bufSize);
    assert(audioData->buffers);

    audioData->freeBufferQueue = new AudioQueue(audioData->bufferCount);
    audioData->receiveBufferQueue = new AudioQueue(audioData->bufferCount);
    assert(audioData->freeBufferQueue && audioData->receiveBufferQueue);

    for (uint32_t i = 0; i < audioData->bufferCount; i++)
    {
        audioData->freeBufferQueue->push(&audioData->buffers[i]);
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

    engine->pcmHandle = new AudioRecorder(&sampleFormat, engine->slEngineInterface);
    if (!engine->pcmHandle)
    {
        return false;
    }

    engine->pcmHandle->SetBufQueues(engine->freeBufferQueue, engine->receiveBufferQueue);
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
        sample_buf *buf = static_cast<sample_buf *>(data);

        if (audioData->currentInputState == AUDIO_STATE_RUNNING) {
            if (audioData->audioWriteCallback)
            {
                audioData->audioWriteCallback(
                    audioData->userWriteContext,
                    (uint8_t*)buf->buf_,
                    (size_t)buf->size_);
            }

            // Note: we would not need the buffer queue since we are copying
            //       the data anyway.
            // free any pending buffer
            sample_buf *freeBuf;
            while (audioData->receiveBufferQueue &&
                audioData->receiveBufferQueue->front(&freeBuf))
            {
                audioData->receiveBufferQueue->pop();
                audioData->freeBufferQueue->push(freeBuf);
            }
        }
        break;
    }

    default:
        assert(false);
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

static int output_async_read(void* userContext, uint8_t* pBuffer, size_t size)
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

static int output_write_async(void *p)
{
    struct _ASYNCAUDIO *async = (struct _ASYNCAUDIO *)p;
    write_audio_stream(
        async->audioData,
        &async->format,
        async->readCallback,
        async->completedCallback,
        async->bufferUnderrunCallback,
        async->userContext);
    ThreadAPI_Join(async->outputThread, nullptr);
    free(async);
    return 0;
}
