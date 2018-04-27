//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

// <toplevel>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
// </toplevel>

// Speech recognition using microphone.
// <SpeechRecognitionWithMicrophone>
void SpeechRecognitionWithMicrophone()
{
    // Creates an instance of a speech factory with specified
    // subscription key. Replace with your own subscription key.
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();
    wcout << L"Say something...\n";

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << (int)result->Reason << L"-" << result->Text << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
    }
}
// </SpeechRecognitionWithMicrophone>

// Speech recognition using file input
// <SpeechRecognitionWithFile>
void SpeechRecognitionWithFile()
{
    // Creates an instance of a speech factory with specified
    // subscription key. Replace with your own subscription key.
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto recognizer = factory->CreateSpeechRecognizerWithFileInput(L"YourAudioFileName");

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << (int)result->Reason << L"-" << result->Text << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
    }
}
// </SpeechRecognitionWithFile>

// Speech recognition using a customized model.
// <SpeechRecognitionUsingCustomizedModel>
void SpeechRecognitionUsingCustomizedModel()
{
    // Creates an instance of a speech factory with specified
    // subscription key. Replace with your own subscription key from
    // https://www.cris.ai/.
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();

    // Set the deployment id of your customized model
    // Replace with your own CRIS deployment id.
    recognizer->SetDeploymentId(L"YourDeploymentId");
    wcout << L"Say something...\n";

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << (int)result->Reason << L"-" << result->Text << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
    }
}
// </SpeechRecognitionUsingCustomizedModel>

// Defines event handlers for different events.
// <SpeechContinuousRecognitionUsingEvents>
static void OnPartialResult(const SpeechRecognitionEventArgs& e)
{
    wcout << L"IntermediateResult:" << e.Result.Text << '\n';
}

static void OnFinalResult(const SpeechRecognitionEventArgs& e)
{
    wcout << L"FinalResult: status:" << (int)e.Result.Reason << L". Text: " << e.Result.Text << '\n';
}

static void OnNoMatch(const SpeechRecognitionEventArgs& e)
{
    wcout << L"NoMatch:" << (int)e.Result.Reason << '\n';
}

static void OnCanceled(const SpeechRecognitionEventArgs& e)
{
    wcout << L"Canceled:" << (int)e.Result.Reason << L"- " << e.Result.Text << '\n';
}

// Continuous speech recognition.
void SpeechContinuousRecognitionUsingEvents()
{
    // Creates an instance of a speech factory with specified
    // subscription key. Replace with your own subscription key.
    auto factory = SpeechFactory::FromSubscription(L"92069ee289b84e5594a9564ab77ed2ba");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();

    // Subscribes to events.
    recognizer->IntermediateResult.Connect(&OnPartialResult);
    recognizer->FinalResult.Connect(&OnFinalResult);
    recognizer->Canceled.Connect(&OnCanceled);
    recognizer->NoMatch.Connect(&OnNoMatch);

    wcout << L"Say something...\n";

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    wcout << L"Press any key to stop\n";
    string s;
    getline(cin, s);

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();

    // Unsubscribes from events.
    recognizer->IntermediateResult.Disconnect(&OnPartialResult);
    recognizer->FinalResult.Disconnect(&OnFinalResult);
    recognizer->Canceled.Disconnect(&OnCanceled);
    recognizer->NoMatch.Disconnect(&OnNoMatch);
}
// </SpeechContinuousRecognitionUsingEvents>
