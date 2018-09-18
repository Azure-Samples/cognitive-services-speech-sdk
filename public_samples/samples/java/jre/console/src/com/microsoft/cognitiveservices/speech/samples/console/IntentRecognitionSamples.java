package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.IOException;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;

// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.intent.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class IntentRecognitionSamples {

    // Intent recognition using microphone.
    public static void intentRecognitionWithMicrophone() throws InterruptedException, ExecutionException
    {
        // <IntentRecognitionWithMicrophone>
        // Creates an instance of a speech config with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer using microphone as audio input.
        IntentRecognizer recognizer = new IntentRecognizer(config);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

        System.out.println("Say something...");

        // Starts recognition. It returns when the first utterance has been recognized.
        IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();

        // Checks result.
        if (result.getReason() == ResultReason.RecognizedIntent) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent Id: " + result.getIntentId());
            System.out.println("    Intent Service Json: " + result.getJson());
        }
        else if (result.getReason() == ResultReason.RecognizedSpeech) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent not recognized.");
        }
        else if (result.getReason() == ResultReason.NoMatch) {
            System.out.println("NOMATCH: Speech could not be recognized.");
        }
        else if (result.getReason() == ResultReason.Canceled) {
            CancellationDetails cancellation = CancellationDetails.fromResult(result);
            System.out.println("CANCELED: Reason=" + cancellation.getReason());

            if (cancellation.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }
        }
        // </IntentRecognitionWithMicrophone>
    }

    // Intent recognition in the specified language, using microphone.
    public static void intentRecognitionWithLanguage() throws InterruptedException, ExecutionException
    {
        // <IntentRecognitionWithLanguage>
        // Creates an instance of a speech config with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechConfig config = SpeechConfig.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer in the specified language using microphone as audio input.
        String lang = "de-de";
        config.setSpeechRecognitionLanguage(lang);
        IntentRecognizer recognizer = new IntentRecognizer(config);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

        System.out.println("Say something...");

        // Starts recognition. It returns when the first utterance has been recognized.
        IntentRecognitionResult result = recognizer.recognizeOnceAsync().get();

        // Checks result.
        if (result.getReason() == ResultReason.RecognizedIntent) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent Id: " + result.getIntentId());
            System.out.println("    Intent Service Json: " + result.getJson());
        }
        else if (result.getReason() == ResultReason.RecognizedSpeech) {
            System.out.println("RECOGNIZED: Text=" + result.getText());
            System.out.println("    Intent not recognized.");
        }
        else if (result.getReason() == ResultReason.NoMatch) {
            System.out.println("NOMATCH: Speech could not be recognized.");
        }
        else if (result.getReason() == ResultReason.Canceled) {
            CancellationDetails cancellation = CancellationDetails.fromResult(result);
            System.out.println("CANCELED: Reason=" + cancellation.getReason());

            if (cancellation.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }
        }
        // </IntentRecognitionWithLanguage>
    }

    // Continuous intent recognition.
    public static void intentContinuousRecognitionWithFile() throws InterruptedException, ExecutionException, IOException
    {
        // <IntentContinuousRecognitionWithFile>
        // Creates an instance of a speech config with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechConfig config = SpeechConfig.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer using file as audio input.
        // Replace with your own audio file name.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourAudioFile.wav");
        IntentRecognizer recognizer = new IntentRecognizer(config, audioInput);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName1", "id1");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName2", "id2");
        recognizer.addIntent(model, "YourLanguageUnderstandingIntentName3", "any-IntentId-here");

        // Subscribes to events.
        recognizer.recognizing.addEventListener((s, e) -> {
            System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
        });

        recognizer.recognized.addEventListener((s, e) -> {
            if (e.getResult().getReason() == ResultReason.RecognizedIntent) {
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                System.out.println("    Intent Id: " + e.getResult().getIntentId());
                System.out.println("    Intent Service Json: " + e.getResult().getJson());
            }
            else if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                System.out.println("    Intent not recognized.");
            }
            else if (e.getResult().getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
        });

        recognizer.canceled.addEventListener((s, e) -> {
            System.out.println("CANCELED: Reason=" + e.getReason());

            if (e.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }
        });

        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
        recognizer.startContinuousRecognitionAsync().get();

        System.out.println("Press any key to stop...");
        new Scanner(System.in).nextLine();

        // Stops recognition.
        recognizer.stopContinuousRecognitionAsync().get();
        // </IntentContinuousRecognitionWithFile>
    }
}
