//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
package com.microsoft.cognitiveservices.speech.samples.embedded;

import java.util.Scanner;
import java.util.concurrent.ExecutionException;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;


public class SpeechSynthesisSamples
{
    // List available embedded speech synthesis voices.
    public static void listEmbeddedVoicesAsync() throws InterruptedException, ExecutionException
    {
        // Creates an instance of an embedded speech config.
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        if (speechConfig == null)
        {
            return;
        }

        // Creates a speech synthesizer.
        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, null);
        {
            // Gets a list of voices.
            SynthesisVoicesResult result = synthesizer.getVoicesAsync("").get();

            if (result.getReason() == ResultReason.VoicesListRetrieved)
            {
                System.out.println("Voices found:");
                for (VoiceInfo voice : result.getVoices())
                {
                    System.out.println(voice.getName());
                    System.out.println(" Gender: " + voice.getGender());
                    System.out.println(" Locale: " + voice.getLocale());
                    System.out.println(" Path:   " + voice.getVoicePath());
                }

                // To find a voice that supports a specific locale, for example:
                /*
                String voiceName = null;
                String voiceLocale = "en-US";

                for (VoiceInfo voice : result.getVoices())
                {
                    if (voice.getLocale().equals(voiceLocale))
                    {
                        voiceName = voice.getName();
                        break;
                    }
                }

                if (voiceName != null && !voiceName.isEmpty())
                {
                    System.out.println("Found " + voiceLocale + " voice: " + voiceName);
                }
                */
            }
            else if (result.getReason() == ResultReason.Canceled)
            {
                System.err.println("CANCELED: ErrorDetails=\"" + result.getErrorDetails() + "\"");
            }

            result.close();
        }

        synthesizer.close();
        speechConfig.close();
    }


    private static void synthesizeSpeechAsync(SpeechSynthesizer synthesizer) throws InterruptedException, ExecutionException
    {
        // Subscribes to events.
        synthesizer.SynthesisStarted.addEventListener((s, e) ->
        {
            System.out.println("Synthesis started.");
        });

        synthesizer.Synthesizing.addEventListener((s, e) ->
        {
            System.out.println("Synthesizing, received an audio chunk of " + e.getResult().getAudioLength() + " bytes.");
        });

        synthesizer.WordBoundary.addEventListener((s, e) ->
        {
            System.out.print("Word \"" + e.getText() + "\" | ");
            System.out.print("Text offset " + e.getTextOffset() + " | ");
            // Unit of AudioOffset and Duration is tick (1 tick = 100 nanoseconds).
            System.out.println("Audio offset " + (e.getAudioOffset() + 5000) / 10000 + "ms");
        });

        while (true)
        {
            // Receives text from console input.
            System.out.println("Enter some text that you want to speak, or none to exit.");
            System.out.print("> ");

            @SuppressWarnings("resource")
            String text = new Scanner(System.in).nextLine();
            if (text == null || text.isEmpty())
            {
                break;
            }

            // Synthesizes text to speech.
            SpeechSynthesisResult result = synthesizer.SpeakTextAsync(text).get();

            // To adjust the rate or volume, use SpeakSsmlAsync with prosody tags instead of SpeakTextAsync.
            /*
            String ssmlSynthBegin = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'>";
            String ssmlSynthEnd = "</speak>";
            String ssml;

            // Range of rate is -100% to 100%, 100% means 2x faster.
            ssml = ssmlSynthBegin + "<prosody rate='50%'>" + text + "</prosody>" + ssmlSynthEnd;
            System.out.println("Synthesizing with rate +50%");
            SpeechSynthesisResult result1 = synthesizer.SpeakSsmlAsync(ssml).get();
            result1.close();

            // Range of volume is -100% to 100%, 100% means 2x louder.
            ssml = ssmlSynthBegin + "<prosody volume='50%'>" + text + "</prosody>" + ssmlSynthEnd;
            System.out.println("Synthesizing with volume +50%");
            SpeechSynthesisResult result2 = synthesizer.SpeakSsmlAsync(ssml).get();
            result2.close();
            */

            if (result.getReason() == ResultReason.SynthesizingAudioCompleted)
            {
                System.out.println("Speech synthesized for text \"" + text + "\"");

                // See where the result came from, cloud (online) or embedded (offline)
                // speech synthesis.
                // This can change during a session where HybridSpeechConfig is used.
                /*
                System.out.println("Synthesis backend: " + result.getProperties().getProperty(PropertyId.SpeechServiceResponse_SynthesisBackend));
                */

                // To save the output audio to a WAV file:
                /*
                AudioDataStream audioDataStream = AudioDataStream.fromResult(result);
                audioDataStream.saveToWavFileAsync("SynthesizedSpeech.wav").get();;
                */

                // To read the output audio for e.g. processing it in memory:
                /*
                audioDataStream.setPosition(0); // reset the stream position

                byte[] buffer = new byte[16000];
                long readBytes = 0;
                long totalBytes = 0;

                while ((readBytes = audioDataStream.readData(buffer)) > 0)
                {
                    System.out.println("Read " + readBytes + " bytes");
                    totalBytes += readBytes;
                }
                System.out.println("Total " + totalBytes + " bytes");

                audioDataStream.close();
                */
            }
            else if (result.getReason() == ResultReason.Canceled)
            {
                SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                System.out.println("CANCELED: Reason=" + cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error)
                {
                    System.err.println("CANCELED: ErrorCode=" + cancellation.getErrorCode());
                    System.err.println("CANCELED: ErrorDetails=\"" + cancellation.getErrorDetails() + "\"");
                }
            }

            result.close();
        }
    }


    // Synthesizes speech using embedded speech config and the system default speaker device.
    public static void embeddedSynthesisToSpeaker() throws InterruptedException, ExecutionException
    {
        EmbeddedSpeechConfig speechConfig = Settings.createEmbeddedSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromDefaultSpeakerOutput();

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);
        synthesizeSpeechAsync(synthesizer);

        synthesizer.close();
        audioConfig.close();
        speechConfig.close();
    }


    // Synthesizes speech using hybrid (cloud & embedded) speech config and the system default speaker device.
    public static void hybridSynthesisToSpeaker() throws InterruptedException, ExecutionException
    {
        HybridSpeechConfig speechConfig = Settings.createHybridSpeechConfig();
        AudioConfig audioConfig = AudioConfig.fromDefaultSpeakerOutput();

        SpeechSynthesizer synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);
        synthesizeSpeechAsync(synthesizer);

        synthesizer.close();
        audioConfig.close();
        speechConfig.close();
    }
}
