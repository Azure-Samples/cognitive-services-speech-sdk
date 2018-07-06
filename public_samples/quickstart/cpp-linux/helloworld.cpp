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

    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Creates a speech recognizer
    auto recognizer = factory->CreateSpeechRecognizer();
    cout << "Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto future = recognizer->RecognizeAsync();
    auto result = future.get();
    auto resultText = cvt.to_bytes(result->Text);
    auto errorDetails = cvt.to_bytes(result->ErrorDetails);

    // Checks result.
    if (result->Reason != Reason::Recognized) {
        cout << "Recognition Status:" << int(result->Reason) << ". ";
        if (result->Reason == Reason::Canceled)
        {
            cout << "There was an error, reason: " << errorDetails << std::endl;
        }
        else
        {
            cout << "No speech could be recognized.\n";
        }
    }
    else {
        cout << "We recognized: " << resultText << std::endl;
    }
}

int main(int argc, char **argv) {
    recognizeSpeech();

    cout << "Please press a key to exit.\n";
    cin.get();
    return 0;
}
// </code>
