//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#include <iostream> // cin, cout
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

void synthesizeSpeech()
{
    // Creates an instance of a speech config with specified endpoint and subscription key.
    // Replace with your own endpoint and subscription key.
    auto config = SpeechConfig::FromEndpoint("https://YourServiceRegion.api.cognitive.microsoft.com", "YourSubscriptionKey");

    // Set the voice name, refer to https://aka.ms/speech/voices/neural for full list.
    config->SetSpeechSynthesisVoiceName("en-US-AriaNeural");

    // Creates a speech synthesizer using the default speaker as audio output. The default spoken language is "en-us".
    auto synthesizer = SpeechSynthesizer::FromConfig(config);

    // Receive a text from console input and synthesize it to speaker.
    cout << "Type some text that you want to speak..." << std::endl;
    cout << "> ";
    std::string text;
    getline(cin, text);

    auto result = synthesizer->SpeakTextAsync(text).get();

    // Checks result.
    if (result->Reason == ResultReason::SynthesizingAudioCompleted)
    {
        cout << "Speech synthesized to speaker for text [" << text << "]" << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = SpeechSynthesisCancellationDetails::FromResult(result);
        cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            cout << "CANCELED: ErrorDetails=[" << cancellation->ErrorDetails << "]" << std::endl;
            cout << "CANCELED: Did you update the subscription info?" << std::endl;
        }
    }

    // This is to give some time for the speaker to finish playing back the audio
    cout << "Press enter to exit..." << std::endl;
    cin.get();
}

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    synthesizeSpeech();
    return 0;
}
// </code>
