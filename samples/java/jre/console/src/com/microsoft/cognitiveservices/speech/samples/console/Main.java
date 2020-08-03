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
        System.out.println("D. Speech continuous recognition using events with a PushStream.");
        System.out.println("E. Speech continuous recognition using events with file and Phrase List.");
        System.out.println("F. Speech synthesis to speaker output.");
        System.out.println("G. Speech synthesis with specified language.");
        System.out.println("H. Speech synthesis with specified voice.");
        System.out.println("I. Speech synthesis to wave file.");
        System.out.println("J. Speech synthesis to mp3 file.");
        System.out.println("K. Speech synthesis to pull audio output stream.");
        System.out.println("L. Speech synthesis to push audio output stream.");
        System.out.println("M. Speech synthesis to result.");
        System.out.println("N. Speech synthesis to audio data stream.");
        System.out.println("O. Speech synthesis events.");
        System.out.println("P. Speech synthesis word boundary event.");
        System.out.println("Q: Speech synthesis server scenario example.");
        System.out.println("R: Speech synthesis with source language auto detection.");

        System.out.print(prompt);

        try {
            String x;
            do {
                x = new Scanner(System.in).nextLine();
                System.out.println("");
                switch (x.toLowerCase()) {
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
                    IntentRecognitionSamples.intentRecognitionWithMicrophone();
                    break;
                case "b":
                    IntentRecognitionSamples.intentRecognitionWithLanguage();
                    break;
                case "c":
                    IntentRecognitionSamples.intentContinuousRecognitionWithFile();
                    break;
                case "d":
                    SpeechRecognitionSamples.continuousRecognitionWithPushStream();
                    break;
                case "e":
                    SpeechRecognitionSamples.continuousRecognitionWithFileWithPhraseListAsync();
                    break;
                case "f":
                    SpeechSynthesisSamples.synthesisToSpeakerAsync();
                    break;
                case "g":
                    SpeechSynthesisSamples.synthesisWithLanguageAsync();
                    break;
                case "h":
                    SpeechSynthesisSamples.synthesisWithVoiceAsync();
                    break;
                case "i":
                    SpeechSynthesisSamples.synthesisToWaveFileAsync();
                    break;
                case "j":
                    SpeechSynthesisSamples.synthesisToMp3FileAsync();
                    break;
                case "k":
                    SpeechSynthesisSamples.synthesisToPullAudioOutputStreamAsync();
                    break;
                case "l":
                    SpeechSynthesisSamples.synthesisToPushAudioOutputStreamAsync();
                    break;
                case "m":
                    SpeechSynthesisSamples.synthesisToResultAsync();
                    break;
                case "n":
                    SpeechSynthesisSamples.synthesisToAudioDataStreamAsync();
                    break;
                case "o":
                    SpeechSynthesisSamples.synthesisEventsAsync();
                    break;
                case "p":
                    SpeechSynthesisSamples.synthesisWordBoundaryEventAsync();
                    break;
                case "q":
                    SpeechSynthesisScenarioSamples.synthesisServerScenarioAsync();
                    break;
                case "r":
                    SpeechSynthesisSamples.synthesisWithSourceLanguageAutoDetectionAsync();
                    break;
                case "0":
                    System.out.println("Exiting...");
                    break;
                }
                System.out.println("\nExecution done. " + prompt);
            } while (!x.equals("0"));

            System.out.println("Finishing demo.");
            System.exit(0);
        } catch (Exception ex) {
            System.out.println("Unexpected " + ex.toString());
            System.exit(1);
        }
    }
}
