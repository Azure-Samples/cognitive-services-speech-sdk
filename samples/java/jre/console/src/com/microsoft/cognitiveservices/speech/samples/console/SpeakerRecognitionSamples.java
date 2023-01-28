package com.microsoft.cognitiveservices.speech.samples.console;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;

// <toplevel>
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;
import com.microsoft.cognitiveservices.speech.speaker.*;
// </toplevel>

@SuppressWarnings("resource") // scanner
public class SpeakerRecognitionSamples {

    // Speaker identification from file.
    public static void identificationWithFileAsync() throws InterruptedException, ExecutionException
    {
        // <identificationWithFile>
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".

        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";

        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";

        String enrollmentFile = "YourEnrollmentFile.wav";

        String identificationFile = "YourIdentificationFile.wav";

        // Creates an instance of a voice profile client using speech configuration with specified
        // subscription key and service region
        try (SpeechConfig speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
             VoiceProfileClient client = new VoiceProfileClient(speechConfig)) {

            try (VoiceProfile profile = client.createProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us").get()) {

                if(enrollProfileFromFile(client, profile, enrollmentFile)) {
                    System.out.println("Enrolled successfully");
                    System.out.println("Identification: Identifying using file: " + identificationFile);

                    List<VoiceProfile> profiles = new ArrayList<>();
                    profiles.add(profile);

                    identifySpeaker(speechConfig, profiles, AudioConfig.fromWavFileInput(identificationFile));
                }
                try (VoiceProfileResult result = client.deleteProfileAsync(profile).get()) {
                    assert(result.getReason() == ResultReason.DeletedVoiceProfile);
                }
            } catch (Exception e){
                System.out.println("Exception " + e);
            }
        }
        // </identificationWithFile>
    }

    // Speaker identification from microphone.
    public static void identificationWithMicrophoneAsync() throws InterruptedException, ExecutionException
    {
        // <identificationWithMicrophone>
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".

        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";

        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";

        // Creates an instance of a voice profile client using speech configuration with specified
        // subscription key and service region
        try (SpeechConfig speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
             VoiceProfileClient client = new VoiceProfileClient(speechConfig)) {

            try (VoiceProfile profile = client.createProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us").get()) {

                if(enrollProfileFromMicrophone(client, profile)) {
                    System.out.println("Enrolled successfully");
                    System.out.println("Identification: Identifying using microphone");

                    List<VoiceProfile> profiles = new ArrayList<>();
                    profiles.add(profile);

                    identifySpeaker(speechConfig, profiles, AudioConfig.fromDefaultMicrophoneInput());
                }
                try (VoiceProfileResult result = client.deleteProfileAsync(profile).get()) {
                    assert(result.getReason() == ResultReason.DeletedVoiceProfile);
                }
            } catch (Exception e){
                System.out.println("Exception " + e);
            }
        }
        // </identificationWithMicrophone>
    }


    // Speaker verification from file.
    public static void verificationWithFileAsync() throws InterruptedException, ExecutionException
    {
        // <verificationWithFile>
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".

        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";

        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";

        String enrollmentFile = "YourEnrollmentFile.wav";

        String verificationFile = "YourVerificationFile.wav";

        // Creates an instance of a voice profile client using speech configuration with specified
        // subscription key and service region
        try (SpeechConfig speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
             VoiceProfileClient client = new VoiceProfileClient(speechConfig)) {

            try (VoiceProfile profile = client.createProfileAsync(VoiceProfileType.TextIndependentVerification, "en-us").get()) {

                if(enrollProfileFromFile(client, profile, enrollmentFile)) {
                    System.out.println("Enrolled successfully");
                    System.out.println("Verification: Verifying using file: " + verificationFile);

                    verifySpeaker(speechConfig, profile, AudioConfig.fromWavFileInput(verificationFile));
                }
                try (VoiceProfileResult result = client.deleteProfileAsync(profile).get()) {
                    assert(result.getReason() == ResultReason.DeletedVoiceProfile);
                }
            } catch (Exception e){
                System.out.println("Exception " + e);
            }
        }
        // </verificationWithFile>
    }

    // Speaker verification from microphone.
    public static void verificationWithMicrophoneAsync() throws InterruptedException, ExecutionException
    {
        // <verificationWithMicrophone>
        // Creates an instance of a speech config with specified
        // subscription key and service region. Replace with your own subscription key
        // and service region (e.g., "westus").
        // The default language is "en-us".

        // Replace below with your own subscription key
        String speechSubscriptionKey = "YourSubscriptionKey";

        // Replace below with your own service region (e.g., "westus").
        String serviceRegion = "YourServiceRegion";

        // Creates an instance of a voice profile client using speech configuration with specified
        // subscription key and service region
        try (SpeechConfig speechConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);

             VoiceProfileClient client = new VoiceProfileClient(speechConfig)) {

            try (VoiceProfile profile = client.createProfileAsync(VoiceProfileType.TextIndependentVerification, "en-us").get()) {

                if(enrollProfileFromMicrophone(client, profile)) {

                    System.out.println("Enrolled successfully");
                    System.out.println("Verification: Please say 'I'll talk for a few seconds, so you can recognize my voice in the future.'");

                    verifySpeaker(speechConfig, profile, AudioConfig.fromDefaultMicrophoneInput());
                }
                try (VoiceProfileResult result = client.deleteProfileAsync(profile).get()) {
                    assert(result.getReason() == ResultReason.DeletedVoiceProfile);
                }
             } catch (Exception e){
                System.out.println("Exception " + e);
             }
        }
        // </verificationWithMicrophone>
    }

    private static boolean enrollProfileFromFile(VoiceProfileClient client, VoiceProfile profile, String file) {

        boolean success = false;

        try (AudioConfig audioConfig = AudioConfig.fromWavFileInput(file)) {
            System.out.println("Enrolling using file: " + file);
            success = enrollProfile(client, profile, audioConfig);
        }
        return success;
    }

    private static boolean enrollProfileFromMicrophone(VoiceProfileClient client, VoiceProfile profile) {

        boolean success = false;

        try (AudioConfig audioConfig = AudioConfig.fromDefaultMicrophoneInput()) {
            System.out.println("Please say 'I'll talk for a few seconds, so you can recognize my voice in the future.'");
            success = enrollProfile(client, profile, audioConfig);
        }
        return success;
    }

    private static boolean enrollProfile(VoiceProfileClient client, VoiceProfile profile, AudioConfig audioConfig) {

        ResultReason resultReason = ResultReason.EnrollingVoiceProfile;

        while (resultReason == ResultReason.EnrollingVoiceProfile) {
            try (VoiceProfileEnrollmentResult result = client.enrollProfileAsync(profile, audioConfig).get()) {

                resultReason = result.getReason();

                if (result.getReason() == ResultReason.EnrollingVoiceProfile) {
                    System.out.println("Enrolling, please repeat 'I'll talk for a few seconds, so you can recognize my voice in the future.'");
                }
                else if (result.getReason() == ResultReason.NoMatch) {
                    System.out.println("NOMATCH: Speech could not be recognized.");
                }
                else if (result.getReason() == ResultReason.Canceled) {
                    System.out.println("CANCELED");
                }
            } catch (Exception e){
                break;
            }
        }
        return resultReason == ResultReason.EnrolledVoiceProfile;
    }

    private static void verifySpeaker(SpeechConfig speechConfig, VoiceProfile profile, AudioConfig audioConfig) throws Exception {
        try (SpeakerVerificationModel model = SpeakerVerificationModel.fromProfile(profile)) {

            // Creates an instance of a speaker recognizer using speech configuration with specified
            // subscription key and service region, using the audio config created from the given file
            try (SpeakerRecognizer recognizer = new SpeakerRecognizer(speechConfig, audioConfig)) {

                try (SpeakerRecognitionResult recognitionResult = recognizer.recognizeOnceAsync(model).get()) {
                    System.out.println("Profile verified: " + recognitionResult.getProfileId());
                    System.out.println("Confidence score for profile: " + recognitionResult.getScore());
                }
            }
        } catch (Exception ex) {
            throw ex;
        }
    }

    private static void identifySpeaker(SpeechConfig speechConfig, List<VoiceProfile> profiles, AudioConfig audioConfig) throws Exception {

        try (SpeakerIdentificationModel model = SpeakerIdentificationModel.fromProfiles(profiles)) {

            // Creates an instance of a speaker recognizer using speech configuration with specified
            // subscription key and service region, using the audio config created from the given file
            try (SpeakerRecognizer recognizer = new SpeakerRecognizer(speechConfig, audioConfig)) {

                try (SpeakerRecognitionResult recognitionResult = recognizer.recognizeOnceAsync(model).get()) {
                    System.out.println("Profile identified: " + recognitionResult.getProfileId());
                    System.out.println("Confidence score for profile: " + recognitionResult.getScore());
                }
            }
        } catch (Exception ex) {
            throw ex;
        }
    }

}
