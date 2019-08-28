//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_sys.c: Audio susbystem for macOS and iOS
//
#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"
#include "spx_namespace.h"

#include <AudioToolbox/AudioToolbox.h>

#if !TARGET_OS_IPHONE
#include <CoreAudio/AudioHardware.h>
#endif

#include "audio_sys.h"
#include "spxdebug.h"
#include "azure_c_shared_utility/lock.h"


// for checking for mac/iPhone
#include "TargetConditionals.h"

#define N_RECORD_BUFFERS 3
#define N_PLAYBACK_BUFFERS 2


// User info struct for recording audio queue callbacks
typedef struct AUDIO_SYS_DATA_TAG {
    ON_AUDIOERROR_CALLBACK        error_cb;
    ON_AUDIOOUTPUT_STATE_CALLBACK output_state_cb;
    ON_AUDIOINPUT_STATE_CALLBACK  input_state_cb;
    AUDIOINPUT_WRITE              audio_write_cb;
    AUDIOCOMPLETE_CALLBACK        audio_complete_cb;
    void* user_write_ctx;
    void* user_outputctx;
    void* user_inputctx;
    void* user_errorctx;
    uint16_t channels;
    uint32_t sampleRate;
    uint16_t bitsPerSample;
    AUDIO_STATE current_output_state;
    AUDIO_STATE current_input_state;

    AudioQueueRef audioQueue;

    // stack to keep track of audio buffers that are not enqueued in the AudioQueue
    AudioQueueBufferRef bufferStack[N_RECORD_BUFFERS];
    int nStackedBuffers;
    LOCK_HANDLE bufferStackLock;

    // this stop lock is used when stopping/disposing AudioQueue in playback
    LOCK_HANDLE stopLock;

    STRING_HANDLE hDeviceName;
} AUDIO_SYS_DATA;


// push a AudioQueueBufferRef on the internal stack
int pushBuffer (AUDIO_SYS_DATA *audioData, AudioQueueBufferRef buffer)
{
    Lock(audioData->bufferStackLock);

    int retval = 0;

    if (N_RECORD_BUFFERS <= audioData->nStackedBuffers)
    {
        retval = -1;
    }
    else
    {
        audioData->bufferStack[audioData->nStackedBuffers] = buffer;
        audioData->nStackedBuffers++;
    }
    Unlock(audioData->bufferStackLock);
    return retval;
}


// pop a AudioQueueBufferRef from the internal stack
AudioQueueBufferRef popBuffer (AUDIO_SYS_DATA *audioData)
{
    Lock(audioData->bufferStackLock);

    AudioQueueBufferRef retval;
    if (0 == audioData->nStackedBuffers)
    {
        retval = NULL;
    }
    else
    {
        retval = audioData->bufferStack[audioData->nStackedBuffers - 1];
        audioData->bufferStack[audioData->nStackedBuffers - 1] = NULL;
        audioData->nStackedBuffers--;
    }

    Unlock(audioData->bufferStackLock);
    return retval;
}


// Convert OS error codes to human-readable form
static void stringifyOSStatus(OSStatus error, char *out) {
    *(UInt32 *)(out + 1) = CFSwapInt32HostToBig(error);
// clang-analyze doesn't understand the cast and thinks out[2..4] is uninitialized
#ifndef __clang_analyzer__
    if (isprint(out[1]) && isprint(out[2]) &&
        isprint(out[3]) && isprint(out[4]))
    {
        out[0] = out[5] = '\'';
        out[6] = '\0';
    } else
    {
        sprintf(out, "%d", (int)error);
    }
#endif //__clang_analyzer__
}


// Log formatted errors from Core Audio
static void logOSStatusError(OSStatus error, const char *operation)
{
    if (noErr == error) return;

    char errorString[20];
    stringifyOSStatus(error, errorString);

    // clang complains about using SPX_DBG_TRACE_ERROR with more than one
    // argument in C.  So, constructing the error message manually here.
    char message[1024];
    snprintf(message, 1024, "Error: %s (%s)", operation, errorString);
    LogError("%s", message);
    SPX_DBG_TRACE_ERROR("%s", message);
}

static void audioQueueDispose(AUDIO_SYS_DATA* audioData)
{
    if (audioData->stopLock) {
        Lock(audioData->stopLock);
    }
    auto error = AudioQueueDispose(audioData->audioQueue, TRUE);
    if (noErr != error)
    {
        logOSStatusError(error, "AudioQueueDispose failed");
    }
    audioData->audioQueue = nullptr;
    SPX_DBG_TRACE_ERROR("AudioQueue destroyed.");
    if (audioData->stopLock) {
        Unlock(audioData->stopLock);
    }
    if (audioData->bufferStackLock) {
        Lock_Deinit(audioData->bufferStackLock);
    }
    if (audioData->stopLock) {
        Lock_Deinit(audioData->stopLock);
    }
}

// Callback function for the AudioQueue
static void audioQueueInputCallback(void *inUserData,
                            AudioQueueRef inQueue,
                            AudioQueueBufferRef inBuffer,
                            const AudioTimeStamp *inStartTime,
                            UInt32 inNumPacketDescriptions,
                            const AudioStreamPacketDescription *inPacketDesc)
{
    AUDIO_SYS_DATA *audioData = (AUDIO_SYS_DATA *)inUserData;

    if (inBuffer->mAudioDataByteSize > 0)
    {
        // SPX_DBG_TRACE_VERBOSE("recorded %d bytes of audio, calling callback\n", inBuffer->mAudioDataByteSize);

        // call the application callback
        audioData->audio_write_cb(audioData->user_write_ctx, reinterpret_cast<uint8_t*>(inBuffer->mAudioData), inBuffer->mAudioDataByteSize);

        // Re-enqueue used buffer
        OSStatus result = AudioQueueEnqueueBuffer(inQueue, inBuffer, 0, NULL);

        if (noErr != result) {
            if (kAudioQueueErr_EnqueueDuringReset != result)
            {
                // unless callback is running while queue is being shut down
                logOSStatusError(result, "AudioQueueEnqueueBuffer failed");
            }
            audioData->current_input_state = AUDIO_STATE_STOPPED;

            int error = pushBuffer(audioData, inBuffer);
            if (0 != error)
            {
                SPX_DBG_TRACE_ERROR("push Buffer failed; n buffers on stack: %s", audioData->nStackedBuffers);
            }
        }
    }
}

static void audioQueueOutputCallback(void *inUserData,
                            AudioQueueRef inQueue,
                            AudioQueueBufferRef inBuffer)
{
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(inUserData);

    if (inBuffer->mAudioDataBytesCapacity > 0)
    {
        // call the read audio buffer callback
        int write_size = audioData->audio_write_cb(audioData->user_write_ctx, reinterpret_cast<uint8_t*>(inBuffer->mAudioData), inBuffer->mAudioDataBytesCapacity);
        if (write_size > 0) {
            inBuffer->mAudioDataByteSize = write_size;
            // Re-enqueue used buffer
            OSStatus result = AudioQueueEnqueueBuffer(inQueue, inBuffer, 0, NULL);

            if (noErr != result) {
                if (kAudioQueueErr_EnqueueDuringReset != result)
                {
                    // unless callback is running while queue is being shut down
                    logOSStatusError(result, "AudioQueueEnqueueBuffer failed");
                }
                audioData->current_output_state = AUDIO_STATE_STOPPED;

                // free the buffer that wasn't correctly enqueued
                auto error = AudioQueueFreeBuffer(audioData->audioQueue, inBuffer);
                if (noErr != error)
                {
                    logOSStatusError(error, "AudioQueueFreeBuffer failed finally");
                }

                // call audio complete callback
                audioData->audio_complete_cb(audioData->user_write_ctx);
            }
        }
        else {
            // all audio data is read, stopping audio.
            if (AUDIO_STATE_RUNNING == audioData->current_output_state)
            {
                Lock(audioData->stopLock);
                // audio queue will stop after its queued buffers are played
                auto error = AudioQueueStop(audioData->audioQueue, false);
                if (noErr != error)
                {
                    logOSStatusError(error, "AudioQueueStop failed.");
                    // stop the audio queue immediately
                    audioData->current_output_state = AUDIO_STATE_STOPPED;
                    // call audio complete callback
                    audioData->audio_complete_cb(audioData->user_write_ctx);
                }
                Unlock(audioData->stopLock);
            }
        }
    }
}

void audioQueueIsRunningCallback(void *inUserData, AudioQueueRef inQueue, AudioQueuePropertyID inID)
{
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(inUserData);
    Lock(audioData->stopLock);
    SPX_DBG_ASSERT(kAudioQueueProperty_IsRunning == inID);
    UInt32 running;
    UInt32 output = sizeof(running);
    OSStatus err = AudioQueueGetProperty(audioData->audioQueue, kAudioQueueProperty_IsRunning, &running, &output);
    if (noErr != err) {
        logOSStatusError(err, "error in kAudioQueueProperty_IsRunning.");
    } else {
        if (running) {
            LogInfo("Audio playback started.");
        } else {
            audioData->current_output_state = AUDIO_STATE_STOPPED;
            audioData->audio_complete_cb(audioData->user_write_ctx);
        }
    }
    Unlock(audioData->stopLock);
} 

AUDIO_SYS_HANDLE audio_input_create_with_parameters(AUDIO_SETTINGS_HANDLE format)
{
    SPX_DBG_TRACE_VERBOSE("setting up recording AudioQueue");
    AUDIO_SYS_DATA* audioData;

    bool setup_ok = TRUE;
    audioData = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    if (audioData != NULL)
    {
        memset(audioData, 0, sizeof(AUDIO_SYS_DATA));

        audioData->channels = format->nChannels;
        audioData->sampleRate = format->nSamplesPerSec;
        audioData->bitsPerSample = format->wBitsPerSample;

        audioData->current_output_state = AUDIO_STATE_STOPPED;
        audioData->current_input_state = AUDIO_STATE_STOPPED;

        audioData->hDeviceName = STRING_clone(format->hDeviceName);

        // Set audio stream description for Linear PCM
        AudioStreamBasicDescription recordFormat = {0};
        recordFormat.mFormatID = kAudioFormatLinearPCM;
        recordFormat.mChannelsPerFrame = audioData->channels;
        recordFormat.mSampleRate = audioData->sampleRate;
        recordFormat.mBitsPerChannel = audioData->bitsPerSample;
        recordFormat.mFramesPerPacket = 1;
        recordFormat.mBytesPerFrame = recordFormat.mBytesPerPacket = (recordFormat.mBitsPerChannel / 8) * recordFormat.mChannelsPerFrame;
        recordFormat.mFormatID = kAudioFormatLinearPCM;

        recordFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;

        // Create new audio queue for input
        OSStatus error = AudioQueueNewInput(&recordFormat,
                                            audioQueueInputCallback,
                                            audioData,
                                            NULL,
                                            kCFRunLoopCommonModes,
                                            0,
                                            &audioData->audioQueue);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueNewInput failed");
            setup_ok = FALSE;
        }

        if (0 != strcmp(STRING_c_str(format->hDeviceName), ""))
        {
            CFStringRef deviceUID = CFStringCreateWithCString(NULL, STRING_c_str(format->hDeviceName), kCFStringEncodingUTF8);
            SPX_DBG_TRACE_INFO("Initializing microphone input from device: %s", STRING_c_str(format->hDeviceName));
            error = AudioQueueSetProperty(audioData->audioQueue,
                                  kAudioQueueProperty_CurrentDevice,
                                  &deviceUID,
                                  sizeof(CFStringRef));
            if (noErr != error)
            {
                logOSStatusError(error, "AudioQueueNewInput failed");
                setup_ok = FALSE;
            }
            CFRelease(deviceUID);
            deviceUID = NULL;
        }
        else
        {
            SPX_DBG_TRACE_INFO("Initializing microphone input from default device.");
        }

        if (setup_ok)
        {
            audioData->bufferStackLock = Lock_Init();
            // Allocate and enqueue buffers
            int bufferByteSize = recordFormat.mSampleRate * recordFormat.mBytesPerFrame / 10; // 100ms buffer length
            for (int bufferIndex = 0; bufferIndex < N_RECORD_BUFFERS; ++bufferIndex)
            {
                AudioQueueBufferRef buffer;
                error = AudioQueueAllocateBuffer(audioData->audioQueue, bufferByteSize, &buffer);
                if (noErr != error)
                {
                    logOSStatusError(error, "AudioQueueAllocateBuffer failed");
                    // dispose of the audio queue
                    error = AudioQueueDispose(audioData->audioQueue, TRUE);
                    if (noErr != error)
                    {
                        logOSStatusError(error, "AudioQueueDispose failed");
                    }
                    setup_ok = FALSE;
                    break;
                }

                error = AudioQueueEnqueueBuffer(audioData->audioQueue, buffer, 0, NULL);
                if (noErr != error)
                {
                    logOSStatusError(error, "AudioQueueEnqueueBuffer failed");
                    // free the buffer that wasn't correctly enqueued
                    error = AudioQueueFreeBuffer(audioData->audioQueue, buffer);
                    if (noErr != error)
                    {
                        logOSStatusError(error, "AudioQueueFreeBuffer failed");
                    }
                    // dispose of the audio queue
                    error = AudioQueueDispose(audioData->audioQueue, TRUE);
                    if (noErr != error)
                    {
                        logOSStatusError(error, "AudioQueueDispose failed");
                    }
                    setup_ok = FALSE;
                    break;
                }
            }
        }
        if (!setup_ok)
        {
            free(audioData);
            audioData = NULL;
        }
    }

    return audioData;
}

AUDIO_SYS_HANDLE audio_output_create_with_parameters(AUDIO_SETTINGS_HANDLE format)
{
    SPX_DBG_TRACE_VERBOSE("setting up playback audioData");
    AUDIO_SYS_DATA* audioData;

    audioData = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    if (audioData != NULL)
    {   
        auto setup_ok = true;
        memset(audioData, 0, sizeof(AUDIO_SYS_DATA));

        audioData->channels = format->nChannels;
        audioData->sampleRate = format->nSamplesPerSec;
        audioData->bitsPerSample = format->wBitsPerSample;

        audioData->current_output_state = AUDIO_STATE_STOPPED;
        audioData->current_input_state = AUDIO_STATE_STOPPED;

        if (0 == strcmp(STRING_c_str(format->hDeviceName), "")) // check if default output device is available
        {
#if !TARGET_OS_IPHONE
            AudioDeviceID defaultDevice = kAudioObjectUnknown;
            UInt32 defaultSize = sizeof(AudioDeviceID);
            const AudioObjectPropertyAddress defaultAddr =
            {
                kAudioHardwarePropertyDefaultOutputDevice,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            auto error = AudioObjectGetPropertyData(kAudioObjectSystemObject, &defaultAddr, 0, NULL, &defaultSize, &defaultDevice);
            setup_ok = (error == noErr) && (defaultDevice != kAudioObjectUnknown);
            if (!setup_ok)
            {
                LogError("Default output device not available, ongoing without audio output.");
            }
#endif
        }
        else
        {
            LogError("specific audio output device is not supported now.");
            setup_ok = false;
        }

        AudioStreamBasicDescription playbackFormat = {0};

        if (setup_ok)
        {
            switch (format->wFormatTag) // TODO: add other supported format
            {
                case Microsoft::CognitiveServices::Speech::Impl::WAVE_FORMAT_PCM:
                    playbackFormat.mFormatID = kAudioFormatLinearPCM;
                    break;

                default:
                    LogError("Unsupported audio format for playing");
                    setup_ok = false;
                    break;
            }
        }
        if (setup_ok)
        {
            audioData->stopLock = Lock_Init();
            playbackFormat.mChannelsPerFrame = audioData->channels;
            playbackFormat.mSampleRate = audioData->sampleRate;
            playbackFormat.mBitsPerChannel = audioData->bitsPerSample;
            playbackFormat.mFramesPerPacket = 1;
            playbackFormat.mBytesPerFrame = playbackFormat.mBytesPerPacket = (playbackFormat.mBitsPerChannel / 8) * playbackFormat.mChannelsPerFrame;
            playbackFormat.mFormatID = kAudioFormatLinearPCM;

            playbackFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;

            // Create new audio queue for output
            OSStatus error = AudioQueueNewOutput(&playbackFormat,
                                                audioQueueOutputCallback,
                                                audioData,
                                                NULL,
                                                kCFRunLoopCommonModes,
                                                0,
                                                &audioData->audioQueue);
            if (noErr != error)
            {
                logOSStatusError(error, "AudioQueueNewOutput failed");
                setup_ok = false;
            }
        }
        if (!setup_ok)
        {
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
    if (handle != NULL)
    {
        SPX_DBG_TRACE_INFO("Destroying AudioQueue...");
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

        // The bufferStack/bufferStackLock is not used in audio output, no need to release.
        if (audioData->nStackedBuffers > 0) 
        { 
            // free all non-enqueued buffers
            AudioQueueBufferRef buffer = NULL;
            while ((buffer = popBuffer(audioData)))
            {
                OSStatus error = AudioQueueFreeBuffer(audioData->audioQueue, buffer);
                if (error != noErr) {
                    logOSStatusError(error, "AudioQueueFreeBuffer failed finally");
                }
            }
        }
        // dispose of AudioQueue, this releases all the remaining buffers
        audioQueueDispose(audioData);
        
        if (audioData->hDeviceName)
        {
            STRING_delete(audioData->hDeviceName);
        }
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
        audioData->error_cb      = error_cb;
        audioData->user_errorctx = error_ctx;
        audioData->input_state_cb = input_cb;
        audioData->user_inputctx  = input_ctx;
        audioData->output_state_cb = output_cb;
        audioData->user_outputctx = output_ctx;
        audioData->audio_write_cb = audio_write_cb;
        audioData->user_write_ctx = audio_write_ctx;
        result = AUDIO_RESULT_OK;
    }
    else
    {
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}


AUDIO_RESULT audio_input_start(AUDIO_SYS_HANDLE handle)
{
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    SPX_DBG_TRACE_VERBOSE("Starting AudioQueue");

    if (handle != NULL)
    {
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
        if (audioData->current_input_state == AUDIO_STATE_RUNNING)
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
        else
        {
            audioData->current_input_state = AUDIO_STATE_STARTING;

            // update state
            if (audioData->input_state_cb)
            {
                audioData->input_state_cb(audioData->user_inputctx, audioData->current_input_state);
            }
            audioData->current_input_state = AUDIO_STATE_RUNNING;

            // enqueue all currently non-enqueued buffers
            AudioQueueBufferRef buffer = NULL;
            while ((buffer = popBuffer(audioData)))
            {
                OSStatus error = AudioQueueEnqueueBuffer(audioData->audioQueue, buffer, 0, NULL);
                if (noErr != error)
                {
                    logOSStatusError(error, "AudioQueueEnqueueBuffer failed");
                    // free the buffer that wasn't correctly enqueued
                    error = AudioQueueFreeBuffer(audioData->audioQueue, buffer);
                    if (noErr != error)
                    {
                        logOSStatusError(error, "AudioQueueFreeBuffer failed finally");
                    }
                    // and mark an error
                    result = AUDIO_RESULT_ERROR;
                }
            }

            if (AUDIO_RESULT_ERROR != result)
            {
                // Start the audio queue, unless there was an error
                OSStatus error;
                error = AudioQueueStart(audioData->audioQueue, NULL);
                if (noErr != error)
                {
                    logOSStatusError(error, "AudioQueueStart failed");
                    result = AUDIO_RESULT_ERROR;
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

AUDIO_RESULT audio_stop(AUDIO_SYS_DATA* audioData, AUDIO_STATE& current_state)
{
    SPX_DBG_TRACE_VERBOSE("Stopping AudioQueue");
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    if (audioData != NULL)
    {
        if (AUDIO_STATE_STOPPED == current_state) {
            return AUDIO_RESULT_OK;
        }
        if (current_state != AUDIO_STATE_RUNNING && current_state != AUDIO_STATE_PAUSED && current_state != AUDIO_STATE_STOPPING)
        {
            return AUDIO_RESULT_INVALID_STATE;
        }

        current_state = AUDIO_STATE_STOPPED;
        if (audioData->input_state_cb) // record state callback
        {
            audioData->input_state_cb(audioData->user_inputctx, audioData->current_input_state);
        }
        else if (audioData->audio_complete_cb) // playback complete callback
        {
            audioData->audio_complete_cb(audioData->user_write_ctx);
        }

        OSStatus error = AudioQueueStop(audioData->audioQueue, TRUE);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueStop failed");
            result = AUDIO_RESULT_ERROR;
        }
    }
    else
    {
        SPX_DBG_TRACE_ERROR("could not stop AudioQueue");
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}

AUDIO_RESULT audio_input_stop(AUDIO_SYS_HANDLE handle)
{
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        return audio_stop(audioData, audioData->current_input_state);
    }
    else
    {
        return AUDIO_RESULT_INVALID_ARG;
    }
}

AUDIO_RESULT audio_output_stop(AUDIO_SYS_HANDLE handle) 
{
    if (handle != nullptr)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        Lock(audioData->stopLock);
        auto result = audio_stop(audioData, audioData->current_output_state);
        Unlock(audioData->stopLock);
        return result;
    }
    else
    {
        return AUDIO_RESULT_INVALID_ARG;
    }
}

STRING_HANDLE get_input_device_nice_name(AUDIO_SYS_HANDLE handle) {
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

    if (handle)
    {
        return STRING_clone(audioData->hDeviceName);
    }

    return nullptr;
}

AUDIO_RESULT audio_output_startasync(
    AUDIO_SYS_HANDLE handle,
    const AUDIO_WAVEFORMAT* format,
    AUDIOINPUT_WRITE pfnReadCallback,
    AUDIOCOMPLETE_CALLBACK pfnComplete,
    AUDIO_BUFFERUNDERRUN_CALLBACK pfnBufferUnderRun,
    void* pContext) 
{
    if (nullptr == handle || nullptr == format)
    {
        LogError("audio_output_startasync failed, null AUDIO_SYS_HANDLE or AUDIO_WAVEFORMAT");
        return AUDIO_RESULT_INVALID_ARG;
    }
    
    AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);

    if (AUDIO_STATE_RUNNING == audioData->current_output_state || AUDIO_STATE_STARTING == audioData->current_output_state)
    {
        SPX_DBG_TRACE_VERBOSE("%s: the audio is starting or running, do nothing.", __FUNCTION__);
        return AUDIO_RESULT_OK;
    }

    audioData->audio_write_cb = pfnReadCallback;
    audioData->user_write_ctx = pContext;
    audioData->audio_complete_cb = pfnComplete;

    audioData->current_output_state = AUDIO_STATE_STARTING;
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    OSStatus error = noErr;

    // Allocate and enqueue buffers
    int bufferByteSize = audioData->sampleRate * audioData->bitsPerSample * audioData->channels / 8 / 20; // 50ms buffer length
    for (int bufferIndex = 0; bufferIndex < N_PLAYBACK_BUFFERS; ++bufferIndex)
    {
        AudioQueueBufferRef buffer;
        error = AudioQueueAllocateBuffer(audioData->audioQueue, bufferByteSize, &buffer);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueAllocateBuffer failed");
            // dispose of the audio queue
            audioQueueDispose(audioData);
            result = AUDIO_RESULT_ERROR;
            break;
        }

        // TODO: update this when CanRead method in PullAudioOutputStream is available
        int write_size = audioData->audio_write_cb(audioData->user_write_ctx, reinterpret_cast<uint8_t*>(buffer->mAudioData), 256);
        if (!write_size) {
            LogError("The synthesized audio is too short to play.");
        }
        buffer->mAudioDataByteSize = write_size;

        error = AudioQueueEnqueueBuffer(audioData->audioQueue, buffer, 0, NULL);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueEnqueueBuffer failed");
            // free the buffer that wasn't correctly enqueued
            error = AudioQueueFreeBuffer(audioData->audioQueue, buffer);
            if (noErr != error)
            {
                logOSStatusError(error, "AudioQueueFreeBuffer failed");
            }
            // dispose of the audio queue
            audioQueueDispose(audioData);
            result = AUDIO_RESULT_ERROR;
            break;
        }
    }

    if (AUDIO_RESULT_OK == result)
    {
        // install is running callback
        error = AudioQueueAddPropertyListener(audioData->audioQueue, kAudioQueueProperty_IsRunning, audioQueueIsRunningCallback, audioData);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueAddPropertyListener failed");
            // dispose of the audio queue
            audioQueueDispose(audioData);
            result = AUDIO_RESULT_ERROR;
        }
    }

    if (AUDIO_RESULT_OK == result)
    {
        error = AudioQueueStart(audioData->audioQueue, NULL);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueStart failed");
            // dispose of the audio queue
            audioQueueDispose(audioData);
            result = AUDIO_RESULT_ERROR;
        }
    }

    if (AUDIO_RESULT_OK == result)
    {
        audioData->current_output_state = AUDIO_STATE_RUNNING;
    }
    return result;
}

AUDIO_RESULT audio_output_pause(AUDIO_SYS_HANDLE handle) 
{
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    if (nullptr != handle)
    {
        AUDIO_SYS_DATA* audioData = reinterpret_cast<AUDIO_SYS_DATA*>(handle);
        if (AUDIO_STATE_RUNNING == audioData->current_output_state)
        {
            auto error = AudioQueuePause(audioData->audioQueue);
            audioData->current_output_state = AUDIO_STATE_PAUSED;
            if (noErr != error)
            {
                logOSStatusError(error, "AudioQueuePause failed");
                result = AUDIO_RESULT_ERROR;
            }
        }
        else if (AUDIO_STATE_PAUSED == audioData->current_output_state)
        {
            SPX_DBG_TRACE_VERBOSE("%s: the audio is paused already, do nothing.", __FUNCTION__);
        }
        else
        {
            result = AUDIO_RESULT_INVALID_STATE;
        }
    }
    else
    {
        SPX_DBG_TRACE_ERROR("could not pause AudioQueue");
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return AUDIO_RESULT_OK;
}
