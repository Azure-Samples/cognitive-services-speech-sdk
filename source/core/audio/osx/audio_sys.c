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

#include <AudioToolbox/AudioToolbox.h>

#include "audio_sys.h"
#include "spxdebug.h"
#include "azure_c_shared_utility/lock.h"


// for checking for mac/iPhone
#include "TargetConditionals.h"

#define AUDIO_CHANNELS_MONO     1
#define AUDIO_SAMPLE_RATE       16000
#define AUDIO_BITS              16
#define N_RECORD_BUFFERS 3


// User info struct for recording audio queue callbacks
typedef struct AUDIO_SYS_DATA_TAG {
    ON_AUDIOERROR_CALLBACK        error_cb;
    ON_AUDIOOUTPUT_STATE_CALLBACK output_state_cb;
    ON_AUDIOINPUT_STATE_CALLBACK  input_state_cb;
    AUDIOINPUT_WRITE              audio_write_cb;
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

    if (isprint(out[1]) && isprint(out[2]) &&
        isprint(out[3]) && isprint(out[4]))
    {
        out[0] = out[5] = '\'';
        out[6] = '\0';
    } else
    {
        sprintf(out, "%d", (int)error);
    }
}


// Log formatted errors from Core Audio
static void logOSStatusError(OSStatus error, const char *operation)
{
    if (noErr == error) return;

    char errorString[20];
    stringifyOSStatus(error, errorString);

    SPX_DBG_TRACE_ERROR("Error: %s (%s)\n", operation, errorString);
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
        audioData->audio_write_cb(audioData->user_write_ctx, inBuffer->mAudioData, inBuffer->mAudioDataByteSize);

        //Re-enqueue used buffer
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


AUDIO_SYS_HANDLE audio_create()
{
    SPX_DBG_TRACE_VERBOSE("setting up AudioQueue");
    AUDIO_SYS_DATA* audioData;

    bool setup_ok = TRUE;
    audioData = (AUDIO_SYS_DATA*)malloc(sizeof(AUDIO_SYS_DATA));
    if (audioData != NULL)
    {
        memset(audioData, 0, sizeof(AUDIO_SYS_DATA));

        // TODO make these configurable
        audioData->channels = AUDIO_CHANNELS_MONO;
        audioData->sampleRate = AUDIO_SAMPLE_RATE;
        audioData->bitsPerSample = AUDIO_BITS;
        audioData->current_output_state = AUDIO_STATE_STOPPED;
        audioData->current_input_state = AUDIO_STATE_STOPPED;

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

        if (setup_ok)
        {
            audioData->bufferStackLock = Lock_Init();
            // Allocate and enqueue buffers
            int bufferByteSize = recordFormat.mSampleRate;
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


void audio_destroy(AUDIO_SYS_HANDLE handle)
{
    if (handle != NULL)
    {

        SPX_DBG_TRACE_INFO("Destroying AudioQueue...");
        AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;

        // free all non-enqueued buffers
        AudioQueueBufferRef buffer = NULL;
        while ((buffer = popBuffer(audioData)))
        {
            OSStatus error = AudioQueueFreeBuffer(audioData->audioQueue, buffer);
            if (error != noErr) {
                logOSStatusError(error, "AudioQueueFreeBuffer failed finally");
            }
        }

        // dispose of AudioQueue, this releases all the remaining buffers
        OSStatus error = AudioQueueDispose(audioData->audioQueue, TRUE);
        if (error != noErr) {
            logOSStatusError(error, "AudioQueueDispose failed");
        }
        audioData->audioQueue = NULL;
        SPX_DBG_TRACE_INFO("AudioQueue destroyed.");
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
    SPX_DBG_TRACE_VERBOSE("Stopping AudioQueue");
    AUDIO_RESULT result = AUDIO_RESULT_OK;
    AUDIO_SYS_DATA* audioData = (AUDIO_SYS_DATA*)handle;
    if (handle != NULL)
    {
        if (audioData->current_input_state != AUDIO_STATE_RUNNING)
        {
            return AUDIO_RESULT_INVALID_STATE;
        }

        audioData->current_input_state = AUDIO_STATE_STOPPED;
        if (audioData->input_state_cb)
        {
            audioData->input_state_cb(audioData->user_inputctx, audioData->current_input_state);
        }

        OSStatus error = AudioQueueStop(audioData->audioQueue, TRUE);
        if (noErr != error)
        {
            logOSStatusError(error, "AudioQueueStop failed");
            return AUDIO_RESULT_ERROR;
        }
    }
    else
    {
        SPX_DBG_TRACE_ERROR("could not stop AudioQueue");
        result = AUDIO_RESULT_INVALID_ARG;
    }
    return result;
}
