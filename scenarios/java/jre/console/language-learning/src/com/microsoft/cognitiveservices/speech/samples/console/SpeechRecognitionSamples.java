//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package com.microsoft.cognitiveservices.speech.samples.console;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.Semaphore;
import java.io.FileInputStream;
import java.io.StringReader;
import java.io.File;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import jakarta.json.Json;
import jakarta.json.JsonArray;
import jakarta.json.JsonObject;
import jakarta.json.JsonReader;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Base64;
import java.util.UUID;

import com.github.difflib.DiffUtils;

import com.github.difflib.patch.AbstractDelta;
import com.github.difflib.patch.DeltaType;
import com.github.difflib.patch.Patch;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;

@SuppressWarnings("resource") // scanner
public class SpeechRecognitionSamples {

    // The Source to stop recognition.
    private static Semaphore stopRecognitionSemaphore;

    // Pronunciation assessment.
    // See more information at https://aka.ms/csspeech/pa
    public static void pronunciationAssessmentWithMicrophoneAsync()
            throws ExecutionException, InterruptedException, URISyntaxException {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Replace the language with your language in BCP-47 format, e.g., en-US.
        String lang = "en-US";

        // The pronunciation assessment service has a longer default end silence timeout
        // (5 seconds) than normal STT
        // as the pronunciation assessment is widely used in education scenario where
        // kids have longer break in reading.
        // You can adjust the end silence timeout based on your real scenario.
        config.setProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "3000");

        String referenceText = "";
        // Create pronunciation assessment config, set grading system, granularity and
        // if enable miscue based on your requirement.
        PronunciationAssessmentConfig pronunciationConfig = new PronunciationAssessmentConfig(referenceText,
                PronunciationAssessmentGradingSystem.HundredMark, PronunciationAssessmentGranularity.Phoneme, true);

        pronunciationConfig.enableProsodyAssessment();

        while (true) {
            // Creates a speech recognizer for the specified language, using microphone as
            // audio input.
            SpeechRecognizer recognizer = new SpeechRecognizer(config, lang);
            {
                // Receives reference text from console input.
                System.out.println("Enter reference text you want to assess, or enter empty text to exit.");
                System.out.print("> ");
                referenceText = new Scanner(System.in).nextLine();
                if (referenceText.isEmpty()) {
                    break;
                }

                pronunciationConfig.setReferenceText(referenceText);

                // Starts recognizing.
                System.out.println("Read out \"" + referenceText + "\" for pronunciation assessment ...");

                recognizer.sessionStarted.addEventListener((s, e) -> {
                    System.out.println("SESSION ID: " + e.getSessionId());
                });

                pronunciationConfig.applyTo(recognizer);

                // Starts speech recognition, and returns after a single utterance is
                // recognized.
                // For long-running multi-utterance recognition, use
                // StartContinuousRecognitionAsync() instead.
                SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

                // Checks result.
                if (result.getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + result.getText());
                    System.out.println("  PRONUNCIATION ASSESSMENT RESULTS:");

                    PronunciationAssessmentResult pronunciationResult = PronunciationAssessmentResult
                            .fromResult(result);
                    System.out.println(String.format(
                            "    Accuracy score: %f, Prosody score: %f, Pronunciation score: %f, Completeness score : %f, FluencyScore: %f",
                            pronunciationResult.getAccuracyScore(), pronunciationResult.getProsodyScore(),
                            pronunciationResult.getPronunciationScore(), pronunciationResult.getCompletenessScore(),
                            pronunciationResult.getFluencyScore()));
                } else if (result.getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                } else if (result.getReason() == ResultReason.Canceled) {
                    CancellationDetails cancellation = CancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
                recognizer.close();
            }
        }

        pronunciationConfig.close();
        config.close();
    }

    // Pronunciation assessment with events from a push stream
    // This sample takes and existing file and reads it by chunk into a local buffer
    // and then pushes the
    // buffer into an PushAudioStream for pronunciation assessment.
    // See more information at https://aka.ms/csspeech/pa
    public static void pronunciationAssessmentWithPushStream()
            throws InterruptedException, IOException, ExecutionException, URISyntaxException {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Replace the language with your language in BCP-47 format, e.g., en-US.
        String lang = "en-US";

        // Set audio format
        long samplesPerSecond = 16000;
        short bitsPerSample = 16;
        short channels = 1;

        // Whether to simulate the real time recording (need be set to true when
        // measuring latency with streaming)
        boolean simulateRealtimeRecording = false;

        // Create the push stream to push audio to.
        PushAudioInputStream pushStream = AudioInputStream
                .createPushStream(AudioStreamFormat.getWaveFormatPCM(samplesPerSecond, bitsPerSample, channels));

        // Creates a speech recognizer using Push Stream as audio input.
        AudioConfig audioInput = AudioConfig.fromStreamInput(pushStream);

        SpeechRecognizer recognizer = new SpeechRecognizer(config, lang, audioInput);

        stopRecognitionSemaphore = new Semaphore(0);

        final long[] lastAudioUploadedTime = new long[1];
        recognizer.recognized.addEventListener((s, e) -> {
            if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                PronunciationAssessmentResult pronunciationResult = PronunciationAssessmentResult
                        .fromResult(e.getResult());
                System.out.println(String.format(
                        "    Accuracy score: %f, Prosody score: %f, Pronunciation score: %f, Completeness score : %f, FluencyScore: %f",
                        pronunciationResult.getAccuracyScore(), pronunciationResult.getProsodyScore(),
                        pronunciationResult.getPronunciationScore(), pronunciationResult.getCompletenessScore(),
                        pronunciationResult.getFluencyScore()));
                long resultReceivedTime = System.currentTimeMillis();
                System.out.println(String.format("Latency: %d ms", resultReceivedTime - lastAudioUploadedTime[0]));
            } else if (e.getResult().getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            stopRecognitionSemaphore.release();
        });

        recognizer.canceled.addEventListener((s, e) -> {
            System.out.println("CANCELED: Reason=" + e.getReason());

            if (e.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }

            stopRecognitionSemaphore.release();
        });

        recognizer.sessionStarted.addEventListener((s, e) -> {
            System.out.println("\n    Session started event.");
        });

        recognizer.sessionStopped.addEventListener((s, e) -> {
            System.out.println("\n    Session stopped event.");
        });

        String referenceText = "Hello world";
        // Create pronunciation assessment config, set grading system, granularity and
        // if enable miscue based on your requirement.
        PronunciationAssessmentConfig pronunciationConfig = new PronunciationAssessmentConfig(referenceText,
                PronunciationAssessmentGradingSystem.HundredMark, PronunciationAssessmentGranularity.Phoneme, true);

        pronunciationConfig.enableProsodyAssessment();

        recognizer.sessionStarted.addEventListener((s, e) -> {
            System.out.println("SESSION ID: " + e.getSessionId());
        });

        pronunciationConfig.applyTo(recognizer);

        System.out.println("Assessing...");
        Future<SpeechRecognitionResult> resultFuture = recognizer.recognizeOnceAsync();

        // Replace with your own audio file name.
        // The input stream the sample will read from.
        InputStream inputStream = new FileInputStream("YourAudioFile.wav");

        // Arbitrary buffer size.
        byte[] readBuffer = new byte[4096];

        // Push audio read from the file into the PushStream.
        // The audio can be pushed into the stream before, after, or during recognition
        // and recognition will continue as data becomes available.
        int bytesRead;
        while ((bytesRead = inputStream.read(readBuffer)) != -1) {
            if (bytesRead == readBuffer.length) {
                pushStream.write(readBuffer);
            } else {
                // Last buffer read from the WAV file is likely to have less bytes
                pushStream.write(Arrays.copyOfRange(readBuffer, 0, bytesRead));
            }

            if (simulateRealtimeRecording) {
                // Sleep corresponding time for the uploaded audio chunk, to simulate the
                // natural speaking rate.
                Thread.sleep(bytesRead * 1000 / (bitsPerSample / 8) / samplesPerSecond / channels);
            }
        }

        inputStream.close();
        // Signal the end of stream to stop assessment
        pushStream.close();

        lastAudioUploadedTime[0] = System.currentTimeMillis();

        stopRecognitionSemaphore.acquire();
        // Wait for completion of recognizeOnceAsync
        resultFuture.get();

        config.close();
        audioInput.close();
        recognizer.close();
    }

    // Pronunciation assessment configured with json
    // See more information at https://aka.ms/csspeech/pa
    public static void pronunciationAssessmentConfiguredWithJson()
            throws ExecutionException, InterruptedException, URISyntaxException {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Replace the language with your language in BCP-47 format, e.g., en-US.
        String lang = "en-US";

        // Creates a speech recognizer using wav file.
        AudioConfig audioInput = AudioConfig.fromWavFileOutput("YourAudioFile.wav");

        String referenceText = "Hello world";
        // Create pronunciation assessment config, set grading system, granularity and
        // if enable miscue based on your requirement.
        String jsonConfig = "{\"GradingSystem\":\"HundredMark\",\"Granularity\":\"Phoneme\",\"EnableMiscue\":true,\"ScenarioId\":\"[scenario ID will be assigned by product team]\"}";
        PronunciationAssessmentConfig pronunciationConfig = PronunciationAssessmentConfig.fromJson(jsonConfig);
        pronunciationConfig.setReferenceText(referenceText);

        pronunciationConfig.enableProsodyAssessment();

        // Creates a speech recognizer for the specified language
        SpeechRecognizer recognizer = new SpeechRecognizer(config, lang, audioInput);
        {
            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("SESSION ID: " + e.getSessionId());
            });

            pronunciationConfig.applyTo(recognizer);

            // Starts speech recognition, and returns after a single utterance is
            // recognized.
            // For long-running multi-utterance recognition, use
            // StartContinuousRecognitionAsync() instead.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
                System.out.println("  PRONUNCIATION ASSESSMENT RESULTS:");

                PronunciationAssessmentResult pronunciationResult = PronunciationAssessmentResult.fromResult(result);
                System.out.println(String.format(
                        "    Accuracy score: %f, Prosody score: %f, Pronunciation score: %f, Completeness score : %f, FluencyScore: %f",
                        pronunciationResult.getAccuracyScore(), pronunciationResult.getProsodyScore(),
                        pronunciationResult.getPronunciationScore(), pronunciationResult.getCompletenessScore(),
                        pronunciationResult.getFluencyScore()));
            } else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            } else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
            recognizer.close();
        }

        pronunciationConfig.close();
        config.close();
    }

    private static List<String> convertReferenceWords(String referenceText, List<String> referenceWords) {
        Set<String> dictionary = new HashSet<>(referenceWords);
        int maxLength = dictionary.stream().mapToInt(String::length).max().orElse(0);

        referenceText = removePunctuation(referenceText);
        return segmentWord(referenceText, dictionary, maxLength);
    }

    private static String removePunctuation(String text) {
        // Remove punctuation from the text
        StringBuilder result = new StringBuilder();
        for (char c : text.toCharArray()) {
            if (Character.isLetterOrDigit(c) || Character.isWhitespace(c)) {
                result.append(c);
            }
        }
        return result.toString();
    }

    private static List<String> segmentWord(String referenceText, Set<String> dictionary, int maxLength) {
        List<String> leftToRight = leftToRightSegmentation(referenceText, dictionary, maxLength);
        List<String> rightToLeft = rightToLeftSegmentation(referenceText, dictionary, maxLength);

        if (String.join("", leftToRight).equals(referenceText)) {
            return leftToRight;
        }
        if (String.join("", rightToLeft).equals(referenceText)) {
            return rightToLeft;
        }

        System.out.println("WW failed to segment the text with the dictionary");

        if (leftToRight.size() < rightToLeft.size()) {
            return leftToRight;
        }
        if (leftToRight.size() > rightToLeft.size()) {
            return rightToLeft;
        }

        long leftToRightSingle = leftToRight.stream().filter(word -> word.length() == 1).count();
        long rightToLeftSingle = rightToLeft.stream().filter(word -> word.length() == 1).count();

        return leftToRightSingle < rightToLeftSingle ? leftToRight : rightToLeft;
    }

    // From left to right to do the longest matching to get the word segmentation
    private static List<String> leftToRightSegmentation(String text, Set<String> dictionary, int maxLength) {
        List<String> result = new ArrayList<>();
        while (!text.isEmpty()) {
            // If the length of the text is less than the max_length, then the sub_text is
            // the text itself
            String subText = text.length() < maxLength ? text : text.substring(0, maxLength);
            while (!subText.isEmpty()) {
                // If the sub_text is in the dictionary or the length of the sub_text is 1, then
                // add it to the result
                if (dictionary.contains(subText) || subText.length() == 1) {
                    result.add(subText);
                    // Remove the sub_text from the text
                    text = text.substring(subText.length());
                    break;
                }
                // If the sub_text is not in the dictionary, then remove the last character of
                // the sub_text
                subText = subText.substring(0, subText.length() - 1);
            }
        }
        return result;
    }

    // From right to left to do the longest matching to get the word segmentation
    private static List<String> rightToLeftSegmentation(String text, Set<String> dictionary, int maxLength) {
        List<String> result = new ArrayList<>();
        while (!text.isEmpty()) {
            // If the length of the text is less than the max_length, then the sub_text is
            // the text itself
            String subText = text.length() < maxLength ? text : text.substring(text.length() - maxLength);
            while (!subText.isEmpty()) {
                // If the sub_text is in the dictionary or the length of the sub_text is 1, then
                // add it to the result
                if (dictionary.contains(subText) || subText.length() == 1) {
                    result.add(subText);
                    // Remove the sub_text from the text
                    text = text.substring(0, text.length() - subText.length());
                    break;
                }
                // If the sub_text is not in the dictionary, then remove the first character of
                // the sub_text
                subText = subText.substring(1);
            }
        }
        // Reverse the result to get the correct order
        Collections.reverse(result);
        return result;
    }

    private static List<String> getReferenceWords(String waveFilename, String referenceText, String language,
            SpeechConfig speechConfig) {
        try {
            AudioConfig audioConfig = AudioConfig.fromWavFileInput(waveFilename);
            speechConfig.setSpeechRecognitionLanguage(language);

            SpeechRecognizer speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);

            // Create pronunciation assessment config
            boolean enableMiscue = true;
            PronunciationAssessmentConfig pronunciationConfig = new PronunciationAssessmentConfig(referenceText,
                    PronunciationAssessmentGradingSystem.HundredMark, PronunciationAssessmentGranularity.Phoneme,
                    enableMiscue);

            // Apply pronunciation assessment config to speech recognizer
            pronunciationConfig.applyTo(speechRecognizer);

            // Perform speech recognition
            SpeechRecognitionResult result = speechRecognizer.recognizeOnceAsync().get();

            if (result.getReason() == ResultReason.RecognizedSpeech) {
                List<String> referenceWords = new ArrayList<>();

                String responseJson = result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);

                // Parse the JSON result to extract NBest and Words
                JsonReader jsonReader = Json.createReader(new StringReader(responseJson));
                JsonObject root = jsonReader.readObject();
                JsonArray words = root.getJsonArray("NBest").getJsonObject(0).getJsonArray("Words");

                for (int i = 0; i < words.size(); i++) {
                    JsonObject item = words.getJsonObject(i);
                    String wordItem = item.getString("Word");
                    String errorTypeItem = item.getJsonObject("PronunciationAssessment").getString("ErrorType");

                    if (!"Insertion".equals(errorTypeItem)) {
                        referenceWords.add(wordItem);
                    }
                }
                return convertReferenceWords(referenceText, referenceWords);
            } else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("No speech could be recognized");
                return null;
            } else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("Speech Recognition canceled: " + cancellation.getReason());
                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("Error details: " + cancellation.getErrorDetails());
                }
                return null;
            }

        } catch (Exception ex) {
            ex.printStackTrace();
        }

        return null;
    }

    // Pronunciation assessment continuous with file.
    // See more information at https://aka.ms/csspeech/pa
    public static void pronunciationAssessmentContinuousWithFile()
            throws ExecutionException, InterruptedException, URISyntaxException, IOException {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // You can adjust the segmentation silence timeout based on your real scenario.
        config.setProperty(PropertyId.Speech_SegmentationSilenceTimeoutMs, "1500");

        String referenceText = Files.readString(Path.of("src/resources/zhcn_continuous_mode_sample.txt"));

        // Replace the language with your language in BCP-47 format, e.g., en-US.
        String lang = "zh-CN";
        Boolean enableMiscue = true;
        Boolean enableProsodyAssessment = true;
        Boolean unScriptedScenario = referenceText.length() > 0 ? false : true;

        // We need to convert the reference text to lower case, and split to words, then
        // remove the punctuations.
        String[] referenceWords;
        if (lang == "zh-CN") {
            // Split words for Chinese using the reference text and any short wave file

            referenceText = referenceText.replace(" ", "");
            referenceWords = getReferenceWords("src/resources/zhcn_short_dummy_sample.wav", referenceText, lang, config)
                    .toArray(new String[0]);
        } else {
            referenceWords = referenceText.toLowerCase().split(" ");
            for (int j = 0; j < referenceWords.length; j++) {
                referenceWords[j] = referenceWords[j].replaceAll("^\\p{Punct}+|\\p{Punct}+$", "");
            }
        }

        // Remove empty words
        List<String> filteredWords = new ArrayList<>();
        for (String w : referenceWords) {
            if (w != null && !w.trim().isEmpty()) {
                filteredWords.add(w);
            }
        }
        referenceWords = filteredWords.toArray(new String[0]);
        referenceText = String.join(" ", referenceWords);
        System.out.println("Reference text: " + referenceText);

        // Creates a speech recognizer using wav file.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("src/resources/zhcn_continuous_mode_sample.wav");

        stopRecognitionSemaphore = new Semaphore(0);
        List<String> recognizedWords = new ArrayList<>();
        List<Word> pronWords = new ArrayList<>();
        List<Word> finalWords = new ArrayList<>();
        List<Double> fluencyScores = new ArrayList<>();
        List<Double> prosodyScores = new ArrayList<>();
        LinkedList<Long> start_end_offsets = new LinkedList<>();

        SpeechRecognizer recognizer = new SpeechRecognizer(config, lang, audioInput);
        {
            // Subscribes to events.
            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                    PronunciationAssessmentResult pronResult = PronunciationAssessmentResult.fromResult(e.getResult());
                    System.out.println(String.format(
                            "    Accuracy score: %f, Prosody score: %f, Pronunciation score: %f, Completeness score : %f, FluencyScore: %f",
                            pronResult.getAccuracyScore(), pronResult.getProsodyScore(),
                            pronResult.getPronunciationScore(), pronResult.getCompletenessScore(),
                            pronResult.getFluencyScore()));
                    fluencyScores.add(pronResult.getFluencyScore());
                    prosodyScores.add(pronResult.getProsodyScore());

                    String jString = e.getResult().getProperties()
                            .getProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    JsonReader jsonReader = Json.createReader(new StringReader(jString));
                    JsonObject jsonObject = jsonReader.readObject();
                    jsonReader.close();

                    JsonArray nBestArray = jsonObject.getJsonArray("NBest");

                    for (int i = 0; i < nBestArray.size(); i++) {
                        JsonObject nBestItem = nBestArray.getJsonObject(i);

                        JsonArray wordsArray = nBestItem.getJsonArray("Words");

                        for (int j = 0; j < wordsArray.size(); j++) {
                            JsonObject wordItem = wordsArray.getJsonObject(j);
                            recognizedWords.add(wordItem.getString("Word").toLowerCase());

                            long offset = wordItem.getJsonNumber("Offset").longValue();
                            long duration = wordItem.getJsonNumber("Duration").longValue();

                            if (start_end_offsets.size() == 0) {
                                start_end_offsets.add(offset);
                            }

                            start_end_offsets.add(offset + duration + 100000);

                            JsonObject pronAssessment = wordItem.getJsonObject("PronunciationAssessment");
                            pronWords.add(new Word(wordItem.getString("Word"), pronAssessment.getString("ErrorType"),
                                    pronAssessment.getJsonNumber("AccuracyScore").doubleValue(), duration));
                        }
                    }
                } else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            recognizer.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }

                stopRecognitionSemaphore.release();
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\n    Session started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\n    Session stopped event.");
            });

            // Create pronunciation assessment config, set grading system, granularity and
            // if enable miscue based on your requirement.
            PronunciationAssessmentConfig pronunciationConfig = new PronunciationAssessmentConfig(referenceText,
                    PronunciationAssessmentGradingSystem.HundredMark, PronunciationAssessmentGranularity.Phoneme,
                    enableMiscue);

            if (enableProsodyAssessment) {
                pronunciationConfig.enableProsodyAssessment();
            }

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("SESSION ID: " + e.getSessionId());
            });

            pronunciationConfig.applyTo(recognizer);

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop
            // recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();

            // For continuous pronunciation assessment mode, the service won't return the
            // words with `Insertion` or `Omission`
            // even if miscue is enabled.
            // We need to compare with the reference text after received all recognized
            // words to get these error words.
            if (enableMiscue) {

                List<String> referenceWords_aligned = alignListsWithDiffHandling(Arrays.asList(referenceWords),
                        recognizedWords);

                Patch<String> diff = DiffUtils.diff(referenceWords_aligned, recognizedWords, true);

                int currentIdx = 0;
                for (AbstractDelta<String> d : diff.getDeltas()) {
                    if (d.getType() == DeltaType.EQUAL) {
                        for (int i = currentIdx; i < currentIdx + d.getSource().size(); i++) {
                            finalWords.add(pronWords.get(i));
                        }
                        currentIdx += d.getTarget().size();
                    }
                    if (d.getType() == DeltaType.DELETE || d.getType() == DeltaType.CHANGE) {
                        for (String w : d.getSource().getLines()) {
                            finalWords.add(new Word(w, "Omission"));
                        }
                    }
                    if (d.getType() == DeltaType.INSERT || d.getType() == DeltaType.CHANGE) {
                        for (int i = currentIdx; i < currentIdx + d.getTarget().size(); i++) {
                            Word w = pronWords.get(i);
                            w.errorType = "Insertion";
                            finalWords.add(w);
                        }
                        currentIdx += d.getTarget().size();
                    }
                }
            } else {
                finalWords = pronWords;
            }

            // If accuracy score is below 60 and errorType is None, mark as mispronunciation
            for (Word word : finalWords) {
                if ("None".equals(word.errorType) && word.accuracyScore < 60) {
                    word.errorType = "Mispronunciation";
                }
            }

            // We can calculate whole accuracy by averaging
            double totalAccuracyScore = 0;
            int accuracyCount = 0;
            int validCount = 0;
            long durationSum = 0;
            for (Word word : finalWords) {
                if (!"Insertion".equals(word.errorType)) {
                    totalAccuracyScore += word.accuracyScore;
                    accuracyCount += 1;
                }

                if ("None".equals(word.errorType)) {
                    validCount += 1;
                    durationSum += word.duration + 100000;
                }
            }
            double accuracyScore = totalAccuracyScore / accuracyCount;

            // Re-calculate fluency score
            double fluencyScore = durationSum * 1.0 / (start_end_offsets.getLast() - start_end_offsets.getFirst())
                    * 100;

            // Re-calculate prosody score
            double prosodyScoreSum = 0;
            for (int i = 0; i < prosodyScores.size(); i++) {
                prosodyScoreSum += prosodyScores.get(i);
            }
            double prosodyScore = prosodyScoreSum / prosodyScores.size();

            // Calculate whole completeness score
            double completenessScore = 0.0;
            if (!unScriptedScenario) {
                completenessScore = (double) validCount / accuracyCount * 100;
                completenessScore = completenessScore <= 100 ? completenessScore : 100;
            } else {
                completenessScore = 100;
            }

            double pronunciationScore = 0.0;
            if (!unScriptedScenario) {
                // Scripted scenario
                if (enableProsodyAssessment && !Double.isNaN(prosodyScore)) {
                    double[] scores_all = { accuracyScore, prosodyScore, completenessScore, fluencyScore };
                    double minValue = Arrays.stream(scores_all).min().orElse(0.0);
                    pronunciationScore = Arrays.stream(scores_all).sum() * 0.2 + minValue * 0.2;
                } else {
                    double[] scores_all = { accuracyScore, completenessScore, fluencyScore };
                    double minValue = Arrays.stream(scores_all).min().orElse(0.0);
                    pronunciationScore = Arrays.stream(scores_all).sum() * 0.2 + minValue * 0.4;
                }
            } else {
                // Unscripted scenario
                if (enableProsodyAssessment && !Double.isNaN(prosodyScore)) {
                    double[] scores_all = { accuracyScore, prosodyScore, fluencyScore };
                    double minValue = Arrays.stream(scores_all).min().orElse(0.0);
                    pronunciationScore = Arrays.stream(scores_all).sum() * 0.2 + minValue * 0.4;
                } else {
                    double[] scores_all = { accuracyScore, fluencyScore };
                    double minValue = Arrays.stream(scores_all).min().orElse(0.0);
                    pronunciationScore = Arrays.stream(scores_all).sum() * 0.4 + minValue * 0.2;
                }
            }

            System.out.println(String.format(
                    "Paragraph accuracy score: %.0f, prosody score: %.0f, fluency score: %.0f, completeness score: %.0f, pronunciation score: %.0f",
                    accuracyScore, prosodyScore, fluencyScore, completenessScore, pronunciationScore));
            for (Word w : finalWords) {
                System.out.println(" word: " + w.word + "\taccuracy score: " + Math.round(w.accuracyScore)
                        + "\terror type: " + w.errorType);
            }
        }
        config.close();
        audioInput.close();
        recognizer.close();
    }

    // Performs pronunciation assessment asynchronously with REST API for a short
    // audio file.
    // See more information at
    // https://learn.microsoft.com/azure/ai-services/speech-service/rest-speech-to-text-short
    public static void pronunciationAssessmentWithRestApi() throws Exception {

        String serviceRegion = "YourSubscriptionRegion";
        String serviceKey = "YourSubscriptionKey";

        // Build pronunciation assessment parameters
        String locale = "en-US";
        String referenceText = "Good morning.";
        String audioFilePath = "src/resources/good_morning.pcm";
        boolean enableProsodyAssessment = true;
        String phonemeAlphabet = "SAPI"; // IPA or SAPI
        boolean enableMiscue = true;
        int nbestPhonemeCount = 5;
        String pronAssessmentParamsJson = String.format(
                "{\"GradingSystem\":\"HundredMark\",\"Dimension\":\"Comprehensive\",\"ReferenceText\":\"%s\","
                        + "\"EnableProsodyAssessment\":\"%s\",\"PhonemeAlphabet\":\"%s\",\"EnableMiscue\":\"%s\","
                        + "\"NBestPhonemeCount\":\"%s\"}",
                referenceText, enableProsodyAssessment, phonemeAlphabet, enableMiscue, nbestPhonemeCount);

        String pronAssessmentParamsBase64 = Base64.getEncoder()
                .encodeToString(pronAssessmentParamsJson.getBytes("UTF-8"));

        String sessionId = UUID.randomUUID().toString().replace("-", "");

        String urlStr = String.format(
                "https://%s.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1?format=detailed&language=%s&X-ConnectionId=%s",
                serviceRegion, locale, sessionId);

        System.out.println("II URL: " + urlStr);
        System.out.println("II Config: " + pronAssessmentParamsJson);

        File file = new File(audioFilePath);
        InputStream audioStream = new BufferedInputStream(Files.newInputStream(file.toPath()));

        URL url = new URL(urlStr);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("POST");
        conn.setDoOutput(true);
        conn.setChunkedStreamingMode(1024);
        conn.setRequestProperty("Accept", "application/json;text/xml");
        conn.setRequestProperty("Connection", "Keep-Alive");
        conn.setRequestProperty("Content-Type", "audio/wav; codecs=audio/pcm; samplerate=16000");
        conn.setRequestProperty("Ocp-Apim-Subscription-Key", serviceKey);
        conn.setRequestProperty("Pronunciation-Assessment", pronAssessmentParamsBase64);
        conn.setRequestProperty("Transfer-Encoding", "chunked");
        conn.setRequestProperty("Expect", "100-continue");

        long uploadFinishTime = 0;
        long getResponseTime;

        try (OutputStream out = conn.getOutputStream()) {
            if (WaveHeader16K16BitMono.length > 0) {
                out.write(WaveHeader16K16BitMono);
            }

            byte[] buffer = new byte[1024];
            int bytesRead;
            while ((bytesRead = audioStream.read(buffer)) != -1) {
                out.write(buffer, 0, bytesRead);
                out.flush();
                Thread.sleep(1024 / 32);
            }
            uploadFinishTime = System.currentTimeMillis();
        }

        int responseCode = conn.getResponseCode();
        getResponseTime = System.currentTimeMillis();
        // Show Session ID
        System.out.println("II Session ID: " + sessionId);

        if (responseCode != 200) {
            System.err.println("EE Error code: " + responseCode);
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getErrorStream()))) {
                reader.lines().forEach(System.err::println);
            }
        } else {
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream()))) {
                StringBuilder response = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    response.append(line);
                }
                System.out.println("II Response: " + response.toString());
            }
        }

        long latency = getResponseTime - uploadFinishTime;
        System.out.println("II Latency: " + latency + "ms");

        audioStream.close();
        conn.disconnect();
    }

    // Aligns tokens from the raw list to the reference list by merging or splitting
    // tokens
    public static List<String> alignRawTokensByRef(List<String> rawList, List<String> refList) {
        int refIdx = 0;
        int rawIdx = 0;
        int refLen = refList.size();
        List<String> alignedRaw = new ArrayList<>();

        // Make a copy to avoid modifying the original list
        List<String> rawCopy = new ArrayList<>(rawList);

        while (rawIdx < rawCopy.size() && refIdx < refLen) {
            boolean mergedSplitDone = false;

            for (int length = 1; length <= rawCopy.size() - rawIdx; length++) {
                if (rawIdx + length > rawCopy.size())
                    break;

                // Merge consecutive tokens
                String mergedRaw = String.join("", rawCopy.subList(rawIdx, rawIdx + length));
                String refWord = refList.get(refIdx);

                if (mergedRaw.contains(refWord)) {
                    String[] parts = mergedRaw.split(java.util.regex.Pattern.quote(refWord), 2);

                    // Handle prefix before refWord
                    if (!parts[0].isEmpty()) {
                        int consumed = 0;
                        int tokenIdx = rawIdx;

                        // Consume characters from rawCopy until we cover all of parts[0]
                        while (consumed < parts[0].length() && tokenIdx < rawCopy.size()) {
                            String token = rawCopy.get(tokenIdx);
                            int remain = parts[0].length() - consumed;

                            if (token.length() <= remain) {
                                // Add it directly to aligned result if the whole token is fully within prefix
                                alignedRaw.add(token);
                                consumed += token.length();
                                tokenIdx++;
                            } else {
                                // Add only the prefix part, the rest will be handled later
                                String prefixPart = token.substring(0, remain);
                                alignedRaw.add(prefixPart);
                                consumed += remain;
                            }
                        }
                    }

                    // Append the matched refWord
                    alignedRaw.add(refWord);

                    // Handle suffix after refWord
                    if (parts.length > 1 && !parts[1].isEmpty()) {
                        rawCopy.set(rawIdx, parts[1]);
                        // Remove extra merged tokens
                        for (int i = 1; i < length; i++) {
                            rawCopy.remove(rawIdx + 1);
                        }
                    } else {
                        // No suffix: remove all merged tokens
                        for (int i = 0; i < length; i++) {
                            rawCopy.remove(rawIdx);
                        }
                    }

                    refIdx++;
                    mergedSplitDone = true;
                }

                if (mergedSplitDone)
                    break;
            }

            if (!mergedSplitDone) {
                refIdx++;
            }
        }

        // Append any remaining raw tokens
        while (rawIdx < rawCopy.size()) {
            alignedRaw.add(rawCopy.get(rawIdx));
            rawIdx++;
        }

        return alignedRaw;
    }

    // Aligns two token lists using DiffUtils.diff and handles differences.
    // Equal segments are copied directly.
    // 'Replace' segments are aligned strictly if identical after joining,
    // otherwise aligned using alignRawTokensByRef().
    // 'Delete' segments from raw are preserved.
    public static List<String> alignListsWithDiffHandling(List<String> raw, List<String> ref) {
        List<String> alignedRaw = new ArrayList<>();

        Patch<String> diff = DiffUtils.diff(raw, ref, true);

        for (AbstractDelta<String> d : diff.getDeltas()) {
            if (d.getType() == DeltaType.EQUAL) {
                alignedRaw.addAll(d.getSource().getLines());
            } else if (d.getType() == DeltaType.DELETE) {
                alignedRaw.addAll(d.getSource().getLines());
            } else if (d.getType() == DeltaType.CHANGE) {
                if (String.join("", d.getSource().getLines()).equals(String.join("", d.getTarget().getLines()))) {
                    alignedRaw.addAll(d.getTarget().getLines());
                } else {
                    List<String> alignPart = alignRawTokensByRef(d.getSource().getLines(), d.getTarget().getLines());
                    alignedRaw.addAll(alignPart);
                }
            }
        }

        return alignedRaw;
    }

    public static final byte[] WaveHeader16K16BitMono = new byte[] { 82, 73, 70, 70, 78, (byte) 128, 0, 0, 87, 65, 86,
            69, 102, 109, 116, 32, 18, 0, 0, 0, 1, 0, 1, 0, (byte) 128, 62, 0, 0, 0, 125, 0, 0, 2, 0, 16, 0, 0, 0, 100,
            97, 116, 97, 0, 0, 0, 0 };

    public static class Word {
        public String word;
        public String errorType;
        public double accuracyScore;
        public double duration;

        public Word(String word, String errorType) {
            this.word = word;
            this.errorType = errorType;
            this.accuracyScore = 0;
            this.duration = 0;
        }

        public Word(String word, String errorType, double accuracyScore) {
            this(word, errorType);
            this.accuracyScore = accuracyScore;
        }

        public Word(String word, String errorType, double accuracyScore, double duration) {
            this(word, errorType, accuracyScore);
            this.duration = duration;
        }
    }
}
