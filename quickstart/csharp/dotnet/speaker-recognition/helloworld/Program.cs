//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Threading.Tasks;

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Speaker;

namespace helloworld
{
    class Program
    {
        // helper function for speaker verification.
        public static async Task VerifySpeakerAsync(SpeechConfig config, VoiceProfile profile)
        {
            var speakerRecognizer = new SpeakerRecognizer(config, AudioConfig.FromWavFileInput(@"myVoiceIsMyPassportVerifyMe04.wav"));
            var model = SpeakerVerificationModel.FromProfile(profile);
            var result = await speakerRecognizer.RecognizeOnceAsync(model);
            if (result.Reason == ResultReason.RecognizedSpeaker)
            {
                Console.WriteLine($"Verified voice profile {result.ProfileId}, score is {result.Score}");
            }
            else if (result.Reason == ResultReason.Canceled)
            {
                var cancellation = SpeakerRecognitionCancellationDetails.FromResult(result);
                Console.WriteLine($"CANCELED {profile.Id}: ErrorCode={cancellation.ErrorCode}");
                Console.WriteLine($"CANCELED {profile.Id}: ErrorDetails={cancellation.ErrorDetails}");
            }
        }

        // helper function for speaker identification.
        public static async Task IdentifySpeakersAsync(SpeechConfig config, List<VoiceProfile> profiles)
        {
            var speakerRecognizer = new SpeakerRecognizer(config, AudioConfig.FromWavFileInput(@"TalkForAFewSeconds16.wav"));
            var model = SpeakerIdentificationModel.FromProfiles(profiles);
            var result = await speakerRecognizer.RecognizeOnceAsync(model);
            if (result.Reason == ResultReason.RecognizedSpeakers)
            {
                Console.WriteLine($"The most similiar voice profile is {result.ProfileId} with similiarity score {result.Score}");
                var raw = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                Console.WriteLine($"The raw json from the service is {raw}");
            }
        }

        // perform enrollment
        public static async Task EnrollSpeakerAsync(VoiceProfileClient client, VoiceProfile profile, string audioFileName)
        {
            // Create audio input for enrollment from audio files. Replace with your own audio files.
            using (var audioInput = AudioConfig.FromWavFileInput(audioFileName))
            {
                var reason = ResultReason.EnrollingVoiceProfile;
                while (reason == ResultReason.EnrollingVoiceProfile)
                {
                    var result = await client.EnrollProfileAsync(profile, audioInput);
                    if (result.Reason == ResultReason.EnrollingVoiceProfile)
                    {
                        Console.WriteLine($"Enrolling profile id {profile.Id}.");
                    }
                    else if (result.Reason == ResultReason.EnrolledVoiceProfile)
                    {
                        Console.WriteLine($"Enrolled profile id {profile.Id}.");
                    }
                    else if (result.Reason == ResultReason.Canceled)
                    {
                        var cancellation = VoiceProfileEnrollmentCancellationDetails.FromResult(result);
                        Console.WriteLine($"CANCELED {profile.Id}: ErrorCode={cancellation.ErrorCode}");
                        Console.WriteLine($"CANCELED {profile.Id}: ErrorDetails={cancellation.ErrorDetails}");
                    }
                    Console.WriteLine($"Summation of pure speech across all enrollments in seconds is {result.EnrollmentsSpeechLength.TotalSeconds}.");
                    Console.WriteLine($"The remaining enrollments speech length in seconds is {result.RemainingEnrollmentsSpeechLength?.TotalSeconds}.");
                    reason = result.Reason;
                }
            }
        }

        // perform speaker identification.
        public static async Task SpeakerIdentificationAsync()
        {
            // Replace with your own subscription key and service region (e.g., "westus").
            string subscriptionKey = "YourSubscriptionKey";
            string region = "YourServiceRegion";

            // Creates an instance of a speech config with specified subscription key and service region.
            var config = SpeechConfig.FromSubscription(subscriptionKey, region);

            // Creates a VoiceProfileClient to enroll your voice profile.
            using (var client = new VoiceProfileClient(config))
            // Creates two text independent voice profiles in one of the supported locales.
            using (var profile1 = await client.CreateProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us"))
            using (var profile2 = await client.CreateProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us"))
            {
                try
                {
                    Console.WriteLine($"Created profiles {profile1.Id} and {profile2.Id} for text independent identification.");

                    await EnrollSpeakerAsync(client, profile1, @"TalkForAFewSeconds16.wav");
                    await EnrollSpeakerAsync(client, profile2, @"neuralActivationPhrase.wav");
                    List<VoiceProfile> profiles = new List<VoiceProfile> { profile1, profile2 };
                    await IdentifySpeakersAsync(config, profiles);
                }
                finally
                {
                    await client.DeleteProfileAsync(profile1);
                    await client.DeleteProfileAsync(profile2);
                }
            }
        }

        // perform speaker verification.
        public static async Task SpeakerVerificationAsync()
        {
            // Replace with your own subscription key and service region (e.g., "westus").
            string subscriptionKey = "YourSubscriptionKey";
            string region = "YourServiceRegion";

            // Creates an instance of a speech config with specified subscription key and service region.
            var config = SpeechConfig.FromSubscription(subscriptionKey, region);

            // Creates a VoiceProfileClient to enroll your voice profile.
            using (var client = new VoiceProfileClient(config))
            // Creates a text dependent voice profile in one of the supported locales using the client.
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextDependentVerification, "en-us"))
            {
                try
                {
                    Console.WriteLine($"Created a profile {profile.Id} for text dependent verification.");
                    string[] trainingFiles = new string[]
                    {
                        @"MyVoiceIsMyPassportVerifyMe01.wav",
                        @"MyVoiceIsMyPassportVerifyMe02.wav",
                        @"MyVoiceIsMyPassportVerifyMe03.wav"
                    };

                    // feed each training file to the enrollment service.
                    foreach (var trainingFile in trainingFiles)
                    {
                        // Create audio input for enrollment from audio file. Replace with your own audio files.
                        using (var audioInput = AudioConfig.FromWavFileInput(trainingFile))
                        {
                            var result = await client.EnrollProfileAsync(profile, audioInput);
                            if (result.Reason == ResultReason.EnrollingVoiceProfile)
                            {
                                Console.WriteLine($"Enrolling profile id {profile.Id}.");
                            }
                            else if (result.Reason == ResultReason.EnrolledVoiceProfile)
                            {
                                Console.WriteLine($"Enrolled profile id {profile.Id}.");
                                await VerifySpeakerAsync(config, profile);
                            }
                            else if (result.Reason == ResultReason.Canceled)
                            {
                                var cancellation = VoiceProfileEnrollmentCancellationDetails.FromResult(result);
                                Console.WriteLine($"CANCELED {profile.Id}: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED {profile.Id}: ErrorDetails={cancellation.ErrorDetails}");
                            }

                            Console.WriteLine($"Number of enrollment audios accepted for {profile.Id} is {result.EnrollmentsCount}.");
                            Console.WriteLine($"Number of enrollment audios needed to complete { profile.Id} is {result.RemainingEnrollmentsCount}.");
                        }
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        static async Task Main(string[] args)
        {
            Console.WriteLine("Speaker Verification:");
            await SpeakerVerificationAsync();

            Console.WriteLine("\nSpeaker Identification:");
            await SpeakerIdentificationAsync();
            Console.WriteLine("Please press <Return> to exit.");
            Console.ReadLine();
        }
    }
}
