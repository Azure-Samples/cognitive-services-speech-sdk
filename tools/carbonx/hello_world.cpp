// Copyright (c) Microsoft. All rights reserved.
//
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// hello_world.cpp: Sample #1 - Hello World!!
//

#include "stdafx.h"
#include "carbon_test_console.h"


void CarbonTestConsole::Sample_HelloWorld()
{
    auto recognizer = RecognizerFactory::CreateSpeechRecognizer();

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}

void CarbonTestConsole::Sample_HelloWorld_WithEvents()
{
    auto recognizer = RecognizerFactory::CreateSpeechRecognizer();

    recognizer->IntermediateResult += [&](const SpeechRecognitionEventArgs& e) {
        ConsoleWriteLine(L"IntermediateResult: text=%ls", e.Result.Text.c_str());
    };

    ConsoleWriteLine(L"Say something...");
    auto result = recognizer->RecognizeAsync().get();

    ConsoleWriteLine(L"You said:\n\n    '%ls'", result->Text.c_str());
}
