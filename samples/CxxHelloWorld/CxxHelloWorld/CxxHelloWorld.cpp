//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <speechapi_cxx.h>

using namespace std;

int wmain(int argc, wchar_t **argv)
{
    auto usage = L"Usage: CxxHelloWorld <subscriptionKey> [<path-to-file>]\n";

    if (argc < 2)
    {
        // In Visual Studio, right-click the CxxHelloWorld project in the Solution Explorer and add
        // your subscription key to Properties > Debugging > Command Arguments.
        wcout << "Error: missing speech subscription key.\n" << usage;
        exit(1);
    }

    wstring subscriptionKey{ argv[1] };
    wstring filename;

    if (3 < argc)
    {
        wcout << "Error: too many parameters.\n" << usage;
        exit(1);
    }

    if (argc == 3)
    {
        filename = wstring(argv[2]);
    }

    auto factory = Microsoft::CognitiveServices::Speech::Recognition::RecognizerFactory::GetDefault();
    factory->SetSubscriptionKey(subscriptionKey);

    auto recognizeFromFile = !filename.empty();

    auto recognizer = recognizeFromFile ? factory->CreateSpeechRecognizerWithFileInput(filename) : factory->CreateSpeechRecognizer();

    wcout << (recognizeFromFile ? L"Recognizing from file...\n" : L"Say something...\n");
    auto result = recognizer->RecognizeAsync().get();

    int exitCode = 0;

    if (result->Reason != Microsoft::CognitiveServices::Speech::Recognition::Reason::Recognized) {
        exitCode = 1;
        wcout << L"There was an error, reason " << int(result->Reason) << L" - " << result->Text << '\n';
    }
    else {
        wcout << L"We recognized: " << result->Text << '\n';
    }

    return exitCode;
}
