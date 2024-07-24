package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;

@SuppressWarnings("resource") // scanner
public class SpeechSynthesisSamples {

    // Speech synthesis to the default speaker.
    public static void synthesisToSpeakerAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer using the default speaker as audio output.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to speaker.
                System.out.println("Enter some text that you want to speak, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized to speaker for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis in the specified spoken language.
    public static void synthesisWithLanguageAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Sets the synthesis language.
        // The full list of supported language can be found here:
        // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
        String language = "de-DE";
        config.setSpeechSynthesisLanguage(language);

        // Creates a speech synthesizer using the default speaker as audio output.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to speaker.
                System.out.println("Enter some text that you want to speak, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized to speaker for text [" + text + "] with language [" + language + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis in the specified voice.
    public static void synthesisWithVoiceAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Sets the voice name.
        // e.g. "en-US-AndrewMultilingualNeural".
        // The full list of supported voices can be found here:
        // https://aka.ms/csspeech/voicenames
        // And, you can try getVoicesAsync method to get all available voices (see synthesisGetAvailableVoicesAsync() sample below).
        String voice = "en-US-AndrewMultilingualNeural";
        config.setSpeechSynthesisVoiceName(voice);

        // Creates a speech synthesizer using the default speaker as audio output.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to speaker.
                System.out.println("Enter some text that you want to speak, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized to speaker for text [" + text + "] with voice [" + voice + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis using Custom Voice (https://aka.ms/customvoice).
    public static void synthesisUsingCustomVoiceAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");
        // Replace with the endpoint id of your Custom Voice model.
        config.setEndpointId("YourEndpointId");
        // Replace with the voice name of your Custom Voice model.
        config.setSpeechSynthesisVoiceName("YourVoiceName");

        // Creates a speech synthesizer using the default speaker as audio output.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to speaker.
                System.out.println("Enter some text that you want to speak, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized to speaker for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis to wave file.
    public static void synthesisToWaveFileAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer using file as audio output.
        // Replace with your own audio file name.
        String fileName = "outputaudio.wav";
        AudioConfig fileOutput = AudioConfig.fromWavFileOutput(fileName);

        // Creates a speech synthesizer using a wave file as audio output.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, fileOutput);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to wave file.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "], and the audio was saved to [" + fileName + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
        fileOutput.close();
    }

    // Speech synthesis to MP3 file.
    public static void synthesisToMp3FileAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Sets the synthesis output format.
        // The full list of supported format can be found here:
        // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-text-to-speech#audio-outputs
        config.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio16Khz32KBitRateMonoMp3);

        // Creates a speech synthesizer using file as audio output.
        // Replace with your own audio file name.
        String fileName = "outputaudio.mp3";
        AudioConfig fileOutput = AudioConfig.fromWavFileOutput(fileName);

        // Creates a speech synthesizer using an mp3 file as audio output.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, fileOutput);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to mp3 file.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "], and the audio was saved to [" + fileName + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
        fileOutput.close();
    }

    // Speech synthesis to pull audio output stream.
    public static void synthesisToPullAudioOutputStreamAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates an audio out stream.
        PullAudioOutputStream stream = AudioOutputStream.createPullStream();

        // Creates a speech synthesizer using audio stream output.
        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, streamConfig);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to pull audio output stream.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "], and the audio was written to output stream.");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
        streamConfig.close();

        // Reads(pulls) data from the stream
        byte[] buffer = new byte[32000];
        long totalSize = 0;
        long filledSize = stream.read(buffer);
        while (filledSize > 0) {
            System.out.println(filledSize + " bytes received.");
            totalSize += filledSize;
            filledSize = stream.read(buffer);
        }

        System.out.println("Totally " + totalSize + " bytes received.");

        stream.close();
    }

    // Speech synthesis to push audio output stream.
    public static void synthesisToPushAudioOutputStreamAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates an instance of a customer class inherited from PushAudioOutputStreamCallback
        PushAudioOutputStreamSampleCallback callback = new PushAudioOutputStreamSampleCallback();

        // Creates an audio out stream from the callback.
        PushAudioOutputStream stream = AudioOutputStream.createPushStream(callback);

        // Creates a speech synthesizer using audio stream output.
        AudioConfig streamConfig = AudioConfig.fromStreamOutput(stream);
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, streamConfig);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to push audio output stream.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "], and the audio was written to output stream.");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
        streamConfig.close();

        System.out.println("Totally " + callback.getAudioData().length + " bytes received.");

        stream.close();
    }

    // Gets synthesized audio data from result.
    public static void synthesisToResultAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer with a null output stream.
        // This means the audio output data will not be written to any stream.
        // You can just get the audio from the result.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to result.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "].");
                    byte[] audioData = result.getAudioData();
                    System.out.println(audioData.length + " bytes of audio data received for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis to audio data stream.
    public static void synthesisToAudioDataStreamAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer with a null output stream.
        // This means the audio output data will not be written to any stream.
        // You can just get the audio from the result.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to result.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "].");
                    AudioDataStream audioDataStream = AudioDataStream.fromResult(result);

                    // You can save all the data in the audio data stream to a file
                    String fileName = "outputaudio.wav";
                    audioDataStream.saveToWavFileAsync(fileName).get();
                    System.out.println("Audio data for text [" + text + "] was saved to [" + fileName + "].");

                    // You can also read data from audio data stream and process it in memory
                    // Reset the stream position to the beginning since saving to file puts the position to end
                    audioDataStream.setPosition(0);
                    byte[] buffer = new byte[16000];
                    long totalSize = 0;
                    long filledSize = audioDataStream.readData(buffer);
                    while (filledSize > 0) {
                        System.out.println(filledSize + " bytes received.");
                        totalSize += filledSize;
                        filledSize = audioDataStream.readData(buffer);
                    }

                    System.out.println("Totally " + totalSize + " bytes received for text [" + text + "]");
                    audioDataStream.close();
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis events.
    public static void synthesisEventsAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer with a null output stream.
        // This means the audio output data will not be written to any stream.
        // You can just get the audio from the result.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);
        {
            // Subscribes to events
            synthesizer.SynthesisStarted.addEventListener((o, e) -> {
                System.out.println("Synthesis started.");
            });

            synthesizer.Synthesizing.addEventListener((o, e) -> {
                System.out.println("Synthesizing event received with audio chunk of " + e.getResult().getAudioLength() + " bytes.");
            });

            synthesizer.SynthesisCompleted.addEventListener((o, e) -> {
                System.out.println("Synthesis completed.");
            });

            while (true)
            {
                // Receives a text from console input and synthesize it to result.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "].");
                    byte[] audioData = result.getAudioData();
                    System.out.println(audioData.length + " bytes of audio data received for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis word boundary event.
    public static void synthesisWordBoundaryEventAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer with a null output stream.
        // This means the audio output data will not be written to any stream.
        // You can just get the audio from the result.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);
        {
            // Subscribes to word boundary event
            synthesizer.WordBoundary.addEventListener((o, e) -> {
                // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
                System.out.print("Word boundary event received. Audio offset: " + (e.getAudioOffset() + 5000) / 10000 + "ms, ");
                System.out.println("text offset: " + e.getTextOffset() + ", word length: " + e.getWordLength() + ".");
            });

            while (true)
            {
                // Receives a text from console input and synthesize it to result.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "].");
                    byte[] audioData = result.getAudioData();
                    System.out.println(audioData.length + " bytes of audio data received for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Synthesize and save audio and subtitle (in SubRip file format, SRT).
    public static void synthesisWordBoundaryEventToSrtAsync() throws InterruptedException, ExecutionException, IOException {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");
        config.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio24Khz96KBitRateMonoMp3);
        // Save to audio file
        AudioConfig audioConfig = AudioConfig.fromWavFileOutput("output.mp3");
        List<SpeechSynthesisWordBoundaryEventArgs> wordBoundaries = new ArrayList<>();

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, audioConfig);
        {
            // Subscribes to word boundary event
            synthesizer.WordBoundary.addEventListener((o, e) -> {
                // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
                System.out.print("Word boundary event received. Audio offset: " + (e.getAudioOffset() + 5000) / 10000 + "ms, ");
                System.out.println("text offset: " + e.getTextOffset() + ", word length: " + e.getWordLength() + ".");
                wordBoundaries.add(e);
            });

            String ssml;
            try {
                BufferedReader reader = new BufferedReader(new FileReader("ssml.txt"));
                ssml = reader.lines().collect(Collectors.joining(System.lineSeparator()));
                reader.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
                return;
            }

            SpeechSynthesisResult result = synthesizer.SpeakSsmlAsync(ssml).get();

            // Checks result.
            if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                System.out.println("Speech synthesized.");
                byte[] audioData = result.getAudioData();
                System.out.println(audioData.length + " bytes of audio data received.");

                // Save to SRT
                FileWriter writer = new FileWriter("output.srt");

                for (int idx = 0; idx < wordBoundaries.size(); idx++) {
                    writer.write(Integer.toString(idx + 1) + '\n');
                    SpeechSynthesisWordBoundaryEventArgs e = wordBoundaries.get(idx);
                    long millis = e.getAudioOffset() / 10000;
                    String startTime = String.format("%02d:%02d:%02d,%03d", TimeUnit.MILLISECONDS.toHours(millis),
                            TimeUnit.MILLISECONDS.toMinutes(millis) % TimeUnit.HOURS.toMinutes(1),
                            TimeUnit.MILLISECONDS.toSeconds(millis) % TimeUnit.MINUTES.toSeconds(1),
                            millis % 1000);
                    long endMillis;
                    if (idx < wordBoundaries.size() - 1) {
                        endMillis = wordBoundaries.get(idx + 1).getAudioOffset() / 10000;
                    } else {
                        endMillis = millis + 1000;
                    }
                    String endTime = String.format("%02d:%02d:%02d,%03d", TimeUnit.MILLISECONDS.toHours(endMillis),
                            TimeUnit.MILLISECONDS.toMinutes(endMillis) % TimeUnit.HOURS.toMinutes(1),
                            TimeUnit.MILLISECONDS.toSeconds(endMillis) % TimeUnit.MINUTES.toSeconds(1),
                            endMillis % 1000);
                    writer.write(startTime + " --> " + endTime + '\n');
                    writer.write(ssml.substring((int)e.getTextOffset(), (int)(e.getTextOffset() + e.getWordLength())) + "\n\n");
                }
                writer.close();
            }
            else if (result.getReason() == ResultReason.Canceled) {
                SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            }
            result.close();
        }
        synthesizer.close();
    }

    // Speech synthesis viseme event.
    public static void synthesisVisemeEventAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer with a null output stream.
        // This means the audio output data will not be written to any stream.
        // You can just get the audio from the result.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);
        {
            // Subscribes to viseme received event
            synthesizer.VisemeReceived.addEventListener((o, e) -> {
                // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
                System.out.print("Viseme event received. Audio offset: " + e.getAudioOffset() / 10000 + "ms, ");
                System.out.println("viseme id: " + e.getVisemeId() + ".");
            });

            while (true)
            {
                // Receives a text from console input and synthesize it to result.
                System.out.println("Enter some text that you want to synthesize, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized for text [" + text + "].");
                    byte[] audioData = result.getAudioData();
                    System.out.println(audioData.length + " bytes of audio data received for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis bookmark event.
    public static void synthesisBookmarkEventAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".
        SpeechConfig config = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer with a null output stream.
        // This means the audio output data will not be written to any stream.
        // You can just get the audio from the result.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(config, null);

        // Subscribes to bookmark received event
        synthesizer.BookmarkReached.addEventListener((o, e) -> {
            // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
            System.out.print("Bookmark reached. Audio offset: " + e.getAudioOffset() / 10000 + "ms, ");
            System.out.println("bookmark text: " + e.getText() + ".");
        });

        System.out.println("Press Enter to start synthesizing.");
        new Scanner(System.in).nextLine();

        // Bookmark tag is needed in the SSML, e.g.
        String ssml = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'><voice name='en-US-AvaMultilingualNeural'><bookmark mark='bookmark_one'/> one. <bookmark mark='bookmark_two'/> two. three. four.</voice></speak>";

        SpeechSynthesisResult result = synthesizer.SpeakSsmlAsync(ssml).get();

        // Checks result.
        if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
            System.out.println("Speech synthesized.");
        }
        else if (result.getReason() == ResultReason.Canceled) {
            SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
            System.out.println("CANCELED: Reason=" + cancellation.getReason());

            if (cancellation.getReason() == CancellationReason.Error) {
                System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                System.out.println("CANCELED: Did you update the subscription info?");
            }
        }

        result.close();
        synthesizer.close();
    }

    // Speech synthesis with auto detection for source language.
    // Note: this is a preview feature, which might be updated in future versions.
    public static void synthesisWithSourceLanguageAutoDetectionAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        AutoDetectSourceLanguageConfig autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.fromOpenRange();

        // Creates a speech synthesizer with auto-detection for source language
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig,
                autoDetectSourceLanguageConfig,
                AudioConfig.fromDefaultSpeakerOutput());
        {
            while (true)
            {
                // Receives a text from console input and synthesize it to speaker.
                // For example, you can input "Bonjour le monde. Hello world.", then you will hear "Bonjour le monde."
                // spoken in a French voice and "Hello world." in an English voice.
                System.out.println("Enter some multi lingual text that you want to speak, or enter empty text to exit.");
                System.out.print("> ");
                String text = new Scanner(System.in).nextLine();
                if (text.isEmpty())
                {
                    break;
                }

                SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

                // Checks result.
                if (result.getReason() == ResultReason.SynthesizingAudioCompleted) {
                    System.out.println("Speech synthesized to speaker for text [" + text + "]");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                    System.out.println("CANCELED: Reason=" + cancellation.getReason());

                    if (cancellation.getReason() == CancellationReason.Error) {
                        System.out.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                        System.out.println("CANCELED: ErrorDetails=" + cancellation.getErrorDetails());
                        System.out.println("CANCELED: Did you update the subscription info?");
                    }
                }

                result.close();
            }
        }

        synthesizer.close();
    }

    // Speech synthesis get available voices
    public static void synthesisGetAvailableVoicesAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        SpeechConfig speechConfig = SpeechConfig.fromSubscription("YourSubscriptionKey", "YourServiceRegion");

        // Creates a speech synthesizer
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null);

        System.out.println("Enter a locale in BCP-47 format (e.g. en-US) that you want to get the voices of, or enter empty to get voices in all locales.");
        System.out.print("> ");
        String text = new Scanner(System.in).nextLine();

        SynthesisVoicesResult result = synthesizer.getVoicesAsync(text).get();

        // Checks result.
        if (result.getReason() == ResultReason.VoicesListRetrieved) {
            System.out.println("Voices successfully retrieved, they are:");
            for (VoiceInfo voice : result.getVoices()) {
                System.out.println(voice.getName());
            }
        }
        else if (result.getReason() == ResultReason.Canceled) {
            System.out.println("CANCELED: ErrorDetails=" + result.getErrorDetails());
            System.out.println("CANCELED: Did you update the subscription info?");
        }

        result.close();
        synthesizer.close();
    }
}
