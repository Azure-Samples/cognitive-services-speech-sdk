//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

int wmain(int argc, wchar_t **argv)
{
    auto usage = L"Usage: helloworld <subscriptionKey> <service-region> [<path-to-wav-file>]\n";

    if (argc < 3)
    {
        // In Visual Studio, right-click the helloworld project in the Solution Explorer and add
        // your subscription key to Properties > Debugging > Command Arguments.
        wcout << "Error: missing parameters.\n" << usage;
        exit(1);
    }

    wstring subscriptionKey{ argv[1] };
    wstring region{ argv[2] };
    wstring filename;

    if (4 < argc)
    {
        wcout << "Error: too many parameters.\n" << usage;
        exit(1);
    }

    if (argc == 4)
    {
        filename = wstring(argv[3]);
    }

    auto factory = SpeechFactory::FromSubscription(subscriptionKey, region);

    auto recognizeFromFile = !filename.empty();

    auto recognizer = recognizeFromFile ? factory->CreateSpeechRecognizerWithFileInput(filename) : factory->CreateSpeechRecognizer();

    wcout << (recognizeFromFile ? L"Recognizing from file...\n" : L"Say something...\n");
    auto result = recognizer->RecognizeAsync().get();

    int exitCode = 0;

    if (result->Reason != Reason::Recognized) {
        exitCode = 1;
        wcout << L"There was an error, reason " << int(result->Reason) << L" - " << result->ErrorDetails << '\n';
    }
    else {
        wcout << L"We recognized: " << result->Text << '\n';
    }

    return exitCode;
}
