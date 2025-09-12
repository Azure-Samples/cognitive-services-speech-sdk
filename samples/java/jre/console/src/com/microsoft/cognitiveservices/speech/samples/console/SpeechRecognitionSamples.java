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
import java.util.concurrent.TimeUnit;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.StringReader;
import jakarta.json.Json;
import jakarta.json.JsonArray;
import jakarta.json.JsonObject;
import jakarta.json.JsonReader;
import java.net.URI;
import java.net.URISyntaxException;

import com.azure.core.credential.AzureKeyCredential;
import com.azure.core.credential.TokenCredential;
import com.azure.identity.DefaultAzureCredentialBuilder;
import com.github.difflib.DiffUtils;

import com.github.difflib.patch.AbstractDelta;
import com.github.difflib.patch.DeltaType;
import com.github.difflib.patch.Patch;

// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class SpeechRecognitionSamples {

    // Speech recognition from microphone.
    public static void recognitionWithMicrophoneAsync() throws InterruptedException, ExecutionException, URISyntaxException
    {
        // <recognitionWithMicrophone>
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config);
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        recognizer.close();
        // </recognitionWithMicrophone>
    }

    // Speech recognition from microphone, showing detailed recognition results including word-level timing
    public static void recognitionWithMicrophoneAsyncAndDetailedRecognitionResults() throws InterruptedException, ExecutionException, URISyntaxException
    {
        // <recognitionWithMicrophoneAndDetailedRecognitionResults>
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Ask for detailed recognition result
        config.setOutputFormat(OutputFormat.Detailed);

        // If you also want word-level timing in the detailed recognition results, set the following.
        // Note that if you set the following, you can omit the previous line
        //   "config.setOutputFormat(OutputFormat.Detailed)",
        // since word-level timing implies detailed recognition results.
        config.requestWordLevelTimestamps();

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config);
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text = " + result.getText());

                // Time units are in hundreds of nanoseconds (HNS), where 10000 HNS equals 1 millisecond
                System.out.println("Offset: " + result.getOffset());
                System.out.println("Duration: " + result.getDuration());

                // Now get the detailed recognition results as a JSON string
                String jsonText = result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);

                // Convert the JSON string to a JSON object
                JsonReader jsonReader = Json.createReader(new StringReader(jsonText));

                // Extract the "NBest" array of recognition results from the JSON.
                // Note that the first cell in the NBest array corresponds to the recognition results
                // (NOT the cell with the highest confidence number!)
                JsonArray nbestArray = jsonReader.readObject().getJsonArray("NBest");

                for (int i = 0; i < nbestArray.size(); i++) {
                    JsonObject nbestItem = nbestArray.getJsonObject(i);
                    System.out.println("\tConfidence: " + nbestItem.getJsonNumber("Confidence"));
                    System.out.println("\tLexical: " + nbestItem.getString("Lexical"));
                    // ITN stands for Inverse Text Normalization
                    System.out.println("\tITN: " + nbestItem.getString("ITN"));
                    System.out.println("\tMaskedITN: " + nbestItem.getString("MaskedITN"));
                    System.out.println("\tDisplay: " + nbestItem.getString("Display"));

                    // Word-level timing
                    JsonArray wordsArray = nbestItem.getJsonArray("Words");
                    if (wordsArray != null) {
                        System.out.println("\t\tWord | Offset | Duration");
                        for (int j = 0; j < wordsArray.size(); j++) {
                            JsonObject wordItem = wordsArray.getJsonObject(j);
                            System.out.println("\t\t" + wordItem.getString("Word") + " " + wordItem.getJsonNumber("Offset") + " " + wordItem.getJsonNumber("Duration"));
                        }
                    } else {
                        System.out.println("\t\tNo word-level timing information available.");
                    }
                }

                jsonReader.close();
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        recognizer.close();
        // </recognitionWithMicrophoneAndDetailedRecognitionResults>
    }

    // Speech recognition in the specified spoken language.
    public static void recognitionWithLanguageAsync() throws InterruptedException, ExecutionException, URISyntaxException
    {
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates a speech recognizer for the specified language, using microphone as audio input.
        String lang = "de-de";
        SpeechRecognizer recognizer = new SpeechRecognizer(config, lang);
        {
            // Starts recognizing.
            System.out.println("Say something in " + lang + " ...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }
            result.close();
        }

        config.close();
        recognizer.close();
    }

    // Speech recognition using a customized model.
    public static void recognitionUsingCustomizedModelAsync() throws InterruptedException, ExecutionException, URISyntaxException
    {
        // <recognitionCustomized>
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");
        // Replace with the custom endpoint id of your customized model.
        config.setEndpointId("YourEndpointId");

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config);
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        recognizer.close();
        // </recognitionCustomized>
    }

    // Speech recognition with events from file
    public static void continuousRecognitionWithFileAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // <recognitionContinuousWithFile>
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates a speech recognizer using file as audio input.
        // Replace with your own audio file name.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourAudioFile.wav");

        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        recognizer.close();
        // </recognitionContinuousWithFile>
    }

    // The Source to stop recognition.
    private static Semaphore stopRecognitionSemaphore;

    // Speech recognition with audio stream
    public static void recognitionWithAudioStreamAsync() throws InterruptedException, ExecutionException, FileNotFoundException, URISyntaxException
    {
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Create an object that parses the WAV file and implements PullAudioInputStreamCallback to read audio data from the file.
        // Replace with your own audio file name.
        WavStream wavStream = new WavStream(new FileInputStream("YourAudioFile.wav"));

        // Create a pull audio input stream from the WAV file
        PullAudioInputStream inputStream = PullAudioInputStream.createPullStream(wavStream, wavStream.getFormat());

        // Create a configuration object for the recognizer, to read from the pull audio input stream
        AudioConfig audioInput = AudioConfig.fromStreamInput(inputStream);

        // Creates a speech recognizer using audio stream input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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
                System.out.println("\nSession started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\nSession stopped event.");

                // Stops recognition when session stop is detected.
                System.out.println("\nStop recognition.");
                stopRecognitionSemaphore.release();
            });

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            // Stops recognition.
            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        recognizer.close();
    }

    // Speech recognition with events from a push stream
    // This sample takes and existing file and reads it by chunk into a local buffer and then pushes the
    // buffer into an PushAudioStream for speech recognition.
    public static void continuousRecognitionWithPushStream() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Replace with your own audio file name.
        // The input stream the sample will read from.
        // The default format for a PushStream is 16Khz, 16 bit mono.
        // You can use a different format by passing an AudioStreamFormat into createPushStream.
        InputStream inputStream = new FileInputStream("YourAudioFile.wav");

        // Create the push stream to push audio to.
        PushAudioInputStream pushStream = AudioInputStream.createPushStream();

        // Creates a speech recognizer using Push Stream as audio input.
        AudioConfig audioInput = AudioConfig.fromStreamInput(pushStream);

        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Arbitrary buffer size.
            byte[] readBuffer = new byte[4096];

            // Push audio read from the file into the PushStream.
            // The audio can be pushed into the stream before, after, or during recognition
            // and recognition will continue as data becomes available.
            int bytesRead;
            while ((bytesRead = inputStream.read(readBuffer)) != -1)
            {
                if (bytesRead == readBuffer.length)
                {
                    pushStream.write(readBuffer);
                }
                else
                {
                    // Last buffer read from the WAV file is likely to have less bytes
                    pushStream.write(Arrays.copyOfRange(readBuffer, 0, bytesRead));
                }
            }

            pushStream.close();
            inputStream.close();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        recognizer.close();
    }

    // Keyword-triggered speech recognition from microphone
    public static void keywordTriggeredSpeechRecognitionWithMicrophone() throws InterruptedException, ExecutionException, URISyntaxException
    {
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizingKeyword) {
                    System.out.println("RECOGNIZING KEYWORD: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.RecognizingSpeech) {
                    System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
                }
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\n    Session started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\n    Session stopped event.");

                stopRecognitionSemaphore.release();
            });

            // Creates an instance of a keyword recognition model. Update this to
            // point to the location of your keyword recognition model.
            KeywordRecognitionModel model = KeywordRecognitionModel.fromFile("YourKeywordRecognitionModelFile.table");

            // The phrase your keyword recognition model triggers on.
            String keyword = "YourKeyword";

            // Starts continuous recognition using the keyword model. Use
            // stopKeywordRecognitionAsync() to stop recognition.
            recognizer.startKeywordRecognitionAsync(model).get();

            System.out.println("Say something starting with '" + keyword + "' followed by whatever you want...");

            // Waits for a single successful keyword-triggered speech recognition (or error).
            stopRecognitionSemaphore.acquire();

            recognizer.stopKeywordRecognitionAsync().get();
        }

        config.close();
        recognizer.close();
    }

    // Speech recognition with events from file
    public static void continuousRecognitionWithFileWithPhraseListAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        stopRecognitionSemaphore = new Semaphore(0);

        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates a speech recognizer using file as audio input.
        // Replace with your own audio file name.
        // The audio file wreck-a-nice-beach.wav included with the C# sample contains ambigious audio.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourPhraseListedAudioFile.wav");
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Create the recognizer.
            PhraseListGrammar phraseList = PhraseListGrammar.fromRecognizer(recognizer);

            // Add a phrase to assist in recognition.
            phraseList.addPhrase("Wreck a nice beach");

            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        recognizer.close();
    }

    // Shows how to recognize one utterance from an audio file, with at-start language detection.
    // We assume the utterance is spoken in either English (US), Spanish (Mexico) or German.
    // Speech recognition will use the standard recognition model associated with the detected language.
    // <SpeechRecognizeOnceAndLanguageId>
    public static void recognizeOnceFromFileWithAtStartLanguageDetection() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Define a set of expected spoken languages in the audio. Update the below with your own languages.
        // Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support for all supported languages.
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromLanguages(Arrays.asList("en-US", "es-MX", "de-DE"));

        // We provide a WAV file with Spanish speech as an example. Replace it with your own.
        AudioConfig audioConfig = AudioConfig.fromWavFileInput("es-mx.wav");

        // Creates a speech recognizer using file as audio input and the AutoDetectSourceLanguageConfig
        SpeechRecognizer speechRecognizer = new SpeechRecognizer(config, autoDetectSourceLanguageConfig, audioConfig);

        // Starts recognition. It returns when the first utterance has been recognized.
        System.out.println(" Recognizing from WAV file... please wait");
        SpeechRecognitionResult result = speechRecognizer.recognizeOnceAsync().get();

        // Checks result.
        if (result.getReason() == ResultReason.RecognizedSpeech) {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(result);
            String language = autoDetectSourceLanguageResult.getLanguage();
            System.out.println(" RECOGNIZED: Text = " + result.getText());
            System.out.println(" RECOGNIZED: Language = " + language);
        }
        else if (result.getReason() == ResultReason.NoMatch) {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(result);
            String language = autoDetectSourceLanguageResult.getLanguage();
            if (language == null || language.isEmpty() || language.toLowerCase().equals("unknown")) {
                System.out.println(" NOMATCH: Speech Language could not be detected.");
            }
            else {
                System.out.println(" NOMATCH: Speech could not be recognized.");
            }
        }
        else if (result.getReason() == ResultReason.Canceled) {
            CancellationDetails cancellation = CancellationDetails.fromResult(result);
            System.out.println(" CANCELED: Reason = " + cancellation.getReason());

            if (cancellation.getReason() == CancellationReason.Error) {
                System.out.println(" CANCELED: ErrorCode = " + cancellation.getErrorCode());
                System.out.println(" CANCELED: ErrorDetails = " + cancellation.getErrorDetails());
                System.out.println(" CANCELED: Did you update the subscription info?");
            }
        }

        // These objects must be closed in order to dispose underlying native resources
        result.close();
        speechRecognizer.close();
        config.close();
        audioConfig.close();
        autoDetectSourceLanguageConfig.close();
    }
    // </SpeechRecognizeOnceAndLanguageId>

    // Shows how to do continuous speech recognition from an audio file, with at-start language detection.
    // We assume the audio spoken is either English (US), Spanish (Mexico) or German. The language does not change.
    // Speech recognition will use the standard recognition model associated with the detected language.
    public static void continuousRecognitionFromFileWithAtStartLanguageDetection() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Define a set of expected spoken languages in the audio. Update the below with your own languages.
        // Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support for all supported languages.
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromLanguages(Arrays.asList("en-US", "es-MX", "de-DE"));

        // We provide a WAV file with Spanish speech as an example. Replace it with your own.
        AudioConfig audioConfig = AudioConfig.fromWavFileInput("es-mx.wav");

        // Creates a speech recognizer using file as audio input and the AutoDetectSourceLanguageConfig
        SpeechRecognizer speechRecognizer = new SpeechRecognizer(config, autoDetectSourceLanguageConfig, audioConfig);

        // Semaphore used to signal the call to stop continuous recognition (following either a session ended or a cancelled event)
        final Semaphore doneSemaphone = new Semaphore(0);

        // Subscribes to events.
        speechRecognizer.recognizing.addEventListener((s, e) -> {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
            String language = autoDetectSourceLanguageResult.getLanguage();
            System.out.println(" RECOGNIZING: Text = " + e.getResult().getText());
            System.out.println(" RECOGNIZING: Language = " + language);
        });

        speechRecognizer.recognized.addEventListener((s, e) -> {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
            String language = autoDetectSourceLanguageResult.getLanguage();
            if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                System.out.println(" RECOGNIZED: Text = " + e.getResult().getText());
                System.out.println(" RECOGNIZED: Language = " + language);
            }
            else if (e.getResult().getReason() == ResultReason.NoMatch) {
                if (language == null || language.isEmpty() || language.toLowerCase().equals("unknown")) {
                    System.out.println(" NOMATCH: Speech Language could not be detected.");
                }
                else {
                    System.out.println(" NOMATCH: Speech could not be recognized.");
                }
            }
        });

        speechRecognizer.canceled.addEventListener((s, e) -> {
            System.out.println(" CANCELED: Reason = " + e.getReason());
            if (e.getReason() == CancellationReason.Error) {
                System.out.println(" CANCELED: ErrorCode = " + e.getErrorCode());
                System.out.println(" CANCELED: ErrorDetails = " + e.getErrorDetails());
                System.out.println(" CANCELED: Did you update the subscription info?");
            }
            doneSemaphone.release();
        });

        speechRecognizer.sessionStarted.addEventListener((s, e) -> {
            System.out.println("\n Session started event.");
        });

        speechRecognizer.sessionStopped.addEventListener((s, e) -> {
            System.out.println("\n Session stopped event.");
            doneSemaphone.release();
        });

        // Starts continuous recognition and wait for processing to end
        System.out.println(" Recognizing from WAV file... please wait");
        speechRecognizer.startContinuousRecognitionAsync().get();
        doneSemaphone.tryAcquire(30, TimeUnit.SECONDS);

        // Stop continuous recognition
        speechRecognizer.stopContinuousRecognitionAsync().get();

        // These objects must be closed in order to dispose underlying native resources
        speechRecognizer.close();
        config.close();
        audioConfig.close();
        autoDetectSourceLanguageConfig.close();
    }

    // Shows how to do continuous speech recognition from an audio file, with at-start language detection.
    // We assume the audio spoken is either English (US), Spanish (Mexico) or German. The language does not change.
    // Speech recognition will use the appropriate custom model specified, associated with the detected language.
    public static void continuousRecognitionFromFileWithAtStartLanguageDetectionWithCustomModels() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig speechConfig = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Define a set of expected spoken languages in the audio, with an optional custom model endpoint ID associated with each.
        // Update the below with your own languages. Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
        // for all supported languages.
        // Update the below with your own custom model endpoint IDs, or omit it if you want to use the standard model.
        List<SourceLanguageConfig> sourceLanguageConfigs = new ArrayList<SourceLanguageConfig>();
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("en-US", "YourEnUSCustomModelID"));
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("es-MX", "YourEsMxCustomModelID"));
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("de-DE"));

        // Creates an instance of AutoDetectSourceLanguageConfig with the above 3 source language configurations.
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromSourceLanguageConfigs(sourceLanguageConfigs);

        // We provide an example WAV file with this sample. Replace with your own multilingual audio file name.
        AudioConfig audioConfig = AudioConfig.fromWavFileInput("es-mx.wav");

        // Creates a speech recognizer using file as audio input and the AutoDetectSourceLanguageConfig
        SpeechRecognizer speechRecognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, audioConfig);

        // Semaphore used to signal the call to stop continuous recognition (following either a session ended or a cancelled event)
        final Semaphore doneSemaphone = new Semaphore(0);

        // Subscribes to events.
        speechRecognizer.recognizing.addEventListener((s, e) -> {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
            String language = autoDetectSourceLanguageResult.getLanguage();
            System.out.println(" RECOGNIZING: Text = " + e.getResult().getText());
            System.out.println(" RECOGNIZING: Language = " +language);
        });

        speechRecognizer.recognized.addEventListener((s, e) -> {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
            String language = autoDetectSourceLanguageResult.getLanguage();
            if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                System.out.println(" RECOGNIZED: Text = " + e.getResult().getText());
                System.out.println(" RECOGNIZED: Language = " + language);
            }
            else if (e.getResult().getReason() == ResultReason.NoMatch) {
                if (language == null || language.isEmpty() || language.toLowerCase().equals("unknown")) {
                    System.out.println(" NOMATCH: Speech Language could not be detected.");
                }
                else {
                    System.out.println(" NOMATCH: Speech could not be recognized.");
                }
            }
        });

        speechRecognizer.canceled.addEventListener((s, e) -> {
            System.out.println(" CANCELED: Reason = " + e.getReason());
            if (e.getReason() == CancellationReason.Error) {
                System.out.println(" CANCELED: ErrorCode = " + e.getErrorCode());
                System.out.println(" CANCELED: ErrorDetails = " + e.getErrorDetails());
                System.out.println(" CANCELED: Did you update the subscription info?");
            }
            doneSemaphone.release();
        });

        speechRecognizer.sessionStarted.addEventListener((s, e) -> {
            System.out.println("\n Session started event.");
        });

        speechRecognizer.sessionStopped.addEventListener((s, e) -> {
            System.out.println("\n Session stopped event.");
            doneSemaphone.release();
        });

        // Starts continuous recognition and wait for processing to end
        System.out.println(" Recognizing from WAV file... please wait");
        speechRecognizer.startContinuousRecognitionAsync().get();
        doneSemaphone.tryAcquire(30, TimeUnit.SECONDS);

        // Stop continuous recognition
        speechRecognizer.stopContinuousRecognitionAsync().get();

        // These objects must be closed in order to dispose underlying native resources
        speechRecognizer.close();
        speechConfig.close();
        audioConfig.close();
        for (SourceLanguageConfig sourceLanguageConfig : sourceLanguageConfigs)
        {
            sourceLanguageConfig.close();
        }
        autoDetectSourceLanguageConfig.close();
    }

    // <SpeechContinuousRecognitionAndLanguageId>
    // Shows how to do continuous speech recognition on a multilingual audio file with continuous language detection. Here, we assume the
    // spoken language in the file can alternate between English (US), Spanish (Mexico) and German.
    // If specified, speech recognition will use the custom model associated with the detected language.
    public static void continuousRecognitionFromFileWithContinuousLanguageDetectionWithCustomModels() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // Creates an instance of a speech config with specified
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig speechConfig = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Change the default from at-start language detection to continuous language detection, since the spoken language in the audio
        // may change.
        speechConfig.setProperty(PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous");

        // Define a set of expected spoken languages in the audio, with an optional custom model endpoint ID associated with each.
        // Update the below with your own languages. Please see https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
        // for all supported languages.
        // Update the below with your own custom model endpoint IDs, or omit it if you want to use the standard model.
        List<SourceLanguageConfig> sourceLanguageConfigs = new ArrayList<SourceLanguageConfig>();
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("en-US", "YourEnUsCustomModelID"));
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("es-MX", "YourEsMxCustomModelID"));
        sourceLanguageConfigs.add(SourceLanguageConfig.fromLanguage("de-DE"));

        // Creates an instance of AutoDetectSourceLanguageConfig with the above 3 source language configurations.
        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromSourceLanguageConfigs(sourceLanguageConfigs);

        // We provide a WAV file with English and Spanish utterances as an example. Replace with your own multilingual audio file name.
        AudioConfig audioConfig = AudioConfig.fromWavFileInput( "es-mx_en-us.wav");

        // Creates a speech recognizer using file as audio input and the AutoDetectSourceLanguageConfig
        SpeechRecognizer speechRecognizer = new SpeechRecognizer(speechConfig, autoDetectSourceLanguageConfig, audioConfig);

        // Semaphore used to signal the call to stop continuous recognition (following either a session ended or a cancelled event)
        final Semaphore doneSemaphone = new Semaphore(0);

        // Subscribes to events.

        /* Uncomment this to see intermediate recognition results. Since this is verbose and the WAV file is long, it is commented out by default in this sample.
        speechRecognizer.recognizing.addEventListener((s, e) -> {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
            String language = autoDetectSourceLanguageResult.getLanguage();
            System.out.println(" RECOGNIZING: Text = " + e.getResult().getText());
            System.out.println(" RECOGNIZING: Language = " + language);
        });
        */

        speechRecognizer.recognized.addEventListener((s, e) -> {
            AutoDetectSourceLanguageResult autoDetectSourceLanguageResult = AutoDetectSourceLanguageResult.fromResult(e.getResult());
            String language = autoDetectSourceLanguageResult.getLanguage();
            if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                System.out.println(" RECOGNIZED: Text = " + e.getResult().getText());
                System.out.println(" RECOGNIZED: Language = " + language);
            }
            else if (e.getResult().getReason() == ResultReason.NoMatch) {
                if (language == null || language.isEmpty() || language.toLowerCase().equals("unknown")) {
                    System.out.println(" NOMATCH: Speech Language could not be detected.");
                }
                else {
                    System.out.println(" NOMATCH: Speech could not be recognized.");
                }
            }
        });

        speechRecognizer.canceled.addEventListener((s, e) -> {
            System.out.println(" CANCELED: Reason = " + e.getReason());
            if (e.getReason() == CancellationReason.Error) {
                System.out.println(" CANCELED: ErrorCode = " + e.getErrorCode());
                System.out.println(" CANCELED: ErrorDetails = " + e.getErrorDetails());
                System.out.println(" CANCELED: Did you update the subscription info?");
            }
            doneSemaphone.release();
        });

        speechRecognizer.sessionStarted.addEventListener((s, e) -> {
            System.out.println("\n Session started event.");
        });

        speechRecognizer.sessionStopped.addEventListener((s, e) -> {
            System.out.println("\n Session stopped event.");
            doneSemaphone.release();
        });

        // Starts continuous recognition and wait for processing to end
        System.out.println(" Recognizing from WAV file... please wait");
        speechRecognizer.startContinuousRecognitionAsync().get();
        doneSemaphone.tryAcquire(30, TimeUnit.SECONDS);

        // Stop continuous recognition
        speechRecognizer.stopContinuousRecognitionAsync().get();

        // These objects must be closed in order to dispose underlying native resources
        speechRecognizer.close();
        speechConfig.close();
        audioConfig.close();
        for (SourceLanguageConfig sourceLanguageConfig : sourceLanguageConfigs)
        {
            sourceLanguageConfig.close();
        }
        autoDetectSourceLanguageConfig.close();
    }
    // </SpeechContinuousRecognitionAndLanguageId>

    // Speech recognition from default microphone with Microsoft Audio Stack enabled.
    public static void continuousRecognitionFromDefaultMicrophoneWithMASEnabled() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");


        // Creates an instance of audio config using default microphone as audio input and with audio processing options specified.
        // All default enhancements from Microsoft Audio Stack are enabled.
        // Only works when input is from a microphone array.
        // On Windows, microphone array geometry is obtained from the driver. On other operating systems, a single channel (mono)
        // microphone is assumed.
        AudioProcessingOptions audioProcessingOptions = AudioProcessingOptions.create(AudioProcessingConstants.AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT);
        AudioConfig audioInput = AudioConfig.fromDefaultMicrophoneInput(audioProcessingOptions);

        // Creates a speech recognizer.
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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
            });

            recognizer.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\n    Session started event.");
            });

            recognizer.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\n    Session stopped event.");
            });

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            System.out.println("Say something...");
            recognizer.startContinuousRecognitionAsync().get();

            System.out.println("Press any key to stop");
            new Scanner(System.in).nextLine();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        audioProcessingOptions.close();
        recognizer.close();
    }

    // Speech recognition from a microphone with Microsoft Audio Stack enabled and pre-defined microphone array geometry specified.
    public static void recognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates an instance of audio config using a microphone as audio input and with audio processing options specified.
        // All default enhancements from Microsoft Audio Stack are enabled and preset microphone array geometry is specified
        // in audio processing options.
        AudioProcessingOptions audioProcessingOptions = AudioProcessingOptions.create(AudioProcessingConstants.AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT,
                                                                                      PresetMicrophoneArrayGeometry.Linear2);
        AudioConfig audioInput = AudioConfig.fromMicrophoneInput("<device id>", audioProcessingOptions);

        // Creates a speech recognizer.
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        audioInput.close();
        audioProcessingOptions.close();
        recognizer.close();
    }

    // Speech recognition from multi-channel file with Microsoft Audio Stack enabled and custom microphone array geometry specified.
    public static void continuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        stopRecognitionSemaphore = new Semaphore(0);

        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Approximate coordinates for a microphone array with one microphone in the center and six microphones evenly spaced
        // in a circle with radius approximately equal to 42.5 mm.
        MicrophoneCoordinates[] microphoneCoordinates = new MicrophoneCoordinates[7];
        microphoneCoordinates[0] = new MicrophoneCoordinates(0, 0, 0);
        microphoneCoordinates[1] = new MicrophoneCoordinates(40, 0, 0);
        microphoneCoordinates[2] = new MicrophoneCoordinates(20, -35, 0);
        microphoneCoordinates[3] = new MicrophoneCoordinates(-20, -35, 0);
        microphoneCoordinates[4] = new MicrophoneCoordinates(-40, 0, 0);
        microphoneCoordinates[5] = new MicrophoneCoordinates(-20, 35, 0);
        microphoneCoordinates[6] = new MicrophoneCoordinates(20, 35, 0);

        // Creates an instance of audio config using multi-channel WAV file as audio input and with audio processing options specified.
        // All default enhancements from Microsoft Audio Stack are enabled and custom microphone array geometry is provided.
        MicrophoneArrayGeometry microphoneArrayGeometry = new MicrophoneArrayGeometry(MicrophoneArrayType.Planar, microphoneCoordinates);
        AudioProcessingOptions audioProcessingOptions = AudioProcessingOptions.create(AudioProcessingConstants.AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT,
                                                                                      microphoneArrayGeometry,
                                                                                      SpeakerReferenceChannel.LastChannel);
        AudioConfig audioInput = AudioConfig.fromWavFileInput("katiesteve.wav", audioProcessingOptions);

        // Creates a speech recognizer.
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        audioProcessingOptions.close();
        recognizer.close();
    }

    // Speech recognition from pull stream with custom set of enhancements from Microsoft Audio Stack enabled.
    public static void recognitionFromPullStreamWithSelectMASEnhancementsEnabled() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Creates a callback that will read audio data from a WAV file.
        // Microsoft Audio Stack supports sample rates that are integral multiples of 16 KHz. Additionally, the following
        // formats are supported: 32-bit IEEE little endian float, 32-bit little endian signed int, 24-bit little endian signed int,
        // 16-bit little endian signed int, and 8-bit signed int.
        // Replace with your own audio file name.
        PullAudioInputStreamCallback callback = new WavStream(new FileInputStream("whatstheweatherlike.wav"));

        // Creates an instance of audio config with pull stream as audio input and with audio processing options specified.
        // All default enhancements from Microsoft Audio Stack are enabled except acoustic echo cancellation and preset
        // microphone array geometry is specified in audio processing options.
        AudioProcessingOptions audioProcessingOptions = AudioProcessingOptions.create(AudioProcessingConstants.AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT | AudioProcessingConstants.AUDIO_INPUT_PROCESSING_DISABLE_ECHO_CANCELLATION,
                                                                                      PresetMicrophoneArrayGeometry.Mono);
        AudioConfig audioInput = AudioConfig.fromStreamInput(callback, audioProcessingOptions);

        // Creates a speech recognizer.
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        audioInput.close();
        audioProcessingOptions.close();
        recognizer.close();
    }

    // Speech recognition from push stream with Microsoft Audio Stack enabled and beamforming angles specified.
    public static void continuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        stopRecognitionSemaphore = new Semaphore(0);

        // subscription key and endpoint URL. Replace with your own subscription key
        // and endpoint URL.
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourEndpointUrl"), "YourSubscriptionKey");

        // Create the push stream to push audio to.
        PushAudioInputStream pushStream = AudioInputStream.createPushStream(AudioStreamFormat.getWaveFormatPCM((long)16000, (short)16, (short)8));

        // The input stream the sample will read from.
        InputStream inputStream = new FileInputStream("katiesteve.wav");

        // Approximate coordinates for a microphone array with one microphone in the center and six microphones evenly spaced
        // in a circle with radius approximately equal to 42.5 mm.
        MicrophoneCoordinates[] microphoneCoordinates = new MicrophoneCoordinates[7];
        microphoneCoordinates[0] = new MicrophoneCoordinates(0, 0, 0);
        microphoneCoordinates[1] = new MicrophoneCoordinates(40, 0, 0);
        microphoneCoordinates[2] = new MicrophoneCoordinates(20, -35, 0);
        microphoneCoordinates[3] = new MicrophoneCoordinates(-20, -35, 0);
        microphoneCoordinates[4] = new MicrophoneCoordinates(-40, 0, 0);
        microphoneCoordinates[5] = new MicrophoneCoordinates(-20, 35, 0);
        microphoneCoordinates[6] = new MicrophoneCoordinates(20, 35, 0);

        // Creates an instance of audio config with push stream as audio input and with audio processing options specified.
        // All default enhancements from Microsoft Audio Stack are enabled and custom microphone array geometry with beamforming
        // angles is specified.
        MicrophoneArrayGeometry microphoneArrayGeometry = new MicrophoneArrayGeometry(MicrophoneArrayType.Planar, 70, 110, microphoneCoordinates);
        AudioProcessingOptions audioProcessingOptions = AudioProcessingOptions.create(AudioProcessingConstants.AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT,
                                                                                      microphoneArrayGeometry,
                                                                                      SpeakerReferenceChannel.LastChannel);
        AudioConfig audioInput = AudioConfig.fromStreamInput(pushStream, audioProcessingOptions);

        // Creates a speech recognizer.
        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Arbitrary buffer size.
            byte[] readBuffer = new byte[4096];

            // Push audio read from the file into the PushStream.
            // The audio can be pushed into the stream before, after, or during recognition
            // and recognition will continue as data becomes available.
            int bytesRead;
            while ((bytesRead = inputStream.read(readBuffer)) != -1)
            {
                if (bytesRead == readBuffer.length)
                {
                    pushStream.write(readBuffer);
                }
                else
                {
                    // Last buffer read from the WAV file is likely to have less bytes.
                    pushStream.write(Arrays.copyOfRange(readBuffer, 0, bytesRead));
                }
            }

            pushStream.close();
            inputStream.close();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        audioProcessingOptions.close();
        recognizer.close();
    }

    // Speech recognition authenticated via aad token crendential.
    public static void recognitionWithAADTokenCredentialAsync() throws InterruptedException, ExecutionException, URISyntaxException
    {

        // Use an appropriate instance of token credential. See Azure Core credentials docs for details:
        // https://learn.microsoft.com/java/api/com.azure.core.credential.tokencredential?view=azure-java-stable
        TokenCredential credential = new DefaultAzureCredentialBuilder().build();

        // Creates an instance of a speech config with specified token credential and
        // endpoint URL with custom domain(see: https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name). 
        // Replace with your own endpoint URL.
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourCustomDomainEndpointUrl"), credential);

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config);
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        recognizer.close();
    }

    // Speech continuous recognition authenticated via aad token crendential.
    public static void continuousRecognitionWithAADTokenCredentialAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        // Use an appropriate instance of token credential. See Azure Core credentials docs for details:
        // https://learn.microsoft.com/java/api/com.azure.core.credential.tokencredential?view=azure-java-stable
        TokenCredential credential = new DefaultAzureCredentialBuilder().build();

        // Creates an instance of a speech config with specified token credential and
        // endpoint URL with custom domain(see: https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name). 
        // Replace with your own endpoint URL.
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourCustomDomainEndpointUrl"), credential);

        // Creates a speech recognizer using file as audio input.
        // Replace with your own audio file name.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourAudioFile.wav");

        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        recognizer.close();
    }

    // Speech recognition authenticated via key crendential.
    public static void recognitionWithKeyCredentialAsync() throws InterruptedException, ExecutionException, URISyntaxException
    {
        AzureKeyCredential azureKeyCredential = new AzureKeyCredential("YourSubscriptionKey");

        // Creates an instance of a speech config with specified key credential and
        // endpoint URL with custom domain(see: https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name). 
        // Replace with your own endpoint URL.
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourCustomDomainEndpointUrl"), azureKeyCredential);

        // Creates a speech recognizer using microphone as audio input.
        SpeechRecognizer recognizer = new SpeechRecognizer(config);
        {
            // Starts recognizing.
            System.out.println("Say something...");

            // Starts recognition. It returns when the first utterance has been recognized.
            SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();

            // Checks result.
            if (result.getReason() == ResultReason.RecognizedSpeech) {
                System.out.println("RECOGNIZED: Text=" + result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch) {
                System.out.println("NOMATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled) {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }

            result.close();
        }

        config.close();
        recognizer.close();
    }

    // Speech continuous recognition authenticated via key crendential.
    public static void continuousRecognitionWithKeyCredentialAsync() throws InterruptedException, ExecutionException, IOException, URISyntaxException
    {
        AzureKeyCredential azureKeyCredential = new AzureKeyCredential("YourSubscriptionKey");

        // Creates an instance of a speech config with specified key credential and
        // endpoint URL with custom domain(see: https://learn.microsoft.com/azure/ai-services/speech-service/speech-services-private-link?tabs=portal#create-a-custom-domain-name). 
        // Replace with your own endpoint URL.
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromEndpoint(new URI("YourCustomDomainEndpointUrl"), azureKeyCredential);

        // Creates a speech recognizer using file as audio input.
        // Replace with your own audio file name.
        AudioConfig audioInput = AudioConfig.fromWavFileInput("YourAudioFile.wav");

        SpeechRecognizer recognizer = new SpeechRecognizer(config, audioInput);
        {
            // Subscribes to events.
            recognizer.recognizing.addEventListener((s, e) -> {
                System.out.println("RECOGNIZING: Text=" + e.getResult().getText());
            });

            recognizer.recognized.addEventListener((s, e) -> {
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("RECOGNIZED: Text=" + e.getResult().getText());
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
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

            // Starts continuous recognition. Uses stopContinuousRecognitionAsync() to stop recognition.
            recognizer.startContinuousRecognitionAsync().get();

            // Waits for completion.
            stopRecognitionSemaphore.acquire();

            recognizer.stopContinuousRecognitionAsync().get();
        }

        config.close();
        audioInput.close();
        recognizer.close();
    }
}
