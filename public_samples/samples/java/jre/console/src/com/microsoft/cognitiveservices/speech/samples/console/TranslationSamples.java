package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.IOException;
import java.util.ArrayList;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.translation.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class TranslationSamples {
    // Translation from microphone.
    public static void translationWithMicrophoneAsync() throws InterruptedException, ExecutionException, IOException {
        // <TranslationWithMicrophoneAsync>
        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Sets source and target languages
        String fromLanguage = "en-US";
        ArrayList<String> toLanguages = new ArrayList<String>();
        toLanguages.add("de");

        // Sets voice name of synthesis output.
        String GermanVoice = "de-DE-Hedda";

        // Creates a translation recognizer using microphone as audio input, and requires voice output.
        TranslationRecognizer recognizer = factory.createTranslationRecognizer(fromLanguage, toLanguages, GermanVoice);
        {
            // Subscribes to events.
            recognizer.IntermediateResultReceived.addEventListener((s, e) -> {
                System.out.println("\nPartial result: recognized in " + fromLanguage + ": " + e.getResult().getText() + ".");
                
                if (e.getResult().getTranslationStatus() == TranslationStatus.Success) {
                    Map<String, String> map = e.getResult().getTranslations();
                    for (String element : map.keySet()) {
                        System.out.println("    Translated into " + element + ": " + map.get(element));
                    }
                }
                else {
                    System.out.println("    Translation failed. Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails());
                }
            });

            recognizer.FinalResultReceived.addEventListener((s, e) -> {
                if (e.getResult().getReason() != RecognitionStatus.Recognized) {
                    System.out.println("\nFinal result: Status: " + e.getResult().getReason() + ", FailureReason: " + e.getResult().getErrorDetails() + ".");
                    return;
                }
                else {
                    System.out.println("\nFinal result: Status: " + e.getResult().getReason() + ", recognized text in " + fromLanguage + ": " + e.getResult().getText() + ".");
                    
                    if (e.getResult().getTranslationStatus() == TranslationStatus.Success) {
                        Map<String, String> map = e.getResult().getTranslations();
                        for(String element : map.keySet()) {
                            System.out.println("    Translated into " + element + ": " + map.get(element));
                        }
                    }
                    else {
                        System.out.println("    Translation failed. Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails());
                    }
                }
            });

            recognizer.SynthesisResultReceived.addEventListener((s, e) -> {
                System.out.println("Synthesis result received. Size of audio data: " + e.getResult().getAudio().length);
            });

            recognizer.RecognitionErrorRaised.addEventListener((s, e) -> {
                System.out.println("\nAn error occurred. Status: " + e.getStatus());
            });

            recognizer.SessionEvent.addEventListener((s, e) -> {
                System.out.println("\nSession event. Event: " + e.getEventType() + ".");
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Say something...");
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Press any key to stop");
            new Scanner(System.in).nextLine();

            recognizer.stopContinuousRecognitionAsync().get();
        }
        // </TranslationWithMicrophoneAsync>
    }
    
    // Translation using file input.
    // <TranslationWithFileAsync>
    private static Semaphore stopTranslationWithFileSemaphore;

    public static void translationWithFileAsync() throws InterruptedException, ExecutionException {
        stopTranslationWithFileSemaphore = new Semaphore(0);

        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Sets source and target languages
        String fromLanguage = "en-US";
        ArrayList<String> toLanguages = new ArrayList<String>();
        toLanguages.add("de");
        toLanguages.add("fr");

        // Creates a translation recognizer using file as audio input.
        // Replace with your own audio file name.
        TranslationRecognizer recognizer = factory.createTranslationRecognizerWithFileInput("YourAudioFile.wav", fromLanguage, toLanguages);
        {
            // Subscribes to events.
            recognizer.IntermediateResultReceived.addEventListener((s, e) -> {
                System.out.println("\nPartial result: recognized in " + fromLanguage + ": " + e.getResult().getText() + ".");
                
                if (e.getResult().getTranslationStatus() == TranslationStatus.Success) {
                    Map<String, String> map = e.getResult().getTranslations();
                    for (String element : map.keySet()) {
                        System.out.println("    Translated into " + element + ": " + map.get(element));
                    }
                }
                else {
                    System.out.println("    Translation failed. Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails());
                }
            });

            recognizer.FinalResultReceived.addEventListener((s, e) -> {
                if (e.getResult().getReason() != RecognitionStatus.Recognized) {
                    System.out.println("\nFinal result: Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails() + ".");
                    return;
                }
                else {
                    System.out.println("\nFinal result: Status: " + e.getResult().getTranslationStatus() + ", recognized text in " + fromLanguage + ": " + e.getResult().getErrorDetails() + ".");
                    
                    if (e.getResult().getTranslationStatus() == TranslationStatus.Success) {
                        Map<String, String> map = e.getResult().getTranslations();
                        for (String element : map.keySet()) {
                            System.out.println("    Translated into " + element + ": " + map.get(element));
                        }
                    }
                    else {
                        System.out.println("    Translation failed. Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails());
                    }
                }
            });

            recognizer.RecognitionErrorRaised.addEventListener((s, e) -> {
                System.out.println("\nAn error occurred. Status: " + e.getStatus());
                stopTranslationWithFileSemaphore.release();;
            });

            recognizer.SessionEvent.addEventListener((s, e) -> {
                System.out.println("\nSession event. Event: " + e.getEventType() + ".");
                
                // Stops translation when session stop is detected.
                if (e.getEventType() == SessionEventType.SessionStoppedEvent) {
                    System.out.println("\nStop translation.");
                    stopTranslationWithFileSemaphore.release();;
                }
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Start translation...");
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopTranslationWithFileSemaphore.acquire();;

            // Stops translation.
            recognizer.stopContinuousRecognitionAsync().get();
        }
    }
    // </TranslationWithFileAsync>
    
    // Translation using audio stream.
    // <TranslationWithAudioStreamAsync>
    private static Semaphore stopTranslationWithAudioStreamSemaphore;

    public static void translationWithAudioStreamAsync() throws InterruptedException, ExecutionException, FileNotFoundException {
        stopTranslationWithAudioStreamSemaphore = new Semaphore(0);

        // Creates an instance of a speech factory with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechFactory factory = SpeechFactory.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Sets source and target languages
        String fromLanguage = "en-US";
        ArrayList<String> toLanguages = new ArrayList<String>();
        toLanguages.add("de");
        toLanguages.add("fr");

        // Create an audio stream from a wav file.
        // Replace with your own audio file name.
        AudioInputStream stream = new WavStream(new FileInputStream("YourAudioFile.wav"));

        // Creates a translation recognizer using audio stream as input.
        TranslationRecognizer recognizer = factory.createTranslationRecognizerWithStream(stream, fromLanguage, toLanguages);
        {
            // Subscribes to events.
            recognizer.IntermediateResultReceived.addEventListener((s, e) -> {
                System.out.println("\nPartial result: recognized in " + fromLanguage + ": " + e.getResult().getText() + ".");
                
                if (e.getResult().getTranslationStatus() == TranslationStatus.Success) {
                    Map<String, String> map = e.getResult().getTranslations();
                    for (String element : map.keySet()) {
                        System.out.println("    Translated into " + element + ": " + map.get(element));
                    }
                }
                else {
                    System.out.println("    Translation failed. Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails());
                }
            });

            recognizer.FinalResultReceived.addEventListener((s, e) -> {
                if (e.getResult().getReason() != RecognitionStatus.Recognized) {
                    System.out.println("\nFinal result: Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails() + ".");
                    return;
                }
                else {
                    System.out.println("\nFinal result: Status: " + e.getResult().getTranslationStatus() + ", recognized text in " + fromLanguage + ": " + e.getResult().getText() + ".");
                    
                    if (e.getResult().getTranslationStatus() == TranslationStatus.Success) {
                        Map<String, String> map = e.getResult().getTranslations();
                        for (String element : map.keySet()) {
                            System.out.println("    Translated into " + element + ": " + map.get(element));
                        }
                    }
                    else {
                        System.out.println("    Translation failed. Status: " + e.getResult().getTranslationStatus() + ", FailureReason: " + e.getResult().getErrorDetails());
                    }
                }
            });

            recognizer.RecognitionErrorRaised.addEventListener((s, e) -> {
                System.out.println("\nAn error occurred. Status: " + e.getStatus());
                stopTranslationWithAudioStreamSemaphore.release();
            });

            recognizer.SessionEvent.addEventListener((s, e) -> {
                System.out.println("\nSession event. Event: " + e.getEventType() + ".");
                
                // Stops translation when session stop is detected.
                if (e.getEventType() == SessionEventType.SessionStoppedEvent) {
                    System.out.println("\nStop translation.");
                    stopTranslationWithAudioStreamSemaphore.release();
                }
            });

            // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Start translation...");
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopTranslationWithAudioStreamSemaphore.acquire();

            // Stops translation.
            recognizer.stopContinuousRecognitionAsync().get();
        }
    }
    // </TranslationWithAudioStreamAsync>
}
