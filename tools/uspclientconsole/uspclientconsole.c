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
#include <stdbool.h>
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
    printf("Response: Speech.StartDetected message. Speech starts at offset %llu.\n", message->offset);
}

void OnSpeechEndDetected(UspHandle handle, void* context, UspMsgSpeechEndDetected *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.EndDetected message. Speech ends at offset %llu\n", message->offset);
}

void OnSpeechHypothesis(UspHandle handle, void* context, UspMsgSpeechHypothesis *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.Hypothesis message. Text: %ls, starts at offset %llu, with duration %llu.\n", message->text, message->offset, message->duration);
}

void OnSpeechPhrase(UspHandle handle, void* context, UspMsgSpeechPhrase *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.Phrase message. Status: %s, Text: %ls, starts at %llu, with duration %llu.\n", recognitionStatusToText[message->recognitionStatus], message->displayText, message->offset, message->duration);
}

void OnSpeechFragment(UspHandle handle, void* context, UspMsgSpeechFragment *message)
{
    UNUSED(handle);
    UNUSED(context);
    printf("Response: Speech.Fragment message. Text: %ls, starts at %llu, with duration %llu.\n", message->text, message->offset, message->duration);
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
        printf("Usage: uspclientconsole audio_file endpoint_type(speech/cris) mode(interactive/conversation/dictation) language output(simple/detailed)");
        exit(1);
    }

    // Read audio file.
    FILE *audio = fopen(argv[1], "rb");
    if (audio == NULL)
    {
        printf("Error: open file %s failed", argv[1]);
        exit(1);
    }

    // Set service endpoint type.
    if (argc > 2)
    {
        if (strcmp(argv[2], "speech") == 0)
        {
            endpoint = USP_ENDPOINT_BING_SPEECH;
        }
        else if (strcmp(argv[2], "cris") == 0)
        {
            endpoint = USP_ENDPOINT_CRIS;
        }
        else
        {
            printf("unknown service endpoint type: %s\n", argv[2]);
            exit(1);
        }
    }

    // Set recognition mode.
    if (argc > 3)
    {
        if (strcmp(argv[3], "interactive") == 0)
        {
            mode = USP_RECO_MODE_INTERACTIVE;
        }
        else if (strcmp(argv[3], "conversation") == 0)
        {
            mode = USP_RECO_MODE_CONVERSATION;
        }
        else if (strcmp(argv[3], "dictation") == 0)
        {
            mode = USP_RECO_MODE_DICTATION;
        }
        else
        {
            printf("unknown reco mode: %s\n", argv[3]);
            exit(1);
        }
    }

    // Create USP handle.
    if ((ret = UspOpen(endpoint, mode, &testCallbacks, context, &handle)) != USP_SUCCESS)
    {
        printf("Error: open UspHandle failed (error=0x%x).\n", ret);
        exit(1);
    }

    // Set language.
    if (argc > 4)
    {
        if (UspSetOption(handle, USP_OPTION_LANGUAGE, argv[4]) != USP_SUCCESS)
        {
            printf("Set language to %s failed.\n", argv[4]);
            exit(1);
        }
    }

    // Set output format if needed.
    if (argc > 5)
    {
        if (strcmp(argv[5], "detailed") == 0)
        {
            if (UspSetOption(handle, USP_OPTION_OUTPUT_FORMAT, "detailed") != USP_SUCCESS)
            {
                printf("Set output format to detailed failed.\n");
                exit(1);
            }
        }
        else if (strcmp(argv[5], "simple") == 0)
        {
            if (UspSetOption(handle, USP_OPTION_OUTPUT_FORMAT, "simple") != USP_SUCCESS)
            {
                printf("Set output format to simple failed.\n");
                exit(1);
            }
        }
        else
        {
            printf("unknown output format: %s\n", argv[5]);
            exit(1);
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

        if ((ret = UspWrite(handle, buffer, bytesToWrite, &bytesWritten)) != USP_SUCCESS)
        {
            printf("Error: send data to service failed (error=0x%x).\n", ret);
            UspClose(handle);
            exit(1);
        }
        else
        {
            totalBytesWritten += bytesWritten;
            printf("Info: successfully sent %zu bytes (expected=%zu). Totally: %zu\n", bytesWritten, bytesToWrite, totalBytesWritten);
            if (bytesToWrite != bytesWritten)
            {
                printf("Error: the number of bytes sent to service (%zu) does not match expected (%zu).\n", bytesWritten, bytesToWrite);
                exit(1);
            }
        }

        // Sleep to simulate real-time traffic
        ThreadAPI_Sleep(200);
    }

    if (totalBytesWritten != fileSize)
    {
        printf("Error: the total number of bytes sent (%zu) does not match the file size (%zu).\n", totalBytesWritten, fileSize);
        exit(1);
    }
    else
    {
        printf("Totally send %zu bytes of audio.\n", totalBytesWritten);
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
