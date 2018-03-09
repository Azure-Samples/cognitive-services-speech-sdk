//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspclientconsole.c: A console application for testing USP client library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <mmsystem.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "audio_sys.h"
#include "azure_c_shared_utility/threadapi.h"

// #include "azure_c_shared_utility/audio_sys.h"

#include "usp.h"

#define UNUSED(x) (void)(x)

bool turnEnd = false;
THREAD_HANDLE ttsThreadHandle;

char* recognitionStatusToText[] =
{
    "Success",  // RECOGNITON_SUCCESS
    "No Match", // RECOGNITION_NO_MATCH
    "Initial Silence Timeout", //RECOGNITION_INITIAL_SILENCE_TIMEOUT
    "Babble Timeout", // RECOGNITION_BABBLE_TIMEOUT
    "Error", // RECOGNITION_ERROR
    "EndOfDictation" // END_OF_DICTATION 
};

void OnSpeechStartDetected(UspHandle handle, void* context, UspMsgSpeechStartDetected *message)
{
    UNUSED(handle);
    UNUSED(context);
    UNUSED(message);

    // offset not supported yet.
    printf("Response: Speech.StartDetected message.\n");
    //printf("Response: Speech.StartDetected message. Speech starts at offset %" PRIu64 ".\n", message->offset);
}

void OnSpeechEndDetected(UspHandle handle, void* context, UspMsgSpeechEndDetected *message)
{
    UNUSED(handle);
    UNUSED(context);
    UNUSED(message);

    // offset not supported yet.
    printf("Response: Speech.EndDetected message.\n");
    // printf("Response: Speech.EndDetected message. Speech ends at offset %" PRIu64 "\n", message->offset);
}

void OnSpeechHypothesis(UspHandle handle, void* context, UspMsgSpeechHypothesis *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.Hypothesis message. Text: %ls, starts at offset %" PRIu64 ", with duration %" PRIu64 ".\n", message->text, message->offset, message->duration);
}

void OnSpeechPhrase(UspHandle handle, void* context, UspMsgSpeechPhrase *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.Phrase message. Status: %s, Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 ".\n", recognitionStatusToText[message->recognitionStatus], message->displayText, message->offset, message->duration);
}

void OnSpeechFragment(UspHandle handle, void* context, UspMsgSpeechFragment *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.Fragment message. Text: %ls, starts at %" PRIu64 ", with duration %" PRIu64 ".\n", message->text, message->offset, message->duration);
}

void OnTurnStart(UspHandle handle, void* context, UspMsgTurnStart *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Turn.Start message. Context.ServiceTag: %S\n", message->contextServiceTag);
}

void OnTurnEnd(UspHandle handle, void* context, UspMsgTurnEnd *message)
{
    UNUSED(handle);
    UNUSED(context);
    UNUSED(message);
    printf("Response: Turn.End message.\n");
    turnEnd = true;
}

void OnError(UspHandle handle, void* context, const UspError* error)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: On Error: 0x%x (%s).\n", error->errorCode, error->description);
    turnEnd = true;
    exit(1);
}

void OnUserMessage(UspHandle uspHandle, const char* path, const char* contentType, const unsigned char* buffer, size_t size, void* context)
{
    UNUSED(uspHandle);
    UNUSED(context);
    printf("Response: User defined message. Path: %s, contentType: %s, size: %zu, content: %s.\n", path, contentType, size, buffer);
}

// TODO: MSFT 1135317 Move TTS to own source file
// We receive the audio response chunk by chunk (for example from TTS) and store it into a file. 
// Once the entire response is received, we play out the audio file using the audio system of the OS.
static int TTSRenderLoop(void* ptr)
{
    UspMsgAudioStreamStart* msg = (UspMsgAudioStreamStart*)ptr;
    IoBufferAddRef(msg->ioBuffer);
    FILE* pFile = fopen("test.wav","wb");
    int maxSize = 0;
    uint8_t* buffer = NULL;
    AUDIO_SYS_HANDLE hAudio;

    while (IoBufferWaitForNewBytes(msg->ioBuffer, 500) == 0)
    {
        int size = IoBufferGetUnReadBytes(msg->ioBuffer);
        if (size == 0)
        {
            printf("Response: Final Audio Chunk received.\n");
            break;
        }
        if (maxSize == 0)
        {
            buffer = malloc(size);
            maxSize = size;
        }
        else if (size > maxSize)
        {
            buffer = realloc(buffer, size);
            maxSize = size;
        }
        IoBufferRead(msg->ioBuffer, buffer, 0, size, 100);
        fwrite(buffer, size, 1, pFile);
    }
    long fileSize = ftell(pFile);

    fclose(pFile);
    free(buffer);

    printf("Response: Playing TTS Audio...\n");

#ifdef __MACH__
    // TODO: merge 182438c675f0459f91a4c4f53ad5fd1cbba63ef0 from CortanaSDK
    printf("ERROR: audio system is not yet supported on OSX.");
#else
    hAudio = audio_create();
    audio_output_set_volume(hAudio, 50);
    if (hAudio == NULL)
    {
        printf("WARNING: No audio device");
        return -1;
    }
    audio_playwavfile(hAudio, "test.wav");

    // HACK: Rather than sleeping for the length of the audio...
    // we should modify audio_playwavfile to have a callback to be called when the audio is done playing. 
    long numMilliSeconds = (fileSize * 8) / (256);
    ThreadAPI_Sleep(numMilliSeconds);
#endif

    IoBufferDelete(msg->ioBuffer);

    free(msg);

    return 1;
}

void OnAudioStreamStart(UspHandle uspHandle, void* context, const UspMsgAudioStreamStart* message)
{
    UNUSED(uspHandle);
    UNUSED(context);

    UspMsgAudioStreamStart* msgCopy = (UspMsgAudioStreamStart*)malloc(sizeof(UspMsgAudioStreamStart));
    memcpy(msgCopy, message, sizeof(UspMsgAudioStreamStart));
    
    printf("Response: First Audio Chunk received.\n");

    if (ThreadAPI_Create(&ttsThreadHandle, TTSRenderLoop, msgCopy) != THREADAPI_OK)
    {
        printf("Failed to start audio render thread.\n");
    }
}

#define AUDIO_BYTES_PER_SECOND (16000*2) //16KHz, 16bit PCM

int main(int argc, char* argv[])
{
    UspHandle handle;
    UspResult ret;
    void* context = NULL;
    UspCallbacks testCallbacks;
    UspEndpointType endpoint = USP_ENDPOINT_BING_SPEECH;
    UspRecognitionMode mode = USP_RECO_MODE_INTERACTIVE;
    bool isAudioMessage = true;
    int curArg = 0;
    char* authData = NULL;
    char* messagePath = NULL;

    testCallbacks.version = (uint16_t)USP_CALLBACK_VERSION;
    testCallbacks.size = sizeof(testCallbacks);
    testCallbacks.OnError = OnError;
    testCallbacks.onSpeechEndDetected = OnSpeechEndDetected;
    testCallbacks.onSpeechHypothesis = OnSpeechHypothesis;
    testCallbacks.onSpeechPhrase = OnSpeechPhrase;
    testCallbacks.onSpeechFragment = OnSpeechFragment;
    testCallbacks.onSpeechStartDetected = OnSpeechStartDetected;
    testCallbacks.onTurnEnd = OnTurnEnd;
    testCallbacks.onTurnStart = OnTurnStart;

    if (argc < 4)
    {
        printf("Usage: uspclientconsole message_type(audio/message:[path]) file tts(true/false) authentication endpoint_type(speech/cris/cdsdk/url) mode(interactive/conversation/dictation) language output(simple/detailed) user-defined-messages");
        exit(1);
    }

    curArg++;
    if (strstr(argv[curArg], "message") != NULL)
    {
        isAudioMessage = false;
        char *path = strchr(argv[curArg], ':');
        if (path == NULL)
        {
            //default message path if not specified
            messagePath = "message";
        }
        else
        {
            messagePath = ++path;
        }

    }
    else if (strcmp(argv[curArg], "audio") == 0)
    {
        isAudioMessage = true;
    }
    else
    {
        printf("unknown message type: %s\n", argv[curArg]);
        exit(1);
    }

    // Read input file.
    curArg++;
    FILE *data = fopen(argv[curArg], isAudioMessage ? "rb" : "rt");
    if (data == NULL)
    {
        printf("Error: open file %s failed", argv[curArg]);
        exit(1);
    }

    curArg++;
    if (strcmp(argv[curArg], "true") == 0)
    {
        testCallbacks.onAudioStreamStart = OnAudioStreamStart;
    }

    curArg++;
    if (argc > curArg)
    {
        authData = argv[curArg];
    }

    curArg++;
    // Set service endpoint type.
    if (argc > curArg)
    {
        if (strcmp(argv[curArg], "speech") == 0)
        {
            endpoint = USP_ENDPOINT_BING_SPEECH;
        }
        else if (strcmp(argv[curArg], "cris") == 0)
        {
            endpoint = USP_ENDPOINT_CRIS;
        }
        else if (strcmp(argv[curArg], "cdsdk") == 0)
        {
            endpoint = USP_ENDPOINT_CDSDK;
        }
        else if (strcmp(argv[curArg], "url") == 0)
        {
            endpoint = USP_ENDPOINT_UNKNOWN;
            if (argc != curArg + 2)
            {
                printf("No URL specified or too many parameters.\n");
                exit(1);
            }
        }
        else
        {
            printf("unknown service endpoint type: %s\n", argv[curArg]);
            exit(1);
        }
    }

    curArg++;
    // Set recognition mode.
    if (argc > curArg)
    {
        if (endpoint != USP_ENDPOINT_UNKNOWN)
        {
            if (strcmp(argv[curArg], "interactive") == 0)
            {
                mode = USP_RECO_MODE_INTERACTIVE;
            }
            else if (strcmp(argv[curArg], "conversation") == 0)
            {
                mode = USP_RECO_MODE_CONVERSATION;
            }
            else if (strcmp(argv[curArg], "dictation") == 0)
            {
                mode = USP_RECO_MODE_DICTATION;
            }
            else
            {
                printf("unknown reco mode: %s\n", argv[curArg]);
                exit(1);
            }
        }
    }

    // Create USP handle.
    if (endpoint == USP_ENDPOINT_UNKNOWN)
    {
        printf("Open USP by URL.\n");
        ret = UspInitByUrl(argv[curArg], &testCallbacks, context, &handle);
    }
    else
    {
        printf("Open USP by service type and mode.");
        ret = UspInit(endpoint, mode, &testCallbacks, context, &handle);
    }

    if (ret != USP_SUCCESS)
    {
        printf("Error: open UspHandle failed (error=0x%x).\n", ret);
        exit(1);
    }

    // Set Authentication.
    if (authData != NULL)
    {
        UspAuthenticationType authType = (endpoint == USP_ENDPOINT_CDSDK) ? 
            USP_AUTHENTICATION_SEARCH_DELEGATION_RPS_TOKEN : USP_AUTHENTICATION_SUBSCRIPTION_KEY;

        if ((ret = UspSetAuthentication(handle, authType, authData)) != USP_SUCCESS)
        {
            printf("Error: set authentication data failed. (error=0x%x).\n", ret);
            exit(1);
        }
    }

    curArg++;
    // Set language.
    if (argc > curArg)
    {
        if (endpoint == USP_ENDPOINT_CRIS)
        {
            if (UspSetModelId(handle, argv[curArg]) != USP_SUCCESS)
            {
                printf("Set model id for CRIS failed.\n");
                exit(1);
            }
        }
        else
        {
            if (UspSetLanguage(handle, argv[curArg]) != USP_SUCCESS)
            {
                printf("Set language to %s failed.\n", argv[curArg]);
                exit(1);
            }
        }
    }

    curArg++;
    // Set output format if needed.
    if (argc > curArg)
    {
        if (strcmp(argv[curArg], "detailed") == 0)
        {
            if (UspSetOutputFormat(handle, USP_OUTPUT_DETAILED) != USP_SUCCESS)
            {
                printf("Set output format to detailed failed.\n");
                exit(1);
            }
        }
        else if (strcmp(argv[curArg], "simple") == 0)
        {
            if (UspSetOutputFormat(handle, USP_OUTPUT_SIMPLE) != USP_SUCCESS)
            {
                printf("Set output format to simple failed.\n");
                exit(1);
            }
        }
        else
        {
            printf("unknown output format: %s\n", argv[curArg]);
            exit(1);
        }
    }

    curArg++;
    if (argc > curArg)
    {
        for (int argIndex = curArg; argIndex < argc; argIndex++)
        {
            printf("Register user-defined response message: %s\n", argv[argIndex]);
            if (UspRegisterUserMessage(handle, argv[argIndex], OnUserMessage) != USP_SUCCESS)
            {
                printf("Failed to register user-defined response message: %s\n", argv[argIndex]);
                exit(1);
            }
        }
    }

    // Connect to service
    if ((ret = UspConnect(handle)) != USP_SUCCESS)
    {
        printf("Error: connect to service failed (error=0x%x).\n", ret);
        exit(1);
    }

    turnEnd = false;

    // Send data to service
    uint8_t *buffer;
    size_t bytesToWrite;
    size_t bytesWritten;
    size_t totalBytesWritten = 0;
    size_t fileSize;

    fseek(data, 0L, SEEK_END);
    fileSize = ftell(data);
    rewind(data);

    if (isAudioMessage)
    {
        size_t chunkSize = AUDIO_BYTES_PER_SECOND * 2 / 5; // 2x real-time, 5 chunks per second.
        buffer = malloc(chunkSize);
        while (!feof(data))
        {
            bytesToWrite = fread(buffer, sizeof(uint8_t), chunkSize, data);
            if (UspWriteAudio(handle, buffer, bytesToWrite, &bytesWritten) != USP_SUCCESS)
            {
                printf("%s: Error: send data to service failed (error=0x%x).\n", __FUNCTION__, ret);
                UspClose(handle);
                exit(1);
            }
            else
            {
                totalBytesWritten += bytesWritten;
                if (bytesToWrite != bytesWritten)
                {
                    printf("%s: Error: the number of bytes sent to service (%zu) does not match expected (%zu).\n", __FUNCTION__, bytesWritten, bytesToWrite);
                    UspClose(handle);
                    exit(1);
                }
            }

            // Sleep to simulate real-time traffic
            ThreadAPI_Sleep(200);
        }

        // Send End of Audio to service to close the session.
        if (feof(data))
        {
            if ((ret = UspFlushAudio(handle)) != USP_SUCCESS)
            {
                printf("%s: Error: flushing audio failed (error=0x%x).\n", __FUNCTION__, ret);
                UspClose(handle);
                exit(1);
            }
        }
    }
    else
    {
        buffer = malloc(fileSize);
        bytesToWrite = fread(buffer, sizeof(uint8_t), fileSize, data);
        if (UspSendMessage(handle, messagePath, buffer, bytesToWrite) != USP_SUCCESS)
        {
            printf("%s: Error: send data to service failed (error=0x%x).\n", __FUNCTION__, ret);
            UspClose(handle);
            exit(1);
        }
        totalBytesWritten = fileSize;
    }

    if (totalBytesWritten != fileSize)
    {
        printf("%s: Error: the total number of bytes sent (%zu) does not match the file size (%zu).\n", __FUNCTION__, totalBytesWritten, fileSize);
        exit(1);
    }
    else
    {
        printf("%s: Totally send %zu bytes of data.\n", __FUNCTION__, totalBytesWritten);
    }

    // Wait for end of recognition.
    while (!turnEnd)
    {
        ThreadAPI_Sleep(500);
    }

    UspClose(handle);

    if (ttsThreadHandle != NULL)
    {
        ThreadAPI_Join(ttsThreadHandle, NULL);
    }

    free(buffer);
    fclose(data);
}
