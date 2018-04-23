//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#include <codecvt> // codecvt_utf8_utf16
#include <locale> // wstring_convert
#include <iostream> // cin, cout
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

void recognizeSpeech() {
    wstring_convert<codecvt_utf8_utf16<wchar_t>> cvt;

    wstring subscriptionKey{ L"<Please replace with your subscription key>" };

#ifdef FROM_FILE
    wstring filename = L"/path/to/inputfile.wav";
#endif

    auto factory = Recognition::RecognizerFactory::GetDefault();

    factory->SetSubscriptionKey(subscriptionKey);

#ifndef FROM_FILE
    auto recognizer = factory->CreateSpeechRecognizer();
    cout << "Say something...\n";
#else
    auto recognizer = factory->CreateSpeechRecognizerWithFileInput(filename);
    cout << "Recognizing from file...\n";
#endif

    auto future = recognizer->RecognizeAsync();
    auto result = future.get();
    auto resultText = cvt.to_bytes(result->Text);

    if (result->Reason != Recognition::Reason::Recognized) {
        cout << "There was an error, reason " << int(result->Reason) << " - " << resultText << '\n';
    }
    else {
        cout << "We recognized: " << resultText << '\n';
    }
    cout << "Please press a key to continue.\n";
    cin.get();
}

int main(int argc, char **argv) {
    recognizeSpeech();
    return 0;
}
// </code>
