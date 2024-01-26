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
import com.google.gson.Gson;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.transcription.*;

/**
 * Quickstart: transcribe meetings using the Speech SDK for Java.
 */

// Class which defines VoiceSignature as specified under https://aka.ms/cts/signaturegenservice.
class VoiceSignature
{
    public String Status;
    public VoiceSignatureData Signature;
    public String Transcription;
}

// Class which defines VoiceSignatureData which is used when creating/adding participants
class VoiceSignatureData
{
    public String Version;
    public String Tag;
    public String Data;
}

public class Main {

    public static VoiceSignature generateVoiceSignature(String voiceSamplePath, String subscriptionKey, String region) throws IOException, InterruptedException {
        String httpsURL = "https://signature." + region + ".cts.speech.microsoft.com/api/v1/Signature/GenerateVoiceSignatureFromByteArray";
        byte[] voiceSampleData = Files.readAllBytes(Paths.get(voiceSamplePath));
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest request = HttpRequest.newBuilder().uri(URI.create(httpsURL))
        .header("Ocp-Apim-Subscription-Key", subscriptionKey)
        .timeout(Duration.ofMinutes(1))
        .POST(BodyPublishers.ofByteArray(voiceSampleData))
        .build();
        HttpResponse<String> response = client.send(request, BodyHandlers.ofString());
        Gson gson = new Gson();
        VoiceSignature voiceSignature = gson.fromJson(response.body(), VoiceSignature.class);
        return voiceSignature;
    }
    
    /**
     * @param args Arguments are ignored in this sample.
     */
    public static void main(String[] args) throws IOException, InterruptedException {

        // Replace below with your own subscription key
        String subscriptionKey = "YourSubscriptionKey";
        // Replace below with your own service region (e.g., "centralus").
        String serviceRegion = "YourServiceRegion";

        // The input audio wave format for voice signatures is 16-bit samples, 16 kHz sample rate, and a single channel (mono).
        // The recommended length for each sample is between thirty seconds and two minutes.
        String voiceSignatureWaveForUser1 = "enrollment_audio_katie.wav";
        String voiceSignatureWaveForUser2 = "enrollment_audio_steve.wav";

        // Create voice signatures for the user1 and user2 and serialize it to json string
        VoiceSignature voiceSignatureUser1 = generateVoiceSignature(voiceSignatureWaveForUser1, subscriptionKey, serviceRegion);
        VoiceSignature voiceSignatureUser2 = generateVoiceSignature(voiceSignatureWaveForUser2, subscriptionKey, serviceRegion);        
        Gson gson = new Gson();
        String voiceSignatureStringUser1 = gson.toJson(voiceSignatureUser1.Signature);
        String voiceSignatureStringUser2 = gson.toJson(voiceSignatureUser2.Signature);
        
        // This sample expects a wavfile which is captured using a supported Speech SDK devices (8 channel, 16kHz, 16-bit PCM)
        // See https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-devices-sdk-microphone
        InputStream inputStream = new FileInputStream("katiesteve.wav");

        // Set audio format
        long samplesPerSecond = 16000;
        short bitsPerSample = 16;
        short channels = 8;

        // Create the push stream
        PushAudioInputStream pushStream = AudioInputStream.createPushStream(AudioStreamFormat.getWaveFormatPCM(samplesPerSecond, bitsPerSample, channels));
  
        // Creates speech configuration with subscription information
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(subscriptionKey, serviceRegion);
        speechConfig.setProperty("ConversationTranscriptionInRoomAndOnline", "true");
        
        // Meeting identifier is required when creating meeting.
        UUID meetingId = UUID.randomUUID();
        
        // Creates meeting and transcriber objects using push stream as audio input.
        try (Meeting meeting = Meeting.createMeetingAsync(speechConfig, meetingId.toString()).get(); 
             AudioConfig audioInput = AudioConfig.fromStreamInput(pushStream);
             MeetingTranscriber transcriber = new MeetingTranscriber(audioInput)) {

            System.out.println("Starting meeting...");

            // Subscribes to events
            transcriber.transcribed.addEventListener((s, e) -> {
                // In successful transcription, UserId will show the id of the meeting participant.
                if (e.getResult().getReason() == ResultReason.RecognizedSpeech) {
                    System.out.println("TRANSCRIBED: Text=" + e.getResult().getText() + " UserId=" + e.getResult().getUserId() );
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

            // Add participants to the meeting with their voice signatures.
            Participant participant1 = Participant.from("katie@example.com", "en-us", voiceSignatureStringUser1);
            meeting.addParticipantAsync(participant1);          
            Participant participant2 = Participant.from("stevie@example.com", "en-us", voiceSignatureStringUser2);
            meeting.addParticipantAsync(participant2);

            // Transcriber must be joined to the meeting before starting transcription.
            transcriber.joinMeetingAsync(meeting).get();
            
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
}
// </code>
