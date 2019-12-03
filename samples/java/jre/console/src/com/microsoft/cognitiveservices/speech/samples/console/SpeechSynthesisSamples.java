package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.Scanner;
import java.util.concurrent.ExecutionException;

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
        // e.g. "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)"
        // The full list of supported voices can be found here:
        // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
        String voice = "Microsoft Server Speech Text to Speech Voice (en-US, BenjaminRUS)";
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
                        System.out.println(filledSize + " bytes revceived.");
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
}
