//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspclientconsole.c: A console application for testing USP client library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "azure_c_shared_utility/threadapi.h"
#include "usp.h"

#define UNUSED(x) (void)(x)

bool turnEnd = false;

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

void OnError(UspHandle handle, void* context, UspResult error)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: On Error: 0x%x.\n", error);
    turnEnd = true;
}

void OnUserMessage(UspHandle uspHandle, const char* path, const char* contentType, const unsigned char* buffer, size_t size, void* context)
{
    UNUSED(uspHandle);
    UNUSED(context);
    printf("Response: User defined message. Path: %s, contentType: %s, size: %zu, content: %s.\n", path, contentType, size, buffer);
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
    int curArg = 0;
    char* authData = NULL;

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

    if (argc < 2)
    {
        printf("Usage: uspclientconsole audio_file authentication endpoint_type(speech/cris) mode(interactive/conversation/dictation) language output(simple/detailed) user-defined-messages");
        exit(1);
    }

    // Read audio file.
    FILE *audio = fopen(argv[1], "rb");
    if (audio == NULL)
    {
        printf("Error: open file %s failed", argv[1]);
        exit(1);
    }

    curArg = 2;
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

    // Create USP handle.
    if ((ret = UspInit(endpoint, mode, &testCallbacks, context, &handle)) != USP_SUCCESS)
    {
        printf("Error: open UspHandle failed (error=0x%x).\n", ret);
        exit(1);
    }

    // Set Authentication.
    if (authData != NULL)
    {
        if ((ret = UspSetAuthentication(handle, USP_AUTHENTICATION_SUBSCRIPTION_KEY, authData)) != USP_SUCCESS)
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

    // Conenct to service
    if ((ret = UspConnect(handle)) != USP_SUCCESS)
    {
        printf("Error: connect to service failed (error=0x%x).\n", ret);
        exit(1);
    }

    turnEnd = false;

    // Send audio to service
    uint8_t *buffer;
    size_t bytesToWrite;
    size_t bytesWritten;
    size_t chunkSize = AUDIO_BYTES_PER_SECOND * 2 / 5; // 2x real-time, 5 chunks per second.
    size_t totalBytesWritten = 0;
    size_t fileSize;

    fseek(audio, 0L, SEEK_END);
    fileSize = ftell(audio);
    rewind(audio);

    buffer = malloc(chunkSize);
    while (!feof(audio))
    {
        bytesToWrite = fread(buffer, sizeof(uint8_t), chunkSize, audio);

        if ((ret = UspWriteAudio(handle, buffer, bytesToWrite, &bytesWritten)) != USP_SUCCESS)
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
    if (feof(audio))
    {
        if ((ret = UspFlushAudio(handle)) != USP_SUCCESS)
        {
            printf("%s: Error: flushing audio failed (error=0x%x).\n", __FUNCTION__, ret);
            UspClose(handle);
            exit(1);
        }
    }

    if (totalBytesWritten != fileSize)
    {
        printf("%s: Error: the total number of bytes sent (%zu) does not match the file size (%zu).\n", __FUNCTION__, totalBytesWritten, fileSize);
        exit(1);
    }
    else
    {
        printf("%s: Totally send %zu bytes of audio.\n", __FUNCTION__, totalBytesWritten);
    }

    // Wait for end of recognition.
    while (!turnEnd)
    {
        ThreadAPI_Sleep(500);
    }

    UspClose(handle);

    free(buffer);
    fclose(audio);
}
