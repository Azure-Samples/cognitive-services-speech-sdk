//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech::Recognition;
using namespace Microsoft::CognitiveServices::Speech::Recognition::Speech;

wstring ToString(Reason reason);

// Speech recognition using microphone.
void RecognitionWithMicrophone()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Sets subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();
    wcout << L"Say something...\n";

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << int(result->Reason) << L"-" << result->Text << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
    }
}

// Speech recognition using file input
void RecognitionWithFile()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Set subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Creates a speech recognizer using file as audio input.
    // Replaces with your own audio file name.
    auto recognizer = factory->CreateSpeechRecognizerWithFileInput(L"YourAudioFileName");

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << ToString(result->Reason) << L"-" << result->Text << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
    }
}

// Speech recognition using a customized model.
void RecognitionUsingCustomizedModel()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Sets subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();

    // Set the deployment id of your customizd model
    // Replaces with your own deployment id.
    recognizer->SetCustomSpeechModel(L"YourDeploymentId");
    wcout << L"Say something...\n";

    // Starts recognition. It returns when the first utterance has been recognized.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"There was an error, reason " << int(result->Reason) << L"-" << result->Text << '\n';
    }
    else
    {
        wcout << L"We recognized: " << result->Text << '\n';
    }
}

// Defines event handlers for different events.
void OnPartialResult(const SpeechRecognitionEventArgs& e)
{
    wcout << L"IntermediateResult:" << e.Result.Text << '\n';
}

void OnFinalResult(const SpeechRecognitionEventArgs& e)
{
    wcout << L"IntermediateResult: status:" << ToString(e.Result.Reason) << L". Text: " << e.Result.Text << '\n';
}

void OnNoMatch(const SpeechRecognitionEventArgs& e) 
{ 
    wcout << L"NoMatch:" << ToString(e.Result.Reason) << '\n';
}

void OnCanceled(const SpeechRecognitionEventArgs& e)
{
    wcout << L"Canceled:" << ToString(e.Result.Reason) << L"- " << e.Result.Text << '\n';
}

// Continuous speech recognition.
void ContinuousRecognitionUsingEvents()
{
    // Gets recognizer factory.
    auto factory = RecognizerFactory::GetDefault();

    // Sets subscription key
    // Replaces with your own subscription key.
    factory->SetSubscriptionKey(L"YourSubscriptionKey");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();

    // Subscribes to events.
    recognizer->IntermediateResult.Connect(&OnPartialResult);
    recognizer->FinalResult.Connect(&OnFinalResult);
    recognizer->Canceled.Connect(&OnCanceled);
    recognizer->NoMatch.Connect(&OnNoMatch);

    wcout << L"Say something...\n";

    // Starts continuos recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    wcout << L"Press any key to stop\n";
    string s;
    getline(cin, s);

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();

    // Unsubscribes to events.
    recognizer->IntermediateResult.Disconnect(&OnPartialResult);
    recognizer->FinalResult.Disconnect(&OnFinalResult);
    recognizer->Canceled.Disconnect(&OnCanceled);
    recognizer->NoMatch.Disconnect(&OnNoMatch);
}

int wmain(int argc, wchar_t **argv)
{
    cout << "1. Recognition with microphone input.\n";
    cout << "2. Recognition with file input.\n";
    cout << "3. Using CRIS model for recognition.\n";
    cout << "4. Continuous Recognition using events.\n";
    cout << "Your choice: (0. Exit.) ";

    string input;
    do
    {
        input.empty();
        getline(cin, input);
        
        switch (input[0])
        {
        case '1':
            RecognitionWithMicrophone();
            break;
        case '2':
            RecognitionWithFile();
            break;
        case '3':
            RecognitionUsingCustomizedModel();
            break;
        case '4':
            ContinuousRecognitionUsingEvents();
            break;
        case '0':
            cout << "\nStop recognition";
            break;
        }
        cout << "\nRecognition done. Your Choice:";
    } while ( input[0] != '0');
}

wstring ToString(Reason r)
{
    std::wstring reasons[] = {
        L"Recognized",
        L"IntermediateResult",
        L"NoMatch",
        L"Canceled",
        L"Unknown"
    };

    std::wstring str;
    str = L"    Reason: " + reasons[(int)r] + L"\n";

    return str;
}