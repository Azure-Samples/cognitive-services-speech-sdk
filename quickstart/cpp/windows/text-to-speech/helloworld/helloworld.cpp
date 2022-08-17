//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include <iostream> // cin, cout
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

auto YourSubscriptionKey = "YourSubscriptionKey";
auto YourServiceRegion = "YourServiceRegion";

int main()
{
    auto speechConfig = SpeechConfig::FromSubscription(YourSubscriptionKey, YourServiceRegion);

    // The language of the voice that speaks.
    speechConfig->SetSpeechSynthesisVoiceName("en-US-JennyNeural");

    auto synthesizer = SpeechSynthesizer::FromConfig(speechConfig);

    // Get text from the console and synthesize to the default speaker.
    cout << "Enter some text that you want to speak >" << std::endl;
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
            cout << "CANCELED: Did you set the speech resource key and region values?" << std::endl;
        }
    }

    cout << "Press enter to exit..." << std::endl;
    cin.get();
}
// </code>
