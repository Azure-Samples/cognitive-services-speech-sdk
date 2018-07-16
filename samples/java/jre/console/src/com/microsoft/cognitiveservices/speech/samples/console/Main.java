package com.microsoft.cognitiveservices.speech.samples.console;

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class Main {

    public static void main(String[] args) {
        System.out.println("1. Speech recognition with microphone input.");
        System.out.println("2. Speech recognition in the specified language.");
        System.out.println("3. Speech recognition using customized model.");
        System.out.println("4. Speech continuous recognition using events with file.");
        System.out.println("5. Speech recognition with audio stream.");
        System.out.println("6. Translation with microphone input.");
        System.out.println("7. Translation with file input.");
        System.out.println("8. Translation with audio stream.");
        System.out.println("9. Speech continuous recognition using authorization token.");
        System.out.println("A. Intent recognition with microphone.");
        System.out.println("B. Intent recognition with language.");
        System.out.println("C. Intent continuous recognition using events with file.");

        System.out.print("Your choice (0: Stop.): ");

        try {
            char x;
            do {
                x = (char) System.in.read();
                System.out.println("");
                switch (x) {
                case '1':
                    SpeechRecognitionSamples.recognitionWithMicrophoneAsync();
                    break;
                case '2':
                    SpeechRecognitionSamples.recognitionWithLanguageAsync();
                    break;
                case '3':
                    SpeechRecognitionSamples.recognitionUsingCustomizedModelAsync();
                    break;
                case '4':
                    SpeechRecognitionSamples.continuousRecognitionWithFileAsync();
                    break;
                case '5':
                    SpeechRecognitionSamples.recognitionWithAudioStreamAsync();
                    break;
                case '6':
                    TranslationSamples.translationWithMicrophoneAsync();
                    break;
                case '7':
                    TranslationSamples.translationWithFileAsync();
                    break;
                case '8':
                    TranslationSamples.translationWithAudioStreamAsync();
                    break;
                // case '9':
                // SpeechRecognitionWithTokenSample.ContinuousRecognitionWithAuthorizationTokenAsync();
                // break;
                case 'a':
                case 'A':
                    IntentRecognitionSamples.intentRecognitionWithMicrophone();
                    break;
                case 'b':
                case 'B':
                    IntentRecognitionSamples.intentRecognitionWithLanguage();
                    break;
                case 'c':
                case 'C':
                    IntentRecognitionSamples.intentContinuousRecognitionWithFile();
                    break;
                case '0':
                    System.out.println("Exiting...");
                    break;
                }
                System.out.println("\nRecognition done. Your Choice (0: Stop): ");
            } while (x != '0');
        } catch (Exception ex) {
            System.out.println("Unexpected " + ex.toString());
            System.exit(1);
        }
    }
}
