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

        System.out.println(" 1. Speech recognition with microphone input.");
        System.out.println(" 2. Speech recognition with microphone input and detailed recognition results");
        System.out.println(" 3. Speech recognition in the specified language.");
        System.out.println(" 4. Speech recognition using customized model.");
        System.out.println(" 5. Speech continuous recognition using events with file.");
        System.out.println(" 6. Speech recognition with audio stream.");
        System.out.println(" 7. Speech recognition with keyword spotting.");
        System.out.println(" 8. Translation with microphone input.");
        System.out.println(" 9. Translation with file input.");
        System.out.println("10. Translation with audio stream.");
        System.out.println("11. Intent recognition with microphone.");
        System.out.println("12. Intent recognition with language.");
        System.out.println("13. Intent continuous recognition using events with file.");
        System.out.println("14. Speech continuous recognition using events with a PushStream.");
        System.out.println("15. Speech continuous recognition using events with file and Phrase List.");
        System.out.println("16. Speech synthesis to speaker output.");
        System.out.println("17. Speech synthesis with specified language.");
        System.out.println("18. Speech synthesis with specified voice.");
        System.out.println("19. Speech synthesis to wave file.");
        System.out.println("20. Speech synthesis to mp3 file.");
        System.out.println("21. Speech synthesis to pull audio output stream.");
        System.out.println("22. Speech synthesis to push audio output stream.");
        System.out.println("23. Speech synthesis to result.");
        System.out.println("24. Speech synthesis to audio data stream.");
        System.out.println("25. Speech synthesis events.");
        System.out.println("26. Speech synthesis word boundary event.");
        System.out.println("27. Speech synthesis server scenario example.");
        System.out.println("28. Speech synthesis with source language auto detection.");
        System.out.println("29. Speaker verification with microphone.");
        System.out.println("30. Speaker verification with file.");
        System.out.println("31. Speaker identification with microphone.");
        System.out.println("32. Speaker identification with file.");
        System.out.println("33. Speech recognition from default microphone with Microsoft Audio Stack enabled.");
        System.out.println("34. Speech recognition from a microphone with Microsoft Audio Stack enabled and "
                + "pre-defined microphone array geometry specified.");
        System.out.println("35. Speech recognition from multi-channel file with Microsoft Audio Stack enabled and "
                + "custom microphone array geometry specified.");
        System.out.println("36. Speech recognition from pull stream with custom set of enhancements from Microsoft Audio Stack enabled.");
        System.out.println("37. Speech recognition from push stream with Microsoft Audio Stack enabled and beamforming angles specified.");

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
                    SpeechRecognitionSamples.recognitionWithMicrophoneAsyncAndDetailedRecognitionResults();
                    break;                    
                case "3":
                    SpeechRecognitionSamples.recognitionWithLanguageAsync();
                    break;
                case "4":
                    SpeechRecognitionSamples.recognitionUsingCustomizedModelAsync();
                    break;
                case "5":
                    SpeechRecognitionSamples.continuousRecognitionWithFileAsync();
                    break;
                case "6":
                    SpeechRecognitionSamples.recognitionWithAudioStreamAsync();
                    break;
                case "7":
                    SpeechRecognitionSamples.keywordTriggeredSpeechRecognitionWithMicrophone();
                    break;
                case "8":
                    TranslationSamples.translationWithMicrophoneAsync();
                    break;
                case "9":
                    TranslationSamples.translationWithFileAsync();
                    break;
                case "10":
                    TranslationSamples.translationWithAudioStreamAsync();
                    break;
                case "11":
                    IntentRecognitionSamples.intentRecognitionWithMicrophone();
                    break;
                case "12":
                    IntentRecognitionSamples.intentRecognitionWithLanguage();
                    break;
                case "13":
                    IntentRecognitionSamples.intentContinuousRecognitionWithFile();
                    break;
                case "14":
                    SpeechRecognitionSamples.continuousRecognitionWithPushStream();
                    break;
                case "15":
                    SpeechRecognitionSamples.continuousRecognitionWithFileWithPhraseListAsync();
                    break;
                case "16":
                    SpeechSynthesisSamples.synthesisToSpeakerAsync();
                    break;
                case "17":
                    SpeechSynthesisSamples.synthesisWithLanguageAsync();
                    break;
                case "18":
                    SpeechSynthesisSamples.synthesisWithVoiceAsync();
                    break;
                case "19":
                    SpeechSynthesisSamples.synthesisToWaveFileAsync();
                    break;
                case "20":
                    SpeechSynthesisSamples.synthesisToMp3FileAsync();
                    break;
                case "21":
                    SpeechSynthesisSamples.synthesisToPullAudioOutputStreamAsync();
                    break;
                case "22":
                    SpeechSynthesisSamples.synthesisToPushAudioOutputStreamAsync();
                    break;
                case "23":
                    SpeechSynthesisSamples.synthesisToResultAsync();
                    break;
                case "24":
                    SpeechSynthesisSamples.synthesisToAudioDataStreamAsync();
                    break;
                case "25":
                    SpeechSynthesisSamples.synthesisEventsAsync();
                    break;
                case "26":
                    SpeechSynthesisSamples.synthesisWordBoundaryEventAsync();
                    break;
                case "27":
                    SpeechSynthesisScenarioSamples.synthesisServerScenarioAsync();
                    break;
                case "28":
                    SpeechSynthesisSamples.synthesisWithSourceLanguageAutoDetectionAsync();
                    break;
                case "29":
                    SpeakerRecognitionSamples.verificationWithMicrophoneAsync();
                    break;
                case "30":
                    SpeakerRecognitionSamples.verificationWithFileAsync();
                    break;
                case "31":
                    SpeakerRecognitionSamples.identificationWithMicrophoneAsync();
                    break;
                case "32":
                    SpeakerRecognitionSamples.identificationWithFileAsync();
                    break;
                case "33":
                    SpeechRecognitionSamples.continuousRecognitionFromDefaultMicrophoneWithMASEnabled();
                    break;
                case "34":
                    SpeechRecognitionSamples.recognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified();
                    break;
                case "35":
                    SpeechRecognitionSamples.continuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified();
                    break;
                case "36":
                    SpeechRecognitionSamples.recognitionFromPullStreamWithSelectMASEnhancementsEnabled();
                    break;
                case "37":
                    SpeechRecognitionSamples.continuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified();
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
