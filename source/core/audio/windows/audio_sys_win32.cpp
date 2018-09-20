// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include "audio_sys.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/buffer_.h"
#include <windows.h>
#include <mmsystem.h>

#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <mmstream.h>
#include <endpointvolume.h>
#include <spxdebug.h>
#include <windows/audio_sys_win_base.h>

// remove this when the SDK supports it.
#define AudioCategory_Speech (AUDIO_STREAM_CATEGORY)9


const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

typedef struct AUDIO_SYS_DATA_TAG
{
    //Audio Input Context
    IAudioClient*                   pAudioInputClient;
    HANDLE                          hBufferReady;
    HANDLE                          hCaptureThreadShouldExit;
    HANDLE                          hCaptureThread;
    WAVEFORMATEX                    audioInFormat;
    //bool                          waveDataDirty;

    //Audio Output Context
    IMMDevice*                      pAudioOutputDevice;
    HANDLE                          hRenderThreadShouldExit;
    HANDLE                          hRenderThreadDidExit;
    BOOL                            output_canceled;

    ON_AUDIOERROR_CALLBACK          error_cb;
    ON_AUDIOOUTPUT_STATE_CALLBACK   output_state_cb;
    ON_AUDIOINPUT_STATE_CALLBACK    input_state_cb;
    AUDIOINPUT_WRITE                audio_write_cb;
    void                            *user_write_ctx;
    void                            *user_outputctx;
    void                            *user_inputctx;
    void                            *user_errorctx;
    AUDIO_STATE                     current_output_state;
    AUDIO_STATE                     current_input_state;
    STRING_HANDLE                   hDeviceName;
    size_t                          inputFrameCnt;
} AUDIO_SYS_DATA;


typedef struct _RENDERCALL
{
    AUDIO_SYS_DATA* pData;
    BUFFER_HANDLE   hOutputBuffer;
    WAVEHDR         waveoutHdr;
    WAVEFORMATEX    audioOutFormat;
    AUDIOINPUT_WRITE pfnReadCallback;
    AUDIOCOMPLETE_CALLBACK pfnComplete;
    void* pContext;
} RENDERCALL_DATA;


//-------------------
// helpers
HRESULT audio_create_events(AUDIO_SYS_DATA * const audioData);
template <class Type>
struct Deleter
{
    void operator () (Type * p) const
    {
        p->Release();
    }
};

size_t LoadDataChunk(BYTE* pDestBuffer, size_t destBufferSize, BYTE* pSourceBuffer, size_t sourceBufferSize, DWORD& flags)
{
    flags = 0;

    if (sourceBufferSize == 0)
    {
        flags = AUDCLNT_BUFFERFLAGS_SILENT;
        return 0;
    }

    size_t numBytesWritten = sourceBufferSize;
    if (sourceBufferSize > destBufferSize)
    {
        numBytesWritten = destBufferSize;
    }

    memcpy(pDestBuffer, pSourceBuffer, numBytesWritten);
    memset(pDestBuffer + numBytesWritten, 0, destBufferSize - numBytesWritten);

    return numBytesWritten;
}

DWORD WINAPI PlayAudio(
    RENDERCALL_DATA* pRender
)
{
    HRESULT hr;
    IAudioClient *pAudioClient = nullptr;
    IAudioRenderClient *pRenderClient = nullptr;
    AUDIO_SYS_DATA* pAudio = pRender->pData;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    UINT32 written;
    BYTE *pData;
    DWORD flags = 0;
    size_t offset = 0;
    int ret;
    WORD nBlockAlign = pRender->audioOutFormat.nBlockAlign;
    HANDLE events[2];
    BOOL audioStarted = FALSE;

    events[0] = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
    events[1] = pAudio->hRenderThreadShouldExit;
    if (pAudio->pAudioOutputDevice == NULL)
    {
        LogInfo("WARNING: No audio device");
        goto Exit;
    }

    hr = pAudio->pAudioOutputDevice->Activate(IID_IAudioClient, CLSCTX_ALL,NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        REFTIMES_PER_SEC * 2,
        0,
        &pRender->audioOutFormat,
        NULL);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->SetEventHandle(events[0]);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->GetService(
        IID_IAudioRenderClient,
        (void**)&pRenderClient);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr);
    audioStarted = TRUE;

    // Render Audio Buffer
    offset = 0;

    while (!pAudio->output_canceled || WaitForMultipleObjects(2, events, FALSE, INFINITE) == WAIT_OBJECT_0)
    {
        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr);

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // HACK: Wait until at least 1600 frames are available since AsyncRead()
        // in transcode.c (called via pfnReadCallback below) doesn't currently
        // respect the size parameter passed to it, causing the buffer to
        // overflow.  See Bug #766541.
        // TODO: Remove when AyncRead() is fixed.
        if (numFramesAvailable < 1600)
        {
            Sleep(10);
            continue;
        }

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr);

        // Get next 1/2-second of data from the audio source.
        if (pRender->waveoutHdr.lpData)
        {
            ret = (int)LoadDataChunk(pData, numFramesAvailable * pRender->audioOutFormat.nBlockAlign, (BYTE*)pRender->waveoutHdr.lpData + offset, pRender->waveoutHdr.dwBufferLength - offset, flags);
            written = ret;
        }
        else
        {
            ret = pRender->pfnReadCallback(pRender->pContext, pData, numFramesAvailable * nBlockAlign);
        }

        if (ret > 0)
        {
            written = ret / nBlockAlign;
        }
        else
        {
            written = 0;
        }

        if (ret < 0)
        {
            flags = AUDCLNT_BUFFERFLAGS_SILENT;
        }
        else
        {
            offset += (size_t)ret;
        }

        hr = pRenderClient->ReleaseBuffer(written, flags);
        EXIT_ON_ERROR(hr);

        if (ret <= 0)
        {
            break;
        }
    }

    while (!pAudio->output_canceled || WaitForMultipleObjects(2, events, FALSE, INFINITE) == WAIT_OBJECT_0)
    {
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr);
        if (numFramesPadding == 0)
        {
            break;
        }
    }

Exit:
    pAudio->output_canceled = FALSE;
    if (audioStarted)
    {
        pAudioClient->Stop();  // Stop playing.
    }

    if (pRender->hOutputBuffer)
    {
        BUFFER_delete(pRender->hOutputBuffer);
    }

    pAudio->current_output_state = AUDIO_STATE_STOPPED;
    if (pRender->pfnComplete)
    {
        pRender->pfnComplete(pRender->pContext);
    }

    if (pAudio->output_state_cb)
    {
        pAudio->output_state_cb(pAudio->user_outputctx, AUDIO_STATE_STOPPED);
    }

    SAFE_CLOSE_HANDLE(events[0]);

    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pRenderClient);

    SetEvent(pAudio->hRenderThreadDidExit);

    return 0;
}

DWORD WINAPI renderThreadProc(
    _In_ LPVOID lpParameter
)
{
    RENDERCALL_DATA* pAudio = (RENDERCALL_DATA*)lpParameter;

    PlayAudio(pAudio);

    free(pAudio);

    return 0;
}

static AUDIO_RESULT write_audio_stream(
    AUDIO_SYS_DATA* audioData,
    const AUDIO_WAVEFORMAT *pfmt,
    const unsigned char* data,
    size_t length,
    AUDIOINPUT_WRITE pfnReadCallback,
    AUDIOCOMPLETE_CALLBACK pfnComplete,
    void* pContext)
{
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    RENDERCALL_DATA* pAudio;

    if (!pfmt)
    {
        return AUDIO_RESULT_ERROR;
    }

    pAudio = (RENDERCALL_DATA*)malloc(sizeof(RENDERCALL_DATA));
    if (NULL == pAudio)
    {
        return AUDIO_RESULT_ERROR;
    }

    memset(pAudio, 0, sizeof(RENDERCALL_DATA));

    pAudio->pfnReadCallback = pfnReadCallback;
    pAudio->pfnComplete = pfnComplete;
    pAudio->pContext = pContext;
    memcpy(&pAudio->audioOutFormat, pfmt, sizeof(AUDIO_WAVEFORMAT));
    pAudio->pData = audioData;
    if (data && length)
    {
        pAudio->hOutputBuffer = BUFFER_create(data, length);
        if (!pAudio->hOutputBuffer)
        {
            free(pAudio);
            return AUDIO_RESULT_ERROR;
        }

        memcpy(BUFFER_u_char(pAudio->hOutputBuffer), data, length);

        pAudio->waveoutHdr.lpData = (LPSTR)BUFFER_u_char(pAudio->hOutputBuffer);
        pAudio->waveoutHdr.dwBufferLength = (DWORD)length;
    }

    ResetEvent(audioData->hRenderThreadShouldExit);
    ResetEvent(audioData->hRenderThreadDidExit);

    //Notify Start Rendering
    audioData->current_output_state = AUDIO_STATE_RUNNING;
    if (audioData->output_state_cb)
    {
        audioData->output_state_cb(audioData->user_outputctx, AUDIO_STATE_RUNNING);
    }
    HANDLE hRenderThread = CreateThread(0, 0, renderThreadProc, pAudio, 0, nullptr);
    if (hRenderThread != nullptr)
    {
        CloseHandle(hRenderThread);  // we don't need the handle, release so it doesn't leak
    }

    return result;
}

AUDIO_SYS_HANDLE audio_create()
{
    AUDIO_SYS_DATA      *result = nullptr;
    HRESULT hr = E_FAIL;

    std::shared_ptr<IMMDeviceEnumerator> pEnumerator(nullptr);
    std::shared_ptr<IMMDevice> pDevice(nullptr);
    std::shared_ptr<IAudioClient2> pAudioClient2(nullptr);

    REFERENCE_TIME      hnsRequestedDuration = REFTIMES_PER_SEC;

    result = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    SPX_RETURN_HR_IF(nullptr, result == nullptr);

    memset(result, 0, sizeof(AUDIO_SYS_DATA));

    result->current_output_state = AUDIO_STATE_STOPPED;
    result->current_input_state = AUDIO_STATE_STOPPED;
    result->inputFrameCnt = 160;

    {
        IMMDeviceEnumerator * pEnumeratorRaw = nullptr;
        hr = CoCreateInstance(
            CLSID_MMDeviceEnumerator, NULL,
            CLSCTX_ALL, IID_IMMDeviceEnumerator,
            (void**)&pEnumeratorRaw);
        EXIT_ON_ERROR(hr);
        pEnumerator.reset(pEnumeratorRaw, Deleter<IMMDeviceEnumerator>());
    }

    //Initialize Audio Input, throw exception when no mic.
    {
        IMMDevice * pDeviceRaw = nullptr;
        HRESULT hrlocal = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDeviceRaw);
        LogError("hrlocal is 0x%x", hrlocal);
        EXIT_ON_ERROR_IF(E_FAIL, pDeviceRaw == nullptr || hrlocal != S_OK );
        pDevice.reset(pDeviceRaw, Deleter<IMMDevice>());
    }

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&result->pAudioInputClient);
    EXIT_ON_ERROR(hr);

    {
        IAudioClient2 * pAudioClient2Raw = nullptr;
        hr = result->pAudioInputClient->QueryInterface(&pAudioClient2Raw);
        if (SUCCEEDED(hr))
        {
            AudioClientProperties props = { 0 };

            props.cbSize = sizeof(AudioClientProperties);
            props.eCategory = AudioCategory_Speech;
            hr = pAudioClient2Raw->SetClientProperties(&props);
            pAudioClient2.reset(pAudioClient2Raw, Deleter<IAudioClient2>());
        }
    }
    result->audioInFormat.wFormatTag = AUDIO_FORMAT_PCM;
    result->audioInFormat.wBitsPerSample = AUDIO_BITS;
    result->audioInFormat.nChannels = AUDIO_CHANNELS_MONO;
    result->audioInFormat.nSamplesPerSec = AUDIO_SAMPLE_RATE;
    result->audioInFormat.nAvgBytesPerSec = AUDIO_BYTE_RATE;
    result->audioInFormat.nBlockAlign = AUDIO_BLOCK_ALIGN;
    result->audioInFormat.cbSize = 0;

    hr = result->pAudioInputClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
        hnsRequestedDuration,
        0,
        &result->audioInFormat,
        nullptr);
    EXIT_ON_ERROR(hr);


    //Initialize Audio Output, ignoring failures in order to support systems that do not have speakers hooked up
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &result->pAudioOutputDevice);
    if (FAILED(hr))
    {
        LogError("can't open default output device hr = 0x%x", hr);
    }

    hr = audio_create_events(result);

Exit:
    if (FAILED(hr))
    {
        SAFE_RELEASE(result->pAudioInputClient);
        SAFE_RELEASE(result->pAudioOutputDevice);

        if (result)
        {
            free(result);
            result = nullptr;
        }
    }
    return result;
}

HRESULT audio_create_events(AUDIO_SYS_DATA * const audioData)
{
    HRESULT hr = S_OK;
    if (audioData == nullptr)
    {
        return AUDIO_RESULT_INVALID_ARG;
    }

    audioData->hBufferReady = CreateEvent(0, FALSE, FALSE, nullptr);
    EXIT_ON_ERROR_IF(E_FAIL, nullptr == audioData->hBufferReady);

    audioData->hCaptureThreadShouldExit = CreateEvent(0, FALSE, FALSE, nullptr);
    EXIT_ON_ERROR_IF(E_FAIL, nullptr == audioData->hCaptureThreadShouldExit);

    audioData->hRenderThreadShouldExit = CreateEvent(0, FALSE, FALSE, nullptr);
    EXIT_ON_ERROR_IF(E_FAIL, nullptr == audioData->hRenderThreadShouldExit);

    // n.b. starts signaled so force_render_thread_to_exit_and_wait will work even if audio was never played
    audioData->hRenderThreadDidExit = CreateEvent(0, FALSE, TRUE, nullptr);
    EXIT_ON_ERROR_IF(E_FAIL, NULL == audioData->hRenderThreadDidExit);

    hr = audioData->pAudioInputClient->SetEventHandle(audioData->hBufferReady);    

Exit:
    if (FAILED(hr))
    {
        SAFE_CLOSE_HANDLE(audioData->hBufferReady);
        SAFE_CLOSE_HANDLE(audioData->hCaptureThreadShouldExit);

        SAFE_CLOSE_HANDLE(audioData->hRenderThreadShouldExit);
        SAFE_CLOSE_HANDLE(audioData->hRenderThreadDidExit);

    }
    return hr;
}

// Returns 1 on success.
// Note: There are so many conflicting or competing conditions here (and not just here!) since we
// don't lock the audio structure at all. If we experience real problems we
// should go back and add proper locking.
static bool force_render_thread_to_exit_and_wait(AUDIO_SYS_DATA *audioData, int timeout)
{
    audioData->output_canceled = TRUE;
    SetEvent(audioData->hRenderThreadShouldExit);
    return WaitForSingleObject(audioData->hRenderThreadDidExit, timeout) == WAIT_OBJECT_0;
}

void audio_destroy(AUDIO_SYS_HANDLE handle)
{
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

        SAFE_RELEASE(audioData->pAudioInputClient);

        SAFE_RELEASE(audioData->pAudioOutputDevice);

        if (audioData->hRenderThreadShouldExit != NULL  &&
            audioData->hRenderThreadDidExit != NULL)
        {
            force_render_thread_to_exit_and_wait(audioData, INFINITE);
        }
        SAFE_CLOSE_HANDLE(audioData->hRenderThreadShouldExit);
        SAFE_CLOSE_HANDLE(audioData->hRenderThreadDidExit);

        if (audioData->hDeviceName)
        {
            STRING_delete(audioData->hDeviceName);
        }

        free(audioData);
    }
}

AUDIO_RESULT audio_setcallbacks(AUDIO_SYS_HANDLE              handle,
                                ON_AUDIOOUTPUT_STATE_CALLBACK output_cb,
                                void*                         output_ctx,
                                ON_AUDIOINPUT_STATE_CALLBACK  input_cb,
                                void*                         input_ctx,
                                AUDIOINPUT_WRITE              audio_write_cb,
                                void*                         audio_write_ctx,
                                ON_AUDIOERROR_CALLBACK        error_cb,
                                void*                         error_ctx)
{
    AUDIO_RESULT result;
    if (handle != NULL && audio_write_cb != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

        audioData->error_cb         = error_cb;
        audioData->user_errorctx    = error_ctx;
        audioData->user_errorctx    = error_cb;
        audioData->input_state_cb   = input_cb;
        audioData->user_inputctx    = input_ctx;
        audioData->output_state_cb  = output_cb;
        audioData->user_outputctx   = output_ctx;
        audioData->audio_write_cb   = audio_write_cb;
        audioData->user_write_ctx   = audio_write_ctx;
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_output_start(AUDIO_SYS_HANDLE handle, const unsigned char* audioBuffer, size_t audioLen)
{
    AUDIO_RESULT result = AUDIO_RESULT_ERROR;
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        const unsigned char* responseCtx = audioBuffer;
        size_t offset = 0;
        uint32_t magic;
        uint32_t chunkSize;
        AUDIO_WAVEFORMAT fmt;

        if (audioData->current_output_state == AUDIO_STATE_RUNNING)
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
        else
        {
            while (offset < audioLen)
            {
                magic = *(uint32_t*)(responseCtx + offset);
                switch (magic)
                {
                case MAGIC_TAG_RIFF: // RIFF
                    offset += 4;     // we don't care about the RIFF size
                    break;

                case MAGIC_TAG_WAVE: // WAVE no length
                    break;

                case MAGIC_TAG_data: // data
                    offset += 4;
                    chunkSize = *(uint32_t*)(responseCtx + offset);
                    result = write_audio_stream(audioData, &fmt, responseCtx + (offset + 4), chunkSize, NULL, NULL, NULL);
                    break;

                case MAGIC_TAG_fmt:  // fmt - WAVEFORMAT
                    offset += 4;
                    chunkSize = *(uint32_t*)(responseCtx + offset);
                    if (chunkSize >= sizeof(AUDIO_WAVEFORMAT))
                    {
                        AUDIO_WAVEFORMAT *pFormat = (AUDIO_WAVEFORMAT*)(responseCtx + offset + 4);
                        fmt.wFormatTag = pFormat->wFormatTag;
                        fmt.nChannels = pFormat->nChannels;
                        fmt.nSamplesPerSec = pFormat->nSamplesPerSec;
                        fmt.nAvgBytesPerSec = pFormat->nAvgBytesPerSec;
                        fmt.nBlockAlign = pFormat->nBlockAlign;
                        fmt.wBitsPerSample = pFormat->wBitsPerSample;
                    }
                    offset += chunkSize;

                    break;

                default:
                    offset += 4;
                    chunkSize = *(uint32_t*)(responseCtx + offset);
                    offset += chunkSize;
                    break;
                }
                offset += 4; // skip magic
            }
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT  audio_output_startasync(
    AUDIO_SYS_HANDLE handle,
    const AUDIO_WAVEFORMAT* format,
    AUDIOINPUT_WRITE pfnReadCallback,
    AUDIOCOMPLETE_CALLBACK pfnComplete,
    AUDIO_BUFFERUNDERRUN_CALLBACK pfnBufferUnderRun,
    void* pContext)
{
    pfnBufferUnderRun;

    if (!handle)
    {
        return AUDIO_RESULT_ERROR;
    }

    return write_audio_stream(handle, format, NULL, 0, pfnReadCallback, pfnComplete, pContext);
}

AUDIO_RESULT audio_output_pause(AUDIO_SYS_HANDLE handle)
{
    (void)handle;
    return AUDIO_RESULT_OK;
}

AUDIO_RESULT audio_output_restart(AUDIO_SYS_HANDLE handle)
{
    (void)handle;
    return AUDIO_RESULT_OK;
}

AUDIO_RESULT audio_output_stop(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result;
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        if (audioData->current_output_state != AUDIO_STATE_RUNNING)
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
        else
        {
            force_render_thread_to_exit_and_wait(audioData, 5000);
            result = AUDIO_RESULT_OK;
        }
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

DWORD WINAPI captureThreadProc(
    _In_ LPVOID lpParameter
)
{
    HRESULT hr;
    AUDIO_SYS_DATA* pInput = (AUDIO_SYS_DATA*)lpParameter;
    DWORD waitResult;
    IAudioCaptureClient *pCaptureClient;
    int audio_result = 0;

    HANDLE allEvents[] = { pInput->hCaptureThreadShouldExit, pInput->hBufferReady };

    UINT32 packetLength = 0;
    AudioDataBuffer audioBuff = { nullptr, 0, 0 };

    if (pInput->input_state_cb)
    {
        pInput->input_state_cb(pInput->user_inputctx, AUDIO_STATE_STARTING);
    }
    pInput->current_input_state = AUDIO_STATE_RUNNING;

    hr = pInput->pAudioInputClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr);

    audioBuff.totalSize = pInput->inputFrameCnt * pInput->audioInFormat.nBlockAlign;

    audioBuff.pAudioData = (BYTE *)malloc(sizeof(BYTE) * audioBuff.totalSize);
    if (!audioBuff.pAudioData)
    {
        goto Exit;
    }

    for (;;)
    {
        waitResult = WaitForMultipleObjects(2, allEvents, false, INFINITE);
        if ((WAIT_OBJECT_0 + 1) == waitResult)
        {
            while (1)
            {
                hr = pCaptureClient->GetNextPacketSize(&packetLength);
                EXIT_ON_ERROR(hr);

                if (packetLength == 0)
                {
                    break;
                }
                hr = GetBufferAndCallBackClient(pCaptureClient,
                    audioBuff,
                    pInput->audio_write_cb,
                    pInput->user_write_ctx,
                    audio_result);
                EXIT_ON_ERROR(hr);

                if (audio_result)
                {
                    pInput->current_input_state = AUDIO_STATE_STOPPED;
                    if (pInput->input_state_cb)
                    {
                        pInput->input_state_cb(pInput->user_inputctx, AUDIO_STATE_STOPPED);
                    }
                }
            }
        }
        else
        {
            goto Exit;
        }
    }

Exit:
    if (NULL != audioBuff.pAudioData)
    {
        free(audioBuff.pAudioData);
    }

    pInput->current_input_state = AUDIO_STATE_STOPPED;
    if (pInput->input_state_cb)
    {
        pInput->input_state_cb(pInput->user_inputctx, AUDIO_STATE_STOPPED);
    }

    SAFE_RELEASE(pCaptureClient);
    return 0;
}

AUDIO_RESULT audio_input_start(AUDIO_SYS_HANDLE handle)
{
    HRESULT hr = E_FAIL;
    AUDIO_SYS_DATA * audioData = (AUDIO_SYS_DATA*)handle;

    if (!audioData || audioData->hBufferReady == nullptr)
    {
        return AUDIO_RESULT_INVALID_ARG;
    }

    if (!audioData->audio_write_cb || !audioData->pAudioInputClient || audioData->current_input_state == AUDIO_STATE_RUNNING)
    {
        return AUDIO_RESULT_INVALID_STATE;
    }

    if (nullptr == audioData->hCaptureThread)
    {
        audioData->hCaptureThread = CreateThread(0, 0, captureThreadProc, (LPVOID)audioData, 0, nullptr);
        EXIT_ON_ERROR_IF(E_FAIL, audioData->hCaptureThread == nullptr);
    }

    // Start recording, Starting the stream causes the IAudioClient object to begin streaming data between the endpoint buffer and the audio engine.
    hr = audioData->pAudioInputClient->Start();
    EXIT_ON_ERROR_IF(E_FAIL, hr != S_OK);

Exit:
    return SUCCEEDED(hr) ? AUDIO_RESULT_OK : AUDIO_RESULT_ERROR;
}

AUDIO_RESULT audio_input_stop(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result;
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        if (audioData->current_input_state != AUDIO_STATE_RUNNING)
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
        else
        {
            audioData->pAudioInputClient->Stop();

            // Exit our proc thread.
            SetEvent(audioData->hCaptureThreadShouldExit);
            WaitForSingleObject(audioData->hCaptureThread, INFINITE);
            CloseHandle(audioData->hCaptureThread);
            audioData->hCaptureThread = nullptr;

            result = AUDIO_RESULT_OK;
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
    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

        if (strcmp(AUDIO_OPTION_INPUT_FRAME_COUNT, optionName) == 0)
        {
            if (value)
                audioData->inputFrameCnt = (size_t)*((int*)value);
            else
                audioData->inputFrameCnt = 0;
            return AUDIO_RESULT_OK;
        }
        if (strcmp(AUDIO_OPTION_DEVICENAME, optionName) == 0)
        {
            if (!audioData->hDeviceName)
            {
                if(value)
                    audioData->hDeviceName = STRING_construct((char*)value);
                else
                    audioData->hDeviceName = STRING_construct("");
            }
            else
            {
                if (value)
                    STRING_copy(audioData->hDeviceName, (char*)value);
                else
                    STRING_copy(audioData->hDeviceName, "");
            }

            if (audioData->hDeviceName)
            {
                return AUDIO_RESULT_OK;
            }
        }
    }
    return AUDIO_RESULT_INVALID_ARG;

}

AUDIO_RESULT
audio_playwavfile(
    AUDIO_SYS_HANDLE hAudioOut,
    const char* file
)
{
    AUDIO_RESULT retVal = AUDIO_RESULT_OK;
    HANDLE hFile = CreateFileA(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD fileSize = GetFileSize(hFile, nullptr);

        BYTE *pBuffer = new BYTE[fileSize];
        int nOffset = 0;
        DWORD BytesToRead = fileSize;
        do
        {
            DWORD actualBytesRead = 0;
            BOOL bRet = ReadFile(hFile, pBuffer + nOffset, BytesToRead, &actualBytesRead, nullptr);
            if (!bRet)
            {
                retVal = AUDIO_RESULT_ERROR;
                break;
            }

            nOffset += actualBytesRead;
            BytesToRead -= BytesToRead;
            actualBytesRead = 0;
        } while (BytesToRead > 0);

        CloseHandle(hFile);

        if (BytesToRead == 0)
        {
            retVal = audio_output_start(hAudioOut, pBuffer, fileSize);
        }

        if (pBuffer)
        {
            delete[] pBuffer;
        }
    }
    else
    {
        LogError("File not found: %s", file);
        retVal = AUDIO_RESULT_INVALID_ARG;
    }

    return retVal;
}

AUDIO_RESULT  audio_output_set_volume(AUDIO_SYS_HANDLE handle, long volume)
{
    IAudioEndpointVolume *endpointVolume = NULL;
    HRESULT hr = E_FAIL;
    AUDIO_RESULT result = AUDIO_RESULT_ERROR;

    if (!handle) goto Exit;
    if (!(handle->pAudioOutputDevice)) goto Exit;
    if (volume < 0 || volume > 100)
    {
        result = AUDIO_RESULT_INVALID_ARG;
        goto Exit;
    }
    float target = (float)volume / 100;
    hr = handle->pAudioOutputDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    EXIT_ON_ERROR(hr);

    hr = endpointVolume->SetMasterVolumeLevelScalar(target, NULL);
    EXIT_ON_ERROR(hr);
    result = AUDIO_RESULT_OK;

Exit:
    if (endpointVolume)
        endpointVolume->Release();
    return result;
}

AUDIO_RESULT  audio_output_get_volume(AUDIO_SYS_HANDLE handle, long* volume)
{
    IAudioEndpointVolume *endpointVolume = NULL;
    AUDIO_RESULT result = AUDIO_RESULT_ERROR;
    HRESULT hr = E_FAIL;
    float currentVolume = 0;

    if (!handle) goto Exit;
    if (!(handle->pAudioOutputDevice)) goto Exit;
    hr = handle->pAudioOutputDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    EXIT_ON_ERROR(hr);
    hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
    EXIT_ON_ERROR(hr);
    if (currentVolume < 0 || currentVolume > 1)
        goto Exit;
    *volume = (long)(currentVolume * 100);
    result = AUDIO_RESULT_OK;

Exit:
    if (endpointVolume)
        endpointVolume->Release();
    return result;
}

STRING_HANDLE audio_output_get_name(AUDIO_SYS_HANDLE handle)
{
    AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

    if (handle)
    {
        if (audioData->hDeviceName)
        {
            return STRING_clone(audioData->hDeviceName);
        }
    }

    return NULL;
}
