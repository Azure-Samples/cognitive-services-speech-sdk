// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package speechsdk.quickstart;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Future;
import java.lang.AutoCloseable;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.speaker.*;

/**
 * Quickstart: verify and identify speakers using the Speech SDK for Java.
 */
public class Main {

    public static boolean enrollProfile(VoiceProfileClient client, VoiceProfile profile, String audioFile) {
        try(AudioConfig audioConfig = AudioConfig.fromWavFileInput(audioFile)){
            assert(audioConfig != null);
            ResultReason resultReason = ResultReason.EnrollingVoiceProfile;

            while (resultReason == ResultReason.EnrollingVoiceProfile) {
                try (VoiceProfileEnrollmentResult result = client.enrollProfileAsync(profile, audioConfig).get()) {
                    assert(result != null);
                    resultReason = result.getReason();
                } catch (Exception e){
                    break;
                }
            }
            return resultReason == ResultReason.EnrolledVoiceProfile;
        }
    }

    public static void identifySpeakers(SpeechConfig speechConfig, List<VoiceProfile> profiles, String audioFile) throws Exception {
        try (SpeakerIdentificationModel model = SpeakerIdentificationModel.fromProfiles(profiles);
            AudioConfig audioConfig = AudioConfig.fromWavFileInput(audioFile)) {
            assert(audioConfig != null);
            assert(model != null);
            // Creates an instance of a speaker recognizer using speech configuration with specified
            // subscription key and service region, using the audio config created from the given file
            try (SpeakerRecognizer recognizer = new SpeakerRecognizer(speechConfig, audioConfig)) {
                try (SpeakerRecognitionResult recognitionResult = recognizer.recognizeOnceAsync(model).get()) {
                    if(recognitionResult.getReason() == ResultReason.RecognizedSpeakers) {
                        System.out.println(String.format("The most similar voice profile is %s with similarity score %f", recognitionResult.getProfileId(), recognitionResult.getScore()));
                        String raw = recognitionResult.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        System.out.println("The raw json from the service is " + raw);

                    }
                }
            }

        } catch (Exception ex) {
            throw ex;
        }

    }
    public static void verifySpeaker(SpeechConfig speechConfig, VoiceProfile profile, String audioFile) throws Exception {
        try (SpeakerVerificationModel model = SpeakerVerificationModel.fromProfile(profile);
            AudioConfig audioConfig = AudioConfig.fromWavFileInput(audioFile)) {

            assert(audioConfig != null);
            assert(model != null);

            // Creates an instance of a speaker recognizer using speech configuration with specified
            // subscription key and service region, using the audio config created from the given file
            try (SpeakerRecognizer recognizer = new SpeakerRecognizer(speechConfig, audioConfig)) {
                try (SpeakerRecognitionResult recognitionResult = recognizer.recognizeOnceAsync(model).get()) {
                    assert(recognitionResult.getReason() == ResultReason.RecognizedSpeaker);
                    assert(recognitionResult.getScore() > 0.5);
                    assert(recognitionResult.getProfileId().equals(profile.getId()));

                    System.out.println("Profile verified: " + recognitionResult.getProfileId());
                    System.out.println("Confidence score for profile: " + recognitionResult.getScore());
                }
            }
        } catch (Exception ex) {
            throw ex;
        }
    }

    // This method creates two voice profiles, enrolls them, then performs text independent identification against those profileS
    // before finally deleting them
    public static void speakerIdentification() {
        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";
        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";
        String audioFile1 = "TalkForAFewSeconds16.wav";
        String audioFile2 = "TalkForAFewSeconds16.wav";

        // Creates an instance of a voice profile client using speech configuration with specified
        // subscription key and service region
        try (SpeechConfig speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
             VoiceProfileClient client = new VoiceProfileClient(speechConfig)) {

            assert(speechConfig != null);
            assert(client != null);
            int exitCode = 1;
            try (VoiceProfile profile1 = client.createProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us").get();
                VoiceProfile profile2 = client.createProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us").get()) {
                assert(profile1 != null);
                assert(profile2 != null);
            
                if(enrollProfile(client, profile1, audioFile1) && enrollProfile(client, profile2, audioFile2)) {
                    System.out.println(String.format("Enrolled profiles %s and %s for text independent identification", profile1.getId(), profile2.getId()));
                    List<VoiceProfile> profiles = new ArrayList<>();
                    profiles.add(profile1);
                    profiles.add(profile2);
                    String identificationFile = "wikipediaOcelot.wav";
                    identifySpeakers(speechConfig, profiles, identificationFile);
                }
                try (VoiceProfileResult result1 = client.deleteProfileAsync(profile1).get();
                    VoiceProfileResult result2 = client.deleteProfileAsync(profile2).get()) {
                    assert(result1.getReason() == ResultReason.DeletedVoiceProfile);
                    assert(result2.getReason() == ResultReason.DeletedVoiceProfile);
                }
            }
        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }
    }

    // This method creates a voice profile, enrolls it, then performs text independent verification against that profile
    // before finally deleting the profile
    public static void speakerVerification() {
        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";
        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";
        String audioFile = "TalkForAFewSeconds16.wav";

        // Creates an instance of a voice profile client using speech configuration with specified
        // subscription key and service region
        try (SpeechConfig speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
             VoiceProfileClient client = new VoiceProfileClient(speechConfig)) {

            assert(speechConfig != null);
            assert(client != null);
            
            try (VoiceProfile profile = client.createProfileAsync(VoiceProfileType.TextIndependentVerification, "en-us").get()) {
                assert(VoiceProfileType.TextIndependentVerification == profile.getType());
                assert(profile != null);

                if(enrollProfile(client, profile, audioFile)) {
                    System.out.println("Enrolled successfully");
                    verifySpeaker(speechConfig, profile, audioFile);
                }
                try (VoiceProfileResult result = client.deleteProfileAsync(profile).get()) {
                    assert(result.getReason() == ResultReason.DeletedVoiceProfile);
                }
            }

        } catch (Exception ex) {
            System.out.println("Unexpected exception: " + ex.getMessage());

            assert(false);
            System.exit(1);
        }
    }

    /**
     * @param args Arguments are ignored in this sample.
     */
    public static void main(String[] args) {
        int exitCode = 1;

        System.out.println("Speaker Verification:");
        speakerVerification();
        System.out.println("Speaker Identification:");
        speakerIdentification();
        System.exit(exitCode);
    }
}
