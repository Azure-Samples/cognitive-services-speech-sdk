//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
package com.microsoft.cognitiveservices.speech.samples.embedded;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;
import org.json.JSONArray;
import org.json.JSONObject;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;


public class SpeechRecognitionSamples
{
    // List available embedded speech recognition models.
    public static void listEmbeddedModels() throws InterruptedException, ExecutionException
    {
        // Creates an instance of an embedded speech config.
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        if (speechConfig == null)
        {
            return;
        }

        // Gets a list of models.
        List<SpeechRecognitionModel> models = speechConfig.getSpeechRecognitionModels();

        if (!models.isEmpty())
        {
            System.out.println("Models found:");
            for (SpeechRecognitionModel model : models)
            {
                System.out.println(model.getName());
                System.out.print(" Locale(s): ");
                for (String locale : model.getLocales())
                {
                    System.out.print(locale + " ");
                }
                System.out.println();
                System.out.println(" Path:      " + model.getPath());
            }

            // To find a model that supports a specific locale, for example:
            /*
            String modelName = null;
            String modelLocale = "en-US";

            for (SpeechRecognitionModel model : models)
            {
                if (model.getLocales().get(0).equals(modelLocale))
                {
                    modelName = model.getName();
                    break;
                }
            }

            if (modelName != null && !modelName.isEmpty())
            {
                System.out.println("Found " + modelLocale + " model: " + modelName);
            }
            */
        }
        else
        {
            System.err.println("No models found. Either the path is not valid or the format of model(s) is unknown.");
        }
    }


    private static void recognizeSpeechAsync(SpeechRecognizer recognizer, boolean useKeyword, boolean waitForUser) throws InterruptedException, ExecutionException
    {
        Semaphore recognitionEnd = new Semaphore(0);

        // Subscribes to events.
        recognizer.recognizing.addEventListener((s, e) ->
        {
            // Intermediate result (hypothesis).
            if (e.getResult().getReason() == ResultReason.RecognizingSpeech)
            {
                System.out.println("Recognizing:" + e.getResult().getText());
            }
            else if (e.getResult().getReason() == ResultReason.RecognizingKeyword)
            {
                // ignored
            }
        });

        recognizer.recognized.addEventListener((s, e) ->
        {
            if (e.getResult().getReason() == ResultReason.RecognizedKeyword)
            {
                // Keyword detected, speech recognition will start.
                System.out.println("KEYWORD: Text=" + e.getResult().getText());
            }
            else if (e.getResult().getReason() == ResultReason.RecognizedSpeech)
            {
                // Final result. May differ from the last intermediate result.
                System.out.println("RECOGNIZED: Text=" + e.getResult().getText());

                // See where the result came from, cloud (online) or embedded (offline)
                // speech recognition.
                // This can change during a session where HybridSpeechConfig is used.
                /*
                System.out.println("Recognition backend: " + e.getResult().getProperties().getProperty(PropertyId.SpeechServiceResponse_RecognitionBackend));
                */

                // Recognition results in JSON format.
                //
                // Offset and duration values are in ticks, where a single tick
                // represents 100 nanoseconds or one ten-millionth of a second.
                //
                // To get word level detail, set the output format to detailed.
                // See embeddedRecognitionFromWavFile() in this source file for
                // a configuration example.
                //
                // If an embedded speech recognition model does not support word
                // timing, the word offset and duration values are always 0, and the
                // phrase offset and duration only indicate a time window inside of
                // which the phrase appeared, not the accurate start and end of speech.
                /*
                String jsonResult = e.getResult().getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);
                System.out.println("JSON result: " + jsonResult);
                */
                // Word level detail (if enabled in speech config).
                /*
                // Convert the JSON string to a JSON object.
                JSONObject json = new JSONObject(jsonResult);

                // Extract word level detail from the JSON.
                JSONArray nbestArray = json.getJSONArray("NBest");
                if (nbestArray != null && nbestArray.length() > 0)
                {
                    JSONObject bestResult = nbestArray.getJSONObject(0);
                    JSONArray wordsArray = bestResult.getJSONArray("Words");

                    for (int i = 0; i < wordsArray.length(); i++)
                    {
                        JSONObject word = wordsArray.getJSONObject(i);
                        System.out.println(
                            "Word: \"" + word.getString("Word") + "\" | " +
                            "Offset: " + word.getLong("Offset") / 10000 + "ms | " +
                            "Duration: " + word.getLong("Duration") / 10000 + "ms");
                    }
                }
                */
            }
            else if (e.getResult().getReason() == ResultReason.NoMatch)
            {
                // NoMatch occurs when no speech was recognized.
                NoMatchReason reason = NoMatchDetails.fromResult(e.getResult()).getReason();
                System.out.println("NO MATCH: Reason=" + reason);
            }
        });

        recognizer.canceled.addEventListener((s, e) ->
        {
            System.out.println("CANCELED: Reason=" + e.getReason());

            if (e.getReason() == CancellationReason.Error)
            {
                // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
                System.err.println("CANCELED: ErrorCode=" + e.getErrorCode());
                System.err.println("CANCELED: ErrorDetails=\"" + e.getErrorDetails() + "\"");
            }
        });

        recognizer.sessionStarted.addEventListener((s, e) ->
        {
            System.out.println("Session started.");
        });

        recognizer.sessionStopped.addEventListener((s, e) ->
        {
            System.out.println("Session stopped.");
            recognitionEnd.release();
        });

        if (useKeyword)
        {
            // Creates an instance of a keyword recognition model.
            KeywordRecognitionModel keywordModel = KeywordRecognitionModel.fromFile(Settings.getKeywordModelFileName());

            // Starts the following routine:
            // 1. Listen for a keyword in input audio. There is no timeout.
            //    Speech that does not start with the keyword is ignored.
            // 2. If the keyword is spotted, start normal speech recognition.
            // 3. After a recognition result (that always includes at least
            //    the keyword), go back to step 1.
            // Steps 1-3 repeat until stopKeywordRecognitionAsync() is called.
            recognizer.startKeywordRecognitionAsync(keywordModel).get();

            System.out.println("Say something starting with \"" + Settings.getKeywordPhrase() + "\" (press Enter to stop)...");
            @SuppressWarnings({ "unused", "resource" })
            String input = new Scanner(System.in).nextLine();

            // Stops recognition.
            recognizer.stopKeywordRecognitionAsync().get();
        }
        else
        {
            // The following lines run continuous recognition that listens for speech
            // in input audio and generates results until stopped. To run recognition
            // only once, replace this code block with
            //
            // SpeechRecognitionResult result = recognizer.recognizeOnceAsync().get();
            //
            // and optionally check result.getReason() and result.getText() for the outcome
            // instead of doing it in event handlers.

            // Starts continuous recognition.
            recognizer.startContinuousRecognitionAsync().get();

            if (waitForUser)
            {
                System.out.println("Say something (press Enter to stop)...");
                @SuppressWarnings({ "unused", "resource" })
                String input = new Scanner(System.in).nextLine();
            }
            else
            {
                recognitionEnd.acquire();
            }

            // Stops recognition.
            recognizer.stopContinuousRecognitionAsync().get();
        }
    }


    // Reads audio samples from the source and writes them into a push stream.
    // Push stream can be used when input audio is not generated faster than it
    // can be processed (i.e. the generation of input is the limiting factor).
    // The application determines the rate of input data transfer.
    private static class PushStreamInputReader implements Runnable
    {
        private PushAudioInputStream pushStream;

        public PushStreamInputReader(PushAudioInputStream pushStream)
        {
            this.pushStream = pushStream;
        }

        public void run()
        {
            InputStream input = null;

            try
            {
                // In this example the input stream is a file. Modify the code to use
                // a non-file source (e.g. some audio API that returns data) as needed.
                input = new FileInputStream(Settings.getSpeechRawAudioFileName());

                byte[] buffer = new byte[3200]; // 100ms of 16kHz 16-bit mono audio
                int bytesRead;

                // Read audio data from the input stream to a data buffer.
                while ((bytesRead = input.read(buffer)) != -1)
                {
                    if (bytesRead == buffer.length)
                    {
                        // Copy audio data from the data buffer into a push stream
                        // for the Speech SDK to consume.
                        // Data must NOT include any headers, only audio samples.
                        pushStream.write(buffer);
                    }
                    else
                    {
                        // The final buffer read from the source may not be full.
                        pushStream.write(Arrays.copyOfRange(buffer, 0, bytesRead));
                    }
                }

                input.close();
            }
            catch (IOException e)
            {
                System.err.println("PushStreamInputReader: " + e);
            }

            pushStream.close();
        }
    }


    // Implements a pull stream callback that reads audio samples from the source.
    // Pull stream should be used when input audio may be generated faster than
    // it can be processed (i.e. the processing of input is the limiting factor).
    // The Speech SDK determines the rate of input data transfer.
    private static class PullStreamInputReader extends PullAudioInputStreamCallback
    {
        private final InputStream input;

        PullStreamInputReader() throws FileNotFoundException
        {
            // In this example the input stream is a file. Modify the code to use
            // a non-file source (e.g. audio API that returns data) as necessary.
            this.input = new FileInputStream(Settings.getSpeechRawAudioFileName());
        }

        // This method is called to synchronously get data from the input stream.
        // It returns the number of bytes copied into the data buffer.
        // If there is no data, the method must wait until data becomes available
        // or return 0 to indicate the end of stream.
        @Override
        public int read(byte[] buffer)
        {
            long bytesRead = 0;

            try {
                // Copy audio data from the input stream into a data buffer for the
                // Speech SDK to consume.
                // Data must NOT include any headers, only audio samples.
                bytesRead = this.input.read(buffer, 0, buffer.length);
            }
            catch (Exception e)
            {
                System.out.println("PullStreamInputReader: " + e.toString());
            }
            return (int)Math.max(0, bytesRead);
        }

        // This method is called for cleanup of resources.
        @Override
        public void close()
        {
            try {
                this.input.close();
            } catch (IOException e) {
                // ignored
            }
        }
    }


    // Recognizes speech using embedded speech config and the system default microphone device.
    public static void embeddedRecognitionFromMicrophone() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = false;
        boolean waitForUser = true;

        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput();

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizeSpeechAsync(recognizer, useKeyword, waitForUser);

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Recognizes speech using embedded speech config and the system default microphone device.
    // Recognition is triggered with a keyword.
    public static void embeddedRecognitionWithKeywordFromMicrophone() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = true;
        boolean waitForUser = true;

        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput();

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizeSpeechAsync(recognizer, useKeyword, waitForUser);

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Recognizes speech using embedded speech config and a WAV file.
    public static void embeddedRecognitionFromWavFile() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = false;
        boolean waitForUser = false;

        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromWavFileInput(Settings.getSpeechWavAudioFileName());

        // Enables detailed results (from PropertyId.SpeechServiceResponse_JsonResult).
        // See the event handler of 'recognized' event for how to read the JSON result.
        /*
        speechConfig.setSpeechRecognitionOutputFormat(OutputFormat.Detailed);
        */

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizeSpeechAsync(recognizer, useKeyword, waitForUser);

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Recognizes speech using embedded speech config and a push stream.
    public static void embeddedRecognitionFromPushStream() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = false;
        boolean waitForUser = false;

        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioStreamFormat audioFormat = AudioStreamFormat.getWaveFormatPCM(Settings.getEmbeddedSpeechSamplesPerSecond(), Settings.getEmbeddedSpeechBitsPerSample(), Settings.getEmbeddedSpeechChannels());
        PushAudioInputStream pushStream = AudioInputStream.createPushStream(audioFormat);
        AudioConfig audioConfig = AudioConfig.fromStreamInput(pushStream);

        // Push data into the stream in another thread.
        Thread pushStreamThread = new Thread(new PushStreamInputReader(pushStream));
        pushStreamThread.start();

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizeSpeechAsync(recognizer, useKeyword, waitForUser);

        pushStreamThread.join();

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Recognizes speech using embedded speech config and a pull stream.
    public static void embeddedRecognitionFromPullStream() throws InterruptedException, ExecutionException, FileNotFoundException
    {
        boolean useKeyword = false;
        boolean waitForUser = false;

        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioStreamFormat audioFormat = AudioStreamFormat.getWaveFormatPCM(Settings.getEmbeddedSpeechSamplesPerSecond(), Settings.getEmbeddedSpeechBitsPerSample(), Settings.getEmbeddedSpeechChannels());
        PullAudioInputStream pullStream = PullAudioInputStream.createPullStream(new PullStreamInputReader(), audioFormat);
        AudioConfig audioConfig = AudioConfig.fromStreamInput(pullStream);

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizeSpeechAsync(recognizer, useKeyword, waitForUser);

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Recognizes speech using hybrid (cloud & embedded) speech config and the system default microphone device.
    public static void hybridRecognitionFromMicrophone() throws InterruptedException, ExecutionException
    {
        boolean useKeyword = false;
        boolean waitForUser = true;

        HybridSpeechConfig speechConfig = Settings.createHybridSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput();

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizeSpeechAsync(recognizer, useKeyword, waitForUser);

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Measures the device performance when running embedded speech recognition.
    public static void embeddedRecognitionPerformanceTest() throws InterruptedException, ExecutionException
    {
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromWavFileInput(Settings.getPerfTestAudioFileName());

        // Enables performance metrics to be included with recognition results.
        speechConfig.setProperty(PropertyId.EmbeddedSpeech_EnablePerformanceMetrics, "true");

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);

        Semaphore recognitionEnd = new Semaphore(0);
        AtomicInteger resultCount = new AtomicInteger(0);

        // Subscribes to events.
        recognizer.speechStartDetected.addEventListener((s, e) ->
        {
            System.out.println("Processing, please wait...");
        });

        recognizer.recognized.addEventListener((s, e) ->
        {
            if (e.getResult().getReason() == ResultReason.RecognizedSpeech)
            {
                System.out.println("[" + resultCount.incrementAndGet() + "] RECOGNIZED: Text=" + e.getResult().getText());

                // Recognition results in JSON format.
                String jsonResult = e.getResult().getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);
                JSONObject json = new JSONObject(jsonResult);

                if (json.has("PerformanceCounters"))
                {
                    JSONArray perfCounters = json.getJSONArray("PerformanceCounters");
                    System.out.println("[" + resultCount + "] PerformanceCounters: " + perfCounters.toString(4));
                }
                else
                {
                    System.err.println("ERROR: No performance counters data found.");
                }
            }
        });

        recognizer.canceled.addEventListener((s, e) ->
        {
            if (e.getReason() == CancellationReason.Error)
            {
                System.err.println("CANCELED: ErrorCode=" + e.getErrorCode() + " ErrorDetails=" + e.getErrorDetails());
            }
        });

        recognizer.sessionStopped.addEventListener((s, e) ->
        {
            System.out.println("All done! Please go to https://aka.ms/embedded-speech for information on how to evaluate the results.");
            recognitionEnd.release();
        });

        // Runs continuous recognition.
        recognizer.startContinuousRecognitionAsync().get();
        recognitionEnd.acquire();
        recognizer.stopContinuousRecognitionAsync().get();

        recognizer.close();
        audioConfig.close();
        speechConfig.close();
    }
}
