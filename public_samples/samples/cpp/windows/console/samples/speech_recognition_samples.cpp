//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <mutex>
#include <condition_variable>

// <toplevel>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
// </toplevel>

// Speech recognition using microphone.
void SpeechRecognitionWithMicrophone()
{
    // <SpeechRecognitionWithMicrophone>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
    auto recognizer = factory->CreateSpeechRecognizer();
    wcout << L"Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"Recognition Status: " << int(result->Reason) << L". ";
        if (result->Reason == Reason::Canceled)
        {
            wcout << L"There was an error, reason: " << result->ErrorDetails << std::endl;
        }
        else
        {
            wcout << L"No speech could be recognized.\n";
        }
    }
    else
    {
        wcout << L"We recognized: " << result->Text
              << L", starting at " << result->Offset() << L"(ticks)"
              << L", with duration of " << result->Duration() << L"(ticks)"
              << std::endl;
    }
    // </SpeechRecognitionWithMicrophone>
}


// Speech recognition in the specified language, using microphone, and requesting detailed output format.
void SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat()
{
    // <SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Creates a speech recognizer in the specified language using microphone as audio input.
    // Replace the language with your language in BCP-47 format, e.g. en-US.
    auto lang = L"de-DE";
    // Requests detailed output format.
    auto recognizer = factory->CreateSpeechRecognizer(lang, OutputFormat::Detailed);
    wcout << L"Say something in " << lang << L"...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"Recognition Status:" << int(result->Reason);
        if (result->Reason == Reason::Canceled)
        {
            wcout << L"There was an error, reason: " << result->ErrorDetails << std::endl;
        }
        else
        {
            wcout << L"No speech could be recognized.\n";
        }
    }
    else
    {
        wcout << L"We recognized: " << result->Text << std::endl
              << L"Detailed output result in JSON: " << result->Properties[ResultProperty::Json].GetString()
              << std::endl;
    }
    // </SpeechRecognitionWithLanguageAndUsingDetailedOutputFormat>
}

// <SpeechContinuousRecognitionWithFile>
static std::mutex g_stopSpeechRecognitionMutex;
static std::condition_variable g_stopSpeechRecognitionConditionVariable;
static bool g_stopSpeechRecognition;

static void NotifyToStopRecognition()
{
    std::unique_lock<std::mutex> lock(g_stopSpeechRecognitionMutex);
    g_stopSpeechRecognition = true;
    g_stopSpeechRecognitionConditionVariable.notify_all();
}

static void OnPartialResult(const SpeechRecognitionEventArgs& e)
{
    wcout << L"IntermediateResult:" << e.Result.Text << std::endl;
}

static void OnFinalResult(const SpeechRecognitionEventArgs& e)
{
    wcout << L"Recognition Status:" << int(e.Result.Reason);
    switch (e.Result.Reason)
    {
    case Reason::Recognized:
        wcout << L"We recognized: " << e.Result.Text
              << L". Offset: " << e.Result.Offset() << L". Duration: " << e.Result.Duration()
              << std::endl;
        break;
    case Reason::InitialSilenceTimeout:
        wcout << L"The start of the audio stream contains only silence, and the service timed out waiting for speech.\n";
        break;
    case Reason::InitialBabbleTimeout:
        wcout << L"The start of the audio stream contains only noise, and the service timed out waiting for speech.\n";
        break;
    case Reason::NoMatch:
        wcout << L"The speech was detected in the audio stream, but no words from the target language were matched."
              << L"Possible reasons could be wrong setting of the target language or wrong format of audio stream.\n";
        break;
    case Reason::Canceled:
        wcout << L"There was an error, reason: " << e.Result.ErrorDetails << std::endl;
        break;
    default:
        wcout << L"Recognition Status:" << int(e.Result.Reason);
        break;
    }
}

static void OnCanceled(const SpeechRecognitionEventArgs& e)
{
    wcout << L"Canceled:" << (int)e.Result.Reason << L"- " << e.Result.ErrorDetails << std::endl;

    // Notify to stop recognition.
    NotifyToStopRecognition();

}

static void OnSessionStoppedEvent(const SessionEventArgs& e)
{
    wcout << L"Session stopped.";

    // Notify to stop recognition.
    NotifyToStopRecognition();
}

void SpeechContinuousRecognitionWithFile()
{
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Creates a speech recognizer using file as audio input.
    // Replace with your own audio file name.
    auto recognizer = factory->CreateSpeechRecognizerWithFileInput(L"YourAudioFile.wav");

    // Subscribes to events.
    recognizer->IntermediateResult.Connect(&OnPartialResult);
    recognizer->FinalResult.Connect(&OnFinalResult);
    recognizer->Canceled.Connect(&OnCanceled);
    recognizer->SessionStopped.Connect(&OnSessionStoppedEvent);

    // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
    recognizer->StartContinuousRecognitionAsync().wait();

    {
        std::unique_lock<std::mutex> lock(g_stopSpeechRecognitionMutex);
        g_stopSpeechRecognitionConditionVariable.wait(lock, [] { return g_stopSpeechRecognition; });
    }

    // Stops recognition.
    recognizer->StopContinuousRecognitionAsync().wait();

    // Unsubscribes from events.
    recognizer->IntermediateResult.Disconnect(&OnPartialResult);
    recognizer->FinalResult.Disconnect(&OnFinalResult);
    recognizer->Canceled.Disconnect(&OnCanceled);
    recognizer->SessionStopped.Disconnect(&OnSessionStoppedEvent);
}
// </SpeechContinuousRecognitionWithFile>

// Speech recognition using a customized model.
void SpeechRecognitionUsingCustomizedModel()
{
    // <SpeechRecognitionUsingCustomizedModel>
    // Creates an instance of a speech factory with specified
    // subscription key and service region. Replace with your own subscription key
    // and service region (e.g., "westus").
    auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

    // Creates a speech recognizer using microphone as audio input.
    auto recognizer = factory->CreateSpeechRecognizer();

    // Set the deployment id of your customized model
    // Replace with your own CRIS deployment id.
    recognizer->SetDeploymentId(L"YourDeploymentId");
    wcout << L"Say something...\n";

    // Performs recognition.
    // RecognizeAsync() returns when the first utterance has been recognized, so it is suitable 
    // only for single shot recognition like command or query. For long-running recognition, use
    // StartContinuousRecognitionAsync() instead.
    auto result = recognizer->RecognizeAsync().get();

    // Checks result.
    if (result->Reason != Reason::Recognized)
    {
        wcout << L"Recognition Status:" << int(result->Reason);
        if (result->Reason == Reason::Canceled)
        {
            wcout << L"There was an error, reason: " << result->ErrorDetails << std::endl;
        }
    }
    else
    {
        wcout << L"We recognized: " << result->Text << std::endl;
    }
    // </SpeechRecognitionUsingCustomizedModel>
}
