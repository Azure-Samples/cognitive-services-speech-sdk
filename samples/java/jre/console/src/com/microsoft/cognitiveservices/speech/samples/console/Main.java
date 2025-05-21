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
        System.out.println("14. Intent Pattern Matching with microphone.");
        System.out.println("15. Intent Pattern Matching with microphone and keyword spotting.");
        System.out.println("16. Speech continuous recognition using events with a PushStream.");
        System.out.println("17. Speech continuous recognition using events with file and Phrase List.");
        System.out.println("18. Speech synthesis to speaker output.");
        System.out.println("19. Speech synthesis with specified language.");
        System.out.println("20. Speech synthesis with specified voice.");
        System.out.println("21. Speech synthesis to wave file.");
        System.out.println("22. Speech synthesis to mp3 file.");
        System.out.println("23. Speech synthesis to pull audio output stream.");
        System.out.println("24. Speech synthesis to push audio output stream.");
        System.out.println("25. Speech synthesis to result.");
        System.out.println("26. Speech synthesis to audio data stream.");
        System.out.println("27. Speech synthesis events.");
        System.out.println("28. Speech synthesis word boundary event.");
        System.out.println("29. Speech synthesis server scenario example.");
        System.out.println("30. Speech synthesis with source language auto detection.");
        System.out.println("31. Speaker verification with microphone.");
        System.out.println("32. Speaker verification with file.");
        System.out.println("33. Speaker identification with microphone.");
        System.out.println("34. Speaker identification with file.");
        System.out.println("35. Speech recognition from default microphone with Microsoft Audio Stack enabled.");
        System.out.println("36. Speech recognition from a microphone with Microsoft Audio Stack enabled and "
                + "pre-defined microphone array geometry specified.");
        System.out.println("37. Speech recognition from multi-channel file with Microsoft Audio Stack enabled and "
                + "custom microphone array geometry specified.");
        System.out.println("38. Speech recognition from pull stream with custom set of enhancements from Microsoft Audio Stack enabled.");
        System.out.println("39. Speech recognition from push stream with Microsoft Audio Stack enabled and beamforming angles specified.");
        System.out.println("40. Pronunciation assessment with microphone input.");
        System.out.println("41. Pronunciation assessment with stream input.");
        System.out.println("42. Pronunciation assessment continuous with file.");
        System.out.println("43. Pronunciation assessment configured with JSON.");
        System.out.println("44. Speech synthesis to audio and SRT files.");
        System.out.println("45. Recognize once from file, with at-start language detection.");
        System.out.println("46. Continuous speech recognition from file, with at-start language detection.");
        System.out.println("47. Continuous speech recognition from file, with at-start language detection with custom model.");
        System.out.println("48. Continuous speech recognition from file, with continuous language detection with custom models.");
        System.out.println("49. Diagnostics logging to file (without a filter).");
        System.out.println("50. Diagnostics logging to file (with a filter).");
        System.out.println("51. Diagnostics Subscribing to logging event (without a filter).");
        System.out.println("52. Diagnostics Subscribing to logging event (with a filter).");
        System.out.println("53. Diagnostics logging to memory buffer with logging level (with or without filter).");
        System.out.println("54. Diagnostics logging to memory buffer with ingested self-defined SPX trace mark.");

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
                    IntentRecognitionSamples.IntentPatternMatchingWithMicrophone();
                    break;
                case "15":
                    IntentRecognitionSamples.IntentPatternMatchingWithMicrophoneAndKeywordSpotting();
                    break;
                case "16":
                    SpeechRecognitionSamples.continuousRecognitionWithPushStream();
                    break;
                case "17":
                    SpeechRecognitionSamples.continuousRecognitionWithFileWithPhraseListAsync();
                    break;
                case "18":
                    SpeechSynthesisSamples.synthesisToSpeakerAsync();
                    break;
                case "19":
                    SpeechSynthesisSamples.synthesisWithLanguageAsync();
                    break;
                case "20":
                    SpeechSynthesisSamples.synthesisWithVoiceAsync();
                    break;
                case "21":
                    SpeechSynthesisSamples.synthesisToWaveFileAsync();
                    break;
                case "22":
                    SpeechSynthesisSamples.synthesisToMp3FileAsync();
                    break;
                case "23":
                    SpeechSynthesisSamples.synthesisToPullAudioOutputStreamAsync();
                    break;
                case "24":
                    SpeechSynthesisSamples.synthesisToPushAudioOutputStreamAsync();
                    break;
                case "25":
                    SpeechSynthesisSamples.synthesisToResultAsync();
                    break;
                case "26":
                    SpeechSynthesisSamples.synthesisToAudioDataStreamAsync();
                    break;
                case "27":
                    SpeechSynthesisSamples.synthesisEventsAsync();
                    break;
                case "28":
                    SpeechSynthesisSamples.synthesisWordBoundaryEventAsync();
                    break;
                case "29":
                    SpeechSynthesisScenarioSamples.synthesisServerScenarioAsync();
                    break;
                case "30":
                    SpeechSynthesisSamples.synthesisWithSourceLanguageAutoDetectionAsync();
                    break;
                case "31":
                    SpeakerRecognitionSamples.verificationWithMicrophoneAsync();
                    break;
                case "32":
                    SpeakerRecognitionSamples.verificationWithFileAsync();
                    break;
                case "33":
                    SpeakerRecognitionSamples.identificationWithMicrophoneAsync();
                    break;
                case "34":
                    SpeakerRecognitionSamples.identificationWithFileAsync();
                    break;
                case "35":
                    SpeechRecognitionSamples.continuousRecognitionFromDefaultMicrophoneWithMASEnabled();
                    break;
                case "36":
                    SpeechRecognitionSamples.recognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified();
                    break;
                case "37":
                    SpeechRecognitionSamples.continuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified();
                    break;
                case "38":
                    SpeechRecognitionSamples.recognitionFromPullStreamWithSelectMASEnhancementsEnabled();
                    break;
                case "39":
                    SpeechRecognitionSamples.continuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified();
                    break;
                case "40":
                    SpeechRecognitionSamples.pronunciationAssessmentWithMicrophoneAsync();
                    break;
                case "41":
                    SpeechRecognitionSamples.pronunciationAssessmentWithPushStream();
                    break;
                case "42":
                    SpeechRecognitionSamples.pronunciationAssessmentContinuousWithFile();;
                    break;
                case "43":
                    SpeechRecognitionSamples.pronunciationAssessmentConfiguredWithJson();
                    break;
                case "44":
                    SpeechSynthesisSamples.synthesisWordBoundaryEventToSrtAsync();
                    break;
                case "45":
                    SpeechRecognitionSamples.recognizeOnceFromFileWithAtStartLanguageDetection();
                    break;
                case "46":
                    SpeechRecognitionSamples.continuousRecognitionFromFileWithAtStartLanguageDetection();
                    break;
                case "47":
                    SpeechRecognitionSamples.continuousRecognitionFromFileWithAtStartLanguageDetectionWithCustomModels();
                    break;
                case "48":
                    SpeechRecognitionSamples.continuousRecognitionFromFileWithContinuousLanguageDetectionWithCustomModels();
                    break;
                case "49":
                    SpeechDiagnosticsLoggingSamples.fileLoggerWithoutFilter();
                    break;
                case "50":
                    SpeechDiagnosticsLoggingSamples.fileLoggerWithFilter();
                    break;
                case "51":
                    SpeechDiagnosticsLoggingSamples.eventLoggerWithoutFilter();
                    break;
                case "52":
                    SpeechDiagnosticsLoggingSamples.eventLoggerWithFilter();
                    break;
                case "53":
                    SpeechDiagnosticsLoggingSamples.memoryLoggerWithOrWithoutFilter();
                    break;
                case "54":
                    SpeechDiagnosticsLoggingSamples.selfDefinedSpxTraceLogging();
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
