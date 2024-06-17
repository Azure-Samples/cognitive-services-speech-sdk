//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package speechsdk.quickstart;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.net.http.HttpRequest.BodyPublishers;
import java.net.http.HttpResponse.BodyHandlers;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.Duration;
import java.io.*;
import java.util.Arrays;
import java.util.Scanner;
import java.util.UUID;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.transcription.*;

/**
 * Quickstart: transcribe conversations using the Speech SDK for Java.
 */
public class Main {

    /**
     * @param args Arguments are ignored in this sample.
     */
    public static void main(String[] args) throws IOException, InterruptedException {

        // Replace below with your own subscription key
        String subscriptionKey = "YourSubscriptionKey";
        // Replace below with your own service region (e.g., "centralus").
        String serviceRegion = "YourServiceRegion";

        // Choose transcription from file or microphone by commenting and uncommenting the following function calls.
        ConversationTranscriptionFromFile(subscriptionKey, serviceRegion);
        //ConversationTranscriptionFromMicrophone(subscriptionKey, serviceRegion);
    }

    public static void ConversationTranscriptionFromFile(String subscriptionKey, String serviceRegion) {

        // Create an audio stream from a wav file using 16-bit PCM audio format. Replace with your own audio file.
        // Update the samplesPerSecond, bitsPerSample and channel variables below according to the format of your file.
        // Different PCM formats (1ch, stereo, 4ch, 8ch) with different sampling rates (8KHz, 16KHz, 48KHz) 
        // are supported. Default format is single channel 16KHz.
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream("katiesteve_mono.wav");
        } catch (FileNotFoundException fnf) {
            System.out.println("File not found");
            return;
        }

        // Set audio format
        long samplesPerSecond = 16000;
        short bitsPerSample = 16;
        short channels = 1;

        // Create the push stream
        PushAudioInputStream pushStream = AudioInputStream.createPushStream(AudioStreamFormat.getWaveFormatPCM(samplesPerSecond, bitsPerSample, channels));
  
        // Creates speech configuration with subscription information
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
        
        // Creates conversation and transcriber objects using push stream as audio input.
        try (AudioConfig audioInput = AudioConfig.fromStreamInput(pushStream);
             ConversationTranscriber transcriber = new ConversationTranscriber(speechConfig, audioInput)) {

            System.out.println("Starting conversation...");

            // Subscribes to events
            transcriber.transcribed.addEventListener((s, e) -> {
                // In successful transcription, SpeakerId will show the id of the speaker.
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("TRANSCRIBED: Text=" + e.getResult().getText() + " SpeakerId=" + e.getResult().getSpeakerId() );
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            transcriber.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            });

            transcriber.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\n    Session started event.");
            });

            transcriber.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\n    Session stopped event.");
            });
            
            // Starts continuous transcription. Use stopTranscribingAsync() to stop transcription.
            transcriber.startTranscribingAsync().get();

            // Arbitrary buffer size.
            byte[] readBuffer = new byte[4096];

            // Push audio reads from the file into the PushStream.
            // The audio can be pushed into the stream before, after, or during transcription
            // and transcription will continue as data becomes available.
            int bytesRead;
            while ((bytesRead = inputStream.read(readBuffer)) != -1) {
                if (bytesRead == readBuffer.length) {
                    pushStream.write(readBuffer);
                }
                else {
                    // Last buffer read from the WAV file is likely to have less bytes.
                    pushStream.write(Arrays.copyOfRange(readBuffer, 0, bytesRead));
                }
            }

            pushStream.close();
            inputStream.close();

            System.out.println("Press any key to stop transcription");
            new Scanner(System.in).nextLine();

            transcriber.stopTranscribingAsync().get();
            
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }        
        speechConfig.close();
    }

    public static void ConversationTranscriptionFromMicrophone(String subscriptionKey, String serviceRegion) {

        // Creates speech configuration with subscription information
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
        
        try (AudioConfig audioInput = AudioConfig.fromDefaultMicrophoneInput();
             ConversationTranscriber transcriber = new ConversationTranscriber(speechConfig, audioInput)) {

            System.out.println("Starting conversation...");

            // Subscribes to events
            transcriber.transcribed.addEventListener((s, e) -> {
                // In successful transcription, SpeakerId will show the id of the speaker.
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("TRANSCRIBED: Text=" + e.getResult().getText() + " SpeakerId=" + e.getResult().getSpeakerId() );
                }
                else if (e.getResult().getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
            });

            transcriber.canceled.addEventListener((s, e) -> {
                System.out.println("CANCELED: Reason=" + e.getReason());

                if (e.getReason() == CancellationReason.Error) {
                    System.out.println("CANCELED: ErrorCode=" + e.getErrorCode());
                    System.out.println("CANCELED: ErrorDetails=" + e.getErrorDetails());
                    System.out.println("CANCELED: Did you update the subscription info?");
                }
            });

            transcriber.sessionStarted.addEventListener((s, e) -> {
                System.out.println("\n    Session started event.");
            });

            transcriber.sessionStopped.addEventListener((s, e) -> {
                System.out.println("\n    Session stopped event.");
            });
            
            // Starts continuous transcription. Use stopTranscribingAsync() to stop transcription.
            transcriber.startTranscribingAsync().get();

            System.out.println("Press any key to stop transcription");
            new Scanner(System.in).nextLine();

            transcriber.stopTranscribingAsync().get();
            
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }        
        speechConfig.close();
    }    
}
// </code>
