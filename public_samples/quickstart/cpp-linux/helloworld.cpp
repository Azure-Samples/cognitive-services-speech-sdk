//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#include <iostream> // cin, cout
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

void recognizeSpeech() {
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

    // Creates a speech recognizer
    auto recognizer = factory->CreateSpeechRecognizer();
    cout << "Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto future = recognizer->RecognizeAsync();
    auto result = future.get();

    // Checks result.
    if (result->Reason != Reason::Recognized) {
        cout << "Recognition Status:" << int(result->Reason) << ". ";
        if (result->Reason == Reason::Canceled)
        {
            cout << "There was an error, reason: " << result->ErrorDetails << std::endl;
        }
        else
        {
            cout << "No speech could be recognized.\n";
        }
    }
    else {
        cout << "We recognized: " << result->Text << std::endl;
    }
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    recognizeSpeech();
    return 0;
}
// </code>
