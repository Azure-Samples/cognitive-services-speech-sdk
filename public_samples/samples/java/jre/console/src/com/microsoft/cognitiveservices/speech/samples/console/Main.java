package com.microsoft.cognitiveservices.speech.samples.console;

import java.util.Scanner;

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

@SuppressWarnings("resource") // scanner
public class Main {

    public static void main(String[] args) {
        String prompt = "Your choice (0: Stop): ";

        System.out.println("1. Speech recognition with microphone input.");
        System.out.println("2. Speech recognition in the specified language.");
        System.out.println("3. Speech recognition using customized model.");
        System.out.println("4. Speech continuous recognition using events with file.");
        System.out.println("5. Speech recognition with audio stream.");
        System.out.println("6. Speech recognition with keyword spotting.");
        System.out.println("7. Translation with microphone input.");
        System.out.println("8. Translation with file input.");
        System.out.println("9. Translation with audio stream.");
        System.out.println("A. Intent recognition with microphone.");
        System.out.println("B. Intent recognition with language.");
        System.out.println("C. Intent continuous recognition using events with file.");
        System.out.println("D. Speech continuous recognition using events with file.");

        System.out.print(prompt);

        try {
            String x;
            do {
                x = new Scanner(System.in).nextLine();
                System.out.println("");
                switch (x) {
                case "1":
                    SpeechRecognitionSamples.recognitionWithMicrophoneAsync();
                    break;
                case "2":
                    SpeechRecognitionSamples.recognitionWithLanguageAsync();
                    break;
                case "3":
                    SpeechRecognitionSamples.recognitionUsingCustomizedModelAsync();
                    break;
                case "4":
                    SpeechRecognitionSamples.continuousRecognitionWithFileAsync();
                    break;
                case "5":
                    SpeechRecognitionSamples.recognitionWithAudioStreamAsync();
                    break;
                case "6":
                    SpeechRecognitionSamples.keywordTriggeredSpeechRecognitionWithMicrophone();
                    break;
                case "7":
                    TranslationSamples.translationWithMicrophoneAsync();
                    break;
                case "8":
                    TranslationSamples.translationWithFileAsync();
                    break;
                case "9":
                    TranslationSamples.translationWithAudioStreamAsync();
                    break;
                case "a":
                case "A":
                    IntentRecognitionSamples.intentRecognitionWithMicrophone();
                    break;
                case "b":
                case "B":
                    IntentRecognitionSamples.intentRecognitionWithLanguage();
                    break;
                case "c":
                case "C":
                    IntentRecognitionSamples.intentContinuousRecognitionWithFile();
                    break;
                case "D":
                    SpeechRecognitionSamples.continuousRecognitionWithPushStream();
                    break;
                case "0":
                    System.out.println("Exiting...");
                    break;
                }
                System.out.println("\nRecognition done. " + prompt);
            } while (!x.equals("0"));
            
            System.out.println("Finishing demo.");
            System.exit(0);
        } catch (Exception ex) {
            System.out.println("Unexpected " + ex.toString());
            System.exit(1);
        }
    }
}
