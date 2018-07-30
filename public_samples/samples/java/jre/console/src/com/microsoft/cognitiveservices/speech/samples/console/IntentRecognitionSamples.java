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
import com.microsoft.cognitiveservices.speech.intent.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class IntentRecognitionSamples {

    // Intent recognition using microphone.
    public static void intentRecognitionWithMicrophone() throws InterruptedException, ExecutionException
    {
        // <IntentRecognitionWithMicrophone>
        // Creates an instance of a speech factory with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer using microphone as audio input. The default language is "en-us".
        IntentRecognizer recognizer = factory.createIntentRecognizer();

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent("id1", model, "YourLanguageUnderstandingIntentName1");
        recognizer.addIntent("id2", model, "YourLanguageUnderstandingIntentName2");
        recognizer.addIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");

        System.out.println("Say something...");

        // Starts recognition. It returns when the first utterance has been recognized.
        IntentRecognitionResult result = recognizer.recognizeAsync().get();

        // Checks result.
        if (result.getReason() != RecognitionStatus.Recognized) {
            System.out.println("There was an error, reason " + result.getReason() + "-" + result.getErrorDetails());
        }
        else {
            System.out.println("We recognized: " + result.getText());
            System.out.println("    Intent Id: " + result.getIntentId());
            System.out.println("    Language Understanding Json: " + result.getJson());
        }
        // </IntentRecognitionWithMicrophone>
    }

    // Intent recognition in the specified language, using microphone.
    public static void intentRecognitionWithLanguage() throws InterruptedException, ExecutionException
    {
        // <IntentRecognitionWithLanguage>
        // Creates an instance of a speech factory with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer in the specified language using microphone as audio input.
        String lang = "de-de";
        IntentRecognizer recognizer = factory.createIntentRecognizer(lang);

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent("id1", model, "YourLanguageUnderstandingIntentName1");
        recognizer.addIntent("id2", model, "YourLanguageUnderstandingIntentName2");
        recognizer.addIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");

        System.out.println("Say something...");

        // Starts recognition. It returns when the first utterance has been recognized.
        IntentRecognitionResult result = recognizer.recognizeAsync().get();

        // Checks result.
        if (result.getReason() != RecognitionStatus.Recognized) {
            System.out.println("There was an error, reason " + result.getReason() + "-" + result.getErrorDetails());
        }
        else {
            System.out.println("We recognized: " + result.getText());
            System.out.println("    Intent Id: " + result.getIntentId());
            System.out.println("    Language Understanding Json: " + result.getJson());
        }
        // </IntentRecognitionWithLanguage>
    }

    // Continuous intent recognition.
    public static void intentContinuousRecognitionWithFile() throws InterruptedException, ExecutionException, IOException
    {
        // <IntentContinuousRecognitionWithFile>
        // Creates an instance of a speech factory with specified
        // subscription key (called 'endpoint key' by the Language Understanding service)
        // and service region. Replace with your own subscription (endpoint) key
        // and service region (e.g., "westus2").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourLanguageUnderstandingSubscriptionKey", "YourLanguageUnderstandingServiceRegion");

        // Creates an intent recognizer using file as audio input.
        // Replace with your own audio file name.
        IntentRecognizer recognizer = factory.createIntentRecognizerWithFileInput("YourAudioFile.wav");

        // Creates a language understanding model using the app id, and adds specific intents from your model
        LanguageUnderstandingModel model = LanguageUnderstandingModel.fromAppId("YourLanguageUnderstandingAppId");
        recognizer.addIntent("id1", model, "YourLanguageUnderstandingIntentName1");
        recognizer.addIntent("id2", model, "YourLanguageUnderstandingIntentName2");
        recognizer.addIntent("any-IntentId-here", model, "YourLanguageUnderstandingIntentName3");

        // Subscribes to events.
        recognizer.IntermediateResultReceived.addEventListener((s, e) -> {
            System.out.println("IntermediateResult:" + e.getResult().getText());
        });
        
        recognizer.FinalResultReceived.addEventListener((s, e) -> {
            System.out.println("We recognized: " + e.getResult().getText());
            System.out.println("    Intent Id: " + e.getResult().getIntentId());
            System.out.println("    Language Understanding Json: " + e.getResult().getJson());
        });
        
        recognizer.RecognitionErrorRaised.addEventListener((s, e) -> {
            System.out.println("Canceled:" + e.getStatus());            
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
