package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.IOException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
// </toplevel>

public class SpeechRecognitionSamples {
    // Speech recognition from microphone.
    public static void recognitionWithMicrophoneAsync() throws InterruptedException, ExecutionException {
        // <recognitionWithMicrophone>
        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech recognizer using microphone as audio input. The default language is "en-us".
        SpeechRecognizer recognizer = factory.createSpeechRecognizer();
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeAsync().get();

            // Checks result.
            if (result.getReason() != RecognitionStatus.Recognized) {
                System.out.println("There was an error. Status:" + result.getReason().toString() + "}, Reason:" + result.getErrorDetails());
            }
            else {
                System.out.println("We recognized: " + result.getText());
            }
        }
        recognizer.close();
        // </recognitionWithMicrophone>
    }

    // Speech recognition in the specified spoken language.
    public static void recognitionWithLanguageAsync() throws InterruptedException, ExecutionException {
        // <recognitionWithLanguage>
        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech recognizer for the specified language, using microphone as audio input.
        String lang = "de-de";
        SpeechRecognizer recognizer = factory.createSpeechRecognizer(lang);
        {
            // Starts recognizing.
            System.out.println("Say something in " + lang + " ...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeAsync().get();

            // Checks result.
            if (result.getReason() != RecognitionStatus.Recognized) {
                System.out.println("There was an error. Status:" + result.getReason().toString() + "}, Reason:" + result.getErrorDetails());
            }
            else {
                System.out.println("We recognized: " + result.getText());
            }
        }
        recognizer.close();
        // </recognitionWithLanguage>
    }
    

    // Speech recognition using a customized model.
    public static void recognitionUsingCustomizedModelAsync() throws InterruptedException, ExecutionException {
        // <recognitionCustomized>
        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = factory.createSpeechRecognizer();
        {
            // Replace with the CRIS deployment id of your customized model.
            recognizer.setDeploymentId("YourDeploymentId");

            // Starts recognizing.
             System.out.println("Say something...");

             // Starts recognition. It returns when the first utterance has been recognized.
             SpeechRecognitionResult result = recognizer.recognizeAsync().get();

             // Checks result.
             if (result.getReason() != RecognitionStatus.Recognized) {
                 System.out.println("There was an error. Status:" + result.getReason().toString() + "}, Reason:" + result.getErrorDetails());
             }
             else {
                 System.out.println("We recognized: " + result.getText());
             }
         }

         recognizer.close();
        // </recognitionCustomized>
    }


    // Speech recognition with events from file
    public static void continuousRecognitionWithFileAsync() throws InterruptedException, ExecutionException, IOException {
        // <recognitionContinuousWithFile>
        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech recognizer using file as audio input.
        // Replace with your own audio file name.
        SpeechRecognizer recognizer = factory.createSpeechRecognizerWithFileInput("YourAudioFile.wav");
        {
            // Subscribes to events.
            recognizer.IntermediateResultReceived.addEventListener((s, e) -> {
                System.out.println("\n    Partial result: " + e.getResult().getText() + ".");
            });

            recognizer.FinalResultReceived.addEventListener((s, e) -> {
                if (e.getResult().getReason() == RecognitionStatus.Recognized) {
                    System.out.println("\n    Final result: Status: " + e.getResult().getReason().toString() +
                                       ", Text: " + e.getResult().getText() + ".");
                }
                else {
                    System.out.println("\n    Final result: Status: " + e.getResult().getReason().toString() +
                                       ", FailureReason: " + e.getResult().getErrorDetails() + ".");
                }
            });

            recognizer.RecognitionErrorRaised.addEventListener((s, e) -> {
                System.out.println("\n    An error occurred. Status: " + e.getStatus() + ", FailureReason: " + e.getStatus());
            });

            recognizer.SessionEvent.addEventListener((s, e) -> {
                System.out.println("\n    Session event. Event: " + e.getEventType() + ".");
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Say something...");
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Press any key to stop");
            System.in.read();

            recognizer.stopContinuousRecognitionAsync().get();
        }
        // </recognitionContinuousWithFile>
    }

    // <recognitionAudioStream>
    // The Source to stop recognition.
    private static Semaphore stopRecognitionSemaphore;

    // Speech recognition with audio stream
    public static void recognitionWithAudioStreamAsync() throws InterruptedException, ExecutionException, FileNotFoundException {
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Create an audio stream from a wav file.
        // Replace with your own audio file name.
        AudioInputStream stream = new WavStream(new FileInputStream("YourAudioFile.wav"));

        // Creates a speech recognizer using audio stream input.
        SpeechRecognizer recognizer = factory.createSpeechRecognizerWithStream(stream);
        {
            // Subscribes to events.
            recognizer.IntermediateResultReceived.addEventListener((s, e) -> {
                System.out.println("\n    Partial result: " + e.getResult().getText() + ".");
            });

            recognizer.FinalResultReceived.addEventListener((s, e) -> {
                if (e.getResult().getReason() == RecognitionStatus.Recognized) {
                    System.out.println("\n    Final result: Status: " + e.getResult().getReason().toString() +
                                       ", Text: " + e.getResult().getText() + ".");
                }
                else {
                    System.out.println("\n    Final result: Status: " + e.getResult().getReason().toString() +
                                       ", FailureReason: " + e.getResult().getErrorDetails() + ".");
                }
            });

            recognizer.RecognitionErrorRaised.addEventListener((s, e) -> {
                System.out.println("\n    An error occurred. Status: " + e.getStatus() + ", FailureReason: " + e.getStatus());
                stopRecognitionSemaphore.release();
            });

            recognizer.SessionEvent.addEventListener((s, e) -> {
                System.out.println("\nSession event. Event: " + e.getEventType() + ".");
                
                // Stops translation when session stop is detected.
                if (e.getEventType() == SessionEventType.SessionStoppedEvent)
                {
                    System.out.println("\nStop translation.");
                    stopRecognitionSemaphore.release();
                }
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            // Stops recognition.
            recognizer.stopContinuousRecognitionAsync().get();
        }
    }
    // </recognitionAudioStream>
}
