//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspclientconsole.c: A console application for testing USP client library.
//

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <Windows.h>
#include "usp.h"

boolean turnEnd = FALSE;

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
    printf("Response: Speech.StartDetected message. Speech starts at offset %u (100ns).\n", message->offset);
}

void OnSpeechEndDetected(UspHandle handle, void* context, UspMsgSpeechEndDetected *message)
{
    printf("Response: Speech.EndDetected message. Speech ends at offset %u (100ns)\n", message->offset);
}

void OnSpeechHypothesis(UspHandle handle, void* context, UspMsgSpeechHypothesis *message)
{
    printf("Response: Speech.Hypothesis message. Text: %S, starts at offset %u, with duration %u (100ns).\n", message->text, message->offset, message->duration);
}

void OnSpeechPhrase(UspHandle handle, void* context, UspMsgSpeechPhrase *message)
{
    printf("Response: Speech.Phrase message. Status: %s, Text: %S, starts at %u, with duration %u (100ns).\n", recognitionStatusToText[message->recognitionStatus], message->displayText, message->offset, message->duration);
}

void OnTurnStart(UspHandle handle, void* context, UspMsgTurnStart *message)
{
    printf("Response: Turn.Start message. Context.ServiceTag: %S\n", message->contextServiceTag);
}

void OnTurnEnd(UspHandle handle, void* context, UspMsgTurnEnd *message)
{
    printf("Response: Turn.End message.\n");
    turnEnd = TRUE;
}

void OnError(UspHandle handle, void* context, UspResult error)
{
    printf("Response: On Error: 0x%x.\n", error);
}

#define MAX_AUDIO_SIZE_IN_BYTE (1024*1024)

int main(int argc, char* argv[])
{
    UspHandle handle;
    void* context = NULL;
    UspCallbacks testCallbacks;
    uint8_t *buffer = malloc(MAX_AUDIO_SIZE_IN_BYTE);
    size_t bytesRead;

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
    }
    
    FILE *audio = fopen(argv[1], "rb");
    if (audio == NULL)
    {
        printf("Error: open file %s failed", argv[1]);
    }

    bytesRead = fread(buffer, sizeof(uint8_t), MAX_AUDIO_SIZE_IN_BYTE, audio);

    turnEnd = FALSE;

    UspInitialize(&handle, &testCallbacks, context);

    UspWrite(handle, buffer, bytesRead);

    while (!turnEnd)
    {
        UspRun(handle);

        Sleep(200);
    }

    UspShutdown(handle);

    free(buffer);
    fclose(audio);
}
