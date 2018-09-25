# Overview

The Cognitive Services Speech SDK 1.0.0 introduces a number of breaking changes. This page helps users understand those changes and migrate from an earlier version to this release.

## Removal of SpeechFactory

The `SpeechFactory` class is removed. Instead, the class `SpeechConfig` is introduced to describe various settings of speech configuration and the class `AudioConfig` to describe different audio sources (microphone, file, or stream input). To create a `SpeechRecognizer`, use one of its constructors with SpeechConfig and AudioConfig as parameters. The C# code below shows how to create a SpeechRecognizer using default microphone input.

```csharp
// Creates an instance of speech config with specified subscription key and service region.
var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

// Creates a speech recognizer using microphone as audio input.
using (var recognizer = new SpeechRecognizer(config))
{
    // Performs recognition.
    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
    // Process result.
    // ...
}
```

The same concept is applied for creating `IntentRecognizer` and `TranslationRecognizer`, except that `SpeechTranslationConfig` is required for creating `TranslationRecognizer`.

### Using file input

`CreateSpeechRecognizerWithFileInput()` is replaced by `AudioConfig`. The following C# code shows how to create a speech recognizer using file input.
```csharp
// Creates an instance of speech config with specified subscription key and service region.
var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

// Creates a speech recognizer using file as audio input.
// Replace with your own audio file name.
using (var audioInput = AudioConfig.FromWavFileInput(@"whatstheweatherlike.wav"))
{
    using (var recognizer = new SpeechRecognizer(config, audioInput))
    {
        // Performs recognition.
        var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
        // Process result.
        // ...
    }
}
```

### Using stream input

`CreateSpeechRecognizerWithStream()` is now replaced by `AudioConfig`. To use a stream input, first use `AudioConfig.FromStreamInput()` to create an instance of `AudioConfig` with specified stream, and then pass it as a parameter to `SpeechRecognizer()`. In addition to pull-based audio input streams, Speech SDK 1.0.0 now also supports push-based audio input streams. Check `AudioInputStream`, `PullAudioInputStream`, and `PushAudioInputStream` for details.

## Events

There are some naming changes for event handlers. The table below shows the mapping between event handler names prior to release 1.0.0, referred as **old API**, and those in release 1.0.0, referred as **new API**.

| old API | new API |
|------| ------- |
| FinalResultReceived | Recognized |
| IntermediateResultReceived | Recognizing |
| OnSessionEvent | SessionStarted, SessionStopped |
| OnSpeechDetectedEvent | SpeechStartDetected, SpeechEndDetected |
| RecognitionErrorRaised | Canceled |
| SynthesisResultReceived | Synthesizing |

The payload name of each event has also been changed.

| old API | new API |
|------| ------- |
| SpeechRecognitionResultEventArgs | SpeechRecognitionEventArgs |
| IntentRecognitionResultEventArgs | IntentRecognitionEventArgs |
| TranslationTextResultEventArgs | TranslationRecognitionEventArgs |
| TranslationSynthesisResultEventArgs | TranslationSynthesisEventArgs |
| RecognitionErrorEventArgs | SpeechRecognitionCanceledEventArgs, IntentRecognitionCanceledEventArgs, TranslationRecognitionCanceledEventArgs |

## Recognition result

The Speech SDK Release 1.0.0 introduces the `ResultReason` class that defines a unified recognition status for all three recognizers (speech, intent, and translation). With that, the field `RecognitionStatus` in `SpeechRecognitionResult` is changed to `Reason`, and the field `TranslationStatus`in `TranslationTextResult` is removed. In addition, `CancellationDetails` and `NoMatchDetails` are introduced in release 1.0.0 to describe reasons why a canceled or nomatch result is returned.

There are also changes in class inheritance hierarchy. `RecognitionResult`is introduced as a base class for `SpeechRecognitionResult`, `IntentRecognitionResult`, and `TranslationRecognitionResult`.

For translation, the `TranslationTextResult` is renamed to `TranslationRecognitionResult`.

## Intent recognizer

Based on feedback from users, the `AddIntent()` methods in `IntentRecognizer` changed their signature.

| old API | new API |
|------| ------- |
| AddIntent(intentId, phrase) | AddIntent(phrase, intentId) |
| n.a.| AddIntent(phrase) |
| AddIntent(intentId, model, intentName) | AddIntent(model, intentName, intentId) |
| n.a | AddIntent(model, intentName) |
| n.a.| AddAllIntents(model) |
| n.a.| AddAllIntents(model, intentId) |

## UTF8 encoding in C/C++ API

Starting from this release, the C/C++ APIs now use UTF8 encoding. The C++ API now uses `std::string` instead of `std::wstring`, and `char` instead of `wchar_t`.


# C#

## Creating a SpeechRecognizer, IntentRecognizer, or TranslationRecognizer

The `SpeechFactory` used to create the various types of recognizers has been replaced by constructors for recognizers.
They take a `SpeechConfig` instance and, optionally, a `AudioConfig` instance as argument. For example, for an intent recognizer:

Old API:
```csharp
var factory = SpeechFactory.FromSubscription("SubscriptionKey", "ServiceRegion");

// for recognition from microphone:
var recognizer_microphone = factory.CreateIntentRecognizer();

// for recognition from file:
var recognizer_file = factory.CreateSpeechRecognizerWithFileInput(@"whatstheweatherlike.wav")
```

New API:
```csharp
var config = SpeechConfig.FromSubscription("SubscriptionKey", "ServiceRegion");
var audioInput = AudioConfig.FromWavFileInput(@"whatstheweatherlike.wav");

// for recognition from microphone:
var recognizer_microphone = new IntentRecognizer(config);

// for recognition from file:
var recognizer_file = new IntentRecognizer(config, audioInput);
```

## Recognition Results

The way the recognizers return their results has been changed.
The `RecognitionResult.RecognitionStatus` attribute has been replaced by `RecognitionResult.Reason`, which gives the reason for the result being what it is, e.g. details on why a `NoMatch` or `Canceled` result has been returned.
The particular API changes for cancellations are outlined [below](#cancellation).

### Speech Recognition

Old API:

```csharp
if (result.RecognitionStatus != RecognitionStatus.Recognized)
{
    Console.WriteLine($"Recognition status: {result.RecognitionStatus.ToString()}");
    if (result.RecognitionStatus == RecognitionStatus.Canceled)
    {
        Console.WriteLine($"There was an error, reason: {result.RecognitionFailureReason}");
    }
    else
    {
        Console.WriteLine("No speech could be recognized.\n");
    }
}
else
{
    Console.WriteLine($"We recognized: {result.Text}, Offset: {result.OffsetInTicks}, Duration: {result.Duration}.");
}
```

New API:

```csharp
if (result.Reason == ResultReason.RecognizedSpeech)
{
    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
}
else if (result.Reason == ResultReason.NoMatch)
{
    Console.WriteLine($"NOMATCH: Speech could not be recognized.");
}
else if (result.Reason == ResultReason.Canceled)
{
    var cancellation = CancellationDetails.FromResult(result);
    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

    if (cancellation.Reason == CancellationReason.Error)
    {
        Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
        Console.WriteLine($"CANCELED: Did you update the subscription info?");
    }
}
```

### Intent recognition

Old API:
```csharp
var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

// Checks result.
if (result.RecognitionStatus != RecognitionStatus.Recognized)
{
    Console.WriteLine($"Recognition status: {result.RecognitionStatus.ToString()}");
    if (result.RecognitionStatus == RecognitionStatus.Canceled)
    {
        Console.WriteLine($"There was an error, reason: {result.RecognitionFailureReason}");
    }
    else
    {
        Console.WriteLine("No speech could be recognized.\n");
    }
}
else
{
    Console.WriteLine($"We recognized: {result.Text}.");
    Console.WriteLine($"\n    Intent Id: {result.IntentId}.");
    Console.WriteLine($"\n    Language Understanding JSON: {result.Properties.Get<string>(ResultPropertyKind.LanguageUnderstandingJson)}.");
}
```

New API:
```csharp
var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
if (result.Reason == ResultReason.RecognizedIntent)
{
    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
    Console.WriteLine($"    Intent Id: {result.IntentId}.");
    Console.WriteLine($"    Language Understanding JSON: {result.Properties.GetProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult)}.");
}
else if (result.Reason == ResultReason.RecognizedSpeech)
{
    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
    Console.WriteLine($"    Intent not recognized.");
}
else if (result.Reason == ResultReason.NoMatch)
{
    Console.WriteLine($"NOMATCH: Speech could not be recognized.");
}
else if (result.Reason == ResultReason.Canceled)
{
    var cancellation = CancellationDetails.FromResult(result);
    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");
}
```

## Cancellation

In particular, the `ResultReason` for recognition results that have been canceled allows better insight into why the cancellation occurred.
To get more details on the cancellation, a `CancellationDetails` object can be instantiated with data from the result as shown below.

Old API:
```csharp
var result = await recognizer.RecognizeAsync().ConfigureAwait(false);

if (result.RecognitionStatus == RecognitionStatus.Canceled)
{
    Console.WriteLine($"There was an error, reason: {result.RecognitionFailureReason}");
}
```

New API:
```csharp
var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

if (result.Reason == ResultReason.Canceled)
{
    var cancellation = CancellationDetails.FromResult(result);
    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

    if (cancellation.Reason == CancellationReason.Error)
    {
        Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
        Console.WriteLine($"CANCELED: Did you update the subscription info?");
    }
}
```

## Adding Intents

The order of the arguments of the `IntentRecognizer.AddIntent` method has changed.

Old API:
```
recognizer.AddIntent("id1", model, "YourLanguageUnderstandingIntentName1");
recognizer.AddIntent("id2", model, "YourLanguageUnderstandingIntentName2");
recognizer.AddIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");
```

New API:
```
recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
recognizer.AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");
```

## TranslationSynthesisResult
The property `Audio` is now replaced by method `GetAudio()`. 

## Removal of BinaryAudioStream
`BinaryAudioStream` class was removed from the SDK. Users can create their own stream implementation based on provided samples (that is, for a new BinaryAudioStreamReader, check `samples\csharp\sharedcontent\console\helper.cs` in [the samples repo](https://github.com/Azure-Samples/cognitive-services-speech-sdk)).

# Java

## Initialization

SDK initialization is no longer needed.
You can remove any call to "SpeechFactory.configurePlatformBindingWithDefaultCertificates()".
Instead, the SpeechSDK will use the root certificates that are installed on your Android device. (Note that it will not use any user-imported certificates though)

Old API:
```java
SpeechFactory.configureNativePlatformBindingWithDefaultCertifictae();
```

## Creating a Speech Recognizer

The `SpeechFactory` used to create the various types of recognizers has been replaced by instance constructors that take a `SpeechConfig` instance as argument. For example, for a speech recognizer:

Old API:
```java
var factory = SpeechFactory.fromSubscription("SubscriptionKey", "ServiceRegion");
var recognizer = factory.createSpeechRecognizer();
```

New API:
```java
var config = SpeechConfig.fromSubscription("SubscriptionKey", "ServiceRegion");
var recognizer = new SpeechRecognizer(config);
```

Note that you can set up the audio configuration by using the `AudioConfig` class, for example:
```java
var config = SpeechConfig.fromSubscription("SubscriptionKey", "ServiceRegion");
var audio = AudioConfig.fromWavFileInput("whatstheweatherlike.wav");
var recognizer = new SpeechRecognizer(config, audio);
```

## Adding Intents

The order of the arguments of the `IntentRecognizer.AddIntent` method has changed.

Old API:
```java
recognizer.addIntent("id1", model, "YourLanguageUnderstandingIntentName1");
recognizer.addIntent("id2", model, "YourLanguageUnderstandingIntentName2");
recognizer.addIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");
```

New API:
```java
recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");
```

# C++

## Creating a SpeechRecognizer, IntentRecognizer, or TranslationRecognizer

The `SpeechFactory` used to create the various types of recognizers has been replaced by constructors for recognizers.
They take a `SpeechConfig` instance and, optionally, a `AudioConfig` instance as argument. For example, for an intent recognizer:

Old API:
```cpp
auto factory = SpeechFactory::FromSubscription(L"YourSubscriptionKey", L"YourServiceRegion");

// for recognition from microphone:
auto recognizer = factory->CreateIntentRecognizer();

// for recognition from file:
auto recognizer = factory->CreateIntentRecognizerWithFileInput(L"whatstheweatherlike.wav");
```

New API:
```cpp
auto speechConfig = SpeechConfig::FromSubscription("YourSubscriptionKey", "YourServiceRegion");

// for recognition from microphone:
auto recognizer = IntentRecognizer::FromConfig(speechConfig);

// for recognition from file:
auto audioInput = AudioConfig::FromWavFileInput("whatstheweatherlike.wav");
auto recognizer = IntentRecognizer::FromConfig(speechConfig, audioInput);
```

## Recognition Results

The way the recognizers return their results has been changed.
The `RecognitionResult.RecognitionStatus` attribute has been replaced by `RecognitionResult.Reason`, which gives the reason for the result being what it is, e.g. details on why a `NoMatch` or `Canceled` result has been returned.
The particular API changes for cancellations are outlined [below](#cancellation).

### Speech Recognition

Old API:

```cpp
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
```

New API:

```cpp
auto result = recognizer->RecognizeOnceAsync().get();

// Checks result.
if (result->Reason == ResultReason::RecognizedSpeech)
{
    cout << "RECOGNIZED: Text=" << result->Text << std::endl
            << "  Speech Service JSON: " << result->Properties.GetProperty(PropertyId::SpeechServiceResponse_JsonResult)
            << std::endl;
}
else if (result->Reason == ResultReason::NoMatch)
{
    cout << "NOMATCH: Speech could not be recognized." << std::endl;
}
else if (result->Reason == ResultReason::Canceled)
{
    auto cancellation = CancellationDetails::FromResult(result);
    cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

    if (cancellation->Reason == CancellationReason::Error)
    {
        cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
        cout << "CANCELED: Did you update the subscription info?" << std::endl;
    }
}
```

### Intent recognition

Old API:
```cpp
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
    wcout << L"We recognized: " << result->Text << std::endl;
    wcout << L"    Intent Id: " << result->IntentId << std::endl;
    wcout << L"    Intent response in Json: " << result->Properties[ResultProperty::LanguageUnderstandingJson].GetString() << std::endl;
}
```

New API:
```cpp
auto result = recognizer->RecognizeOnceAsync().get();

// Checks result.
if (result->Reason == ResultReason::RecognizedIntent)
{
    cout << "RECOGNIZED: Text=" << result->Text << std::endl;
    cout << "  Intent Id: " << result->IntentId << std::endl;
    cout << "  Intent Service JSON: " << result->Properties.GetProperty(PropertyId::LanguageUnderstandingServiceResponse_JsonResult) << std::endl;
}
else if (result->Reason == ResultReason::RecognizedSpeech)
{
    cout << "RECOGNIZED: Text=" << result->Text << " (intent could not be recognized)" << std::endl;
}
else if (result->Reason == ResultReason::NoMatch)
{
    cout << "NOMATCH: Speech could not be recognized." << std::endl;
}
else if (result->Reason == ResultReason::Canceled)
{
    auto cancellation = CancellationDetails::FromResult(result);
    cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

    if (cancellation->Reason == CancellationReason::Error)
    {
        cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
        cout << "CANCELED: Did you update the subscription info?" << std::endl;
    }
}
```

## Cancellation

In particular, the `ResultReason` for recognition results that have been canceled allows better insight into why the cancellation occurred.
To get more details on the cancellation, a `CancellationDetails` object can be instantiated with data from the result as shown below.

Old API:
```cpp
auto result = recognizer->RecognizeAsync().get();

if (result->Reason == Reason::Canceled)
{
    wcout << L"There was an error, reason: " << result->ErrorDetails << std::endl;
}
```

New API:
```cpp
auto result = recognizer->RecognizeOnceAsync().get();

// Checks result.
if (result->Reason == ResultReason::Canceled)
{
    auto cancellation = CancellationDetails::FromResult(result);
    cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

    if (cancellation->Reason == CancellationReason::Error)
    {
        cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
        cout << "CANCELED: Did you update the subscription info?" << std::endl;
    }
}
```

## Adding Intents

The order of the arguments of the `IntentRecognizer.AddIntent` method has changed.

Old API:
```cpp
auto model = LanguageUnderstandingModel::FromAppId(L"YourLanguageUnderstandingAppId");
recognizer->AddIntent(L"id1", model, L"YourLanguageUnderstandingIntentName1");
recognizer->AddIntent("id2", model, L"YourLanguageUnderstandingIntentName2");
recognizer->AddIntent("any-IntentId-here", model, L"YourLanguageUnderstandingIntentName3");
```

New API:
```cpp
auto model = LanguageUnderstandingModel::FromAppId("YourLanguageUnderstandingAppId");
recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
recognizer->AddIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");
```
