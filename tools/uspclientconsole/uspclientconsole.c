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
#include "usp.h"



bool turnEnd = false;

char* recognitionStatusToText[] =
{
    "Success",  // RECOGNITON_SUCCESS
    "No Match", // RECOGNITION_NO_MATCH
    "Initial Silence Timeout", //RECOGNITION_INITIAL_SILENCE_TIMEOUT
    "Babble Timeout", // RECOGNITION_BABBLE_TIMEOUT
    "Error", // RECOGNITION_ERROR
};

void OnSpeechStartDetected(UspHandle handle, void* context, UspMsgSpeechStartDetected *message)
{
    printf("Response: Speech.StartDetected message. Speech starts at offset %llu (100ns).\n", message->offset);
}

void OnSpeechEndDetected(UspHandle handle, void* context, UspMsgSpeechEndDetected *message)
{
    printf("Response: Speech.EndDetected message. Speech ends at offset %llu (100ns)\n", message->offset);
}

void OnSpeechHypothesis(UspHandle handle, void* context, UspMsgSpeechHypothesis *message)
{
    printf("Response: Speech.Hypothesis message. Text: %ls, starts at offset %llu, with duration %llu (100ns).\n", message->text, message->offset, message->duration);
}

void OnSpeechPhrase(UspHandle handle, void* context, UspMsgSpeechPhrase *message)
{
    printf("Response: Speech.Phrase message. Status: %s, Text: %ls, starts at %llu, with duration %llu (100ns).\n", recognitionStatusToText[message->recognitionStatus], message->displayText, message->offset, message->duration);
}

void OnTurnStart(UspHandle handle, void* context, UspMsgTurnStart *message)
{
    printf("Response: Turn.Start message. Context.ServiceTag: %S\n", message->contextServiceTag);
}

void OnTurnEnd(UspHandle handle, void* context, UspMsgTurnEnd *message)
{
    printf("Response: Turn.End message.\n");
    turnEnd = true;
}

void OnError(UspHandle handle, void* context, UspResult error)
{
    printf("Response: On Error: 0x%x.\n", error);
}

#define MAX_AUDIO_SIZE_IN_BYTE (1024*1024)

int main(int argc, char* argv[])
{
    UspHandle handle;
    UspResult ret;
    void* context = NULL;
    UspCallbacks testCallbacks;
    uint8_t *buffer = malloc(MAX_AUDIO_SIZE_IN_BYTE);
    size_t bytesRead;
    size_t bytesWritten;
    UspEndpointType endpoint = USP_ENDPOINT_BING_SPEECH;
    UspRecognitionMode mode = USP_RECO_MODE_INTERACTIVE;

    testCallbacks.version = (uint16_t)USP_VERSION;
    testCallbacks.size = sizeof(testCallbacks);
    testCallbacks.OnError = OnError;
    testCallbacks.onSpeechEndDetected = OnSpeechEndDetected;
    testCallbacks.onSpeechHypothesis = OnSpeechHypothesis;
    testCallbacks.onSpeechPhrase = OnSpeechPhrase;
    testCallbacks.onSpeechStartDetected = OnSpeechStartDetected;
    testCallbacks.onTurnEnd = OnTurnEnd;
    testCallbacks.onTurnStart = OnTurnStart;

    if (argc < 2)
    {
        printf("Usage: uspclientconsole audio_file");
        exit(1);
    }

    // Read audio file.
    FILE *audio = fopen(argv[1], "rb");
    if (audio == NULL)
    {
        printf("Error: open file %s failed", argv[1]);
        exit(1);
    }
    bytesRead = fread(buffer, sizeof(uint8_t), MAX_AUDIO_SIZE_IN_BYTE, audio);

    turnEnd = false;

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
    }

    // Send audio to service
    printf("Info: send audio with %d bytes for recognition\n", (uint32_t)bytesRead);
    if ((ret = UspWrite(handle, buffer, bytesRead, &bytesWritten)) != USP_SUCCESS)
    {
        printf("Error: send data to service failed (error=0x%x).\n", ret);
    }
    else
    {
        printf("Info: successfully sent %d bytes.\n", (uint32_t)bytesWritten);
    }

    // Wait for end of recognition.
    while (!turnEnd)
    {
        ThreadAPI_Sleep(2000);
    }

    UspClose(handle);

    free(buffer);
    fclose(audio);
}
