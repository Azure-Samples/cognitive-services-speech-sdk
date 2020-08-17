//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static CatchUtils;
    using static Config;

    [TestClass]
    public class SpeakerRecognitionTests : RecognitionTestBase
    {
        protected string LogFile { get; private set; }

        [ClassInitialize]
        public static void TestClassInitialize(TestContext context)
        {
            BaseClassInit(context);
        }

        [ClassCleanup]
        public static void TestClassCleanup()
        {
            ConversationTranslatorExtensionMethods.ResetLogging();
        }

        [TestInitialize]
        public void Initialize()
        {
            WriteLine("Configuration values are:");
            WriteLine($"\tSubscriptionKey: <{SubscriptionsRegionsMap[SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION].Key?.Length ?? -1}>");
            WriteLine($"\tRegion:          <{SubscriptionsRegionsMap[SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION].Region}>");
            WriteLine($"\tDefault Endpoint:        <{DefaultSettingsMap[DefaultSettingKeys.SPEAKER_RECOGNITION_ENDPOINT]}>");

            // start logging to a file. This will be read back and dumped to the trace logs at the end of the
            // test execution
            LogFile = $"Carbon_{TestContext.TestName.FileNameSanitize()}.txt";
            IntPtr res = this.StartLogging(LogFile);
            if (res != IntPtr.Zero)
            {
                WriteLine($"Failed to start logging to {LogFile}. Cause: {res.ToInt64()}");
            }
            else
            {
                WriteLine($"Started logging to {LogFile}");
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "Don't care about exceptions here")]
        [TestCleanup]
        public void TestCleanup()
        {
            try
            {
                // just in case there are any logs that still need to be written
                System.Threading.Thread.Sleep(1000);

                var logFile = new System.IO.FileInfo(LogFile);
                if (!logFile.Exists)
                {
                    WriteLine("Log file did not exist");
                    return;
                }

                // force the log file to close otherwise we can't access it here
                IntPtr res = this.StopLogging();
                if (res != IntPtr.Zero)
                {
                    WriteLine($"Failed to stop logging to {LogFile}: {res.ToInt64()}");
                }

                // dump log file so the output stream
                foreach (var line in System.IO.File.ReadLines(logFile.FullName))
                {
                    DumpLine(line);
                }

                logFile.Delete();
            }
            catch (Exception e)
            {
                WriteLine($"Encountered an exception when trying to read {LogFile}. {e.GetType().FullName}: {e}");
            }
        }

        [TestMethod]
        public async Task text_independent_verification_enrollment()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextIndependentVerification, "en-us"))
            {
                try
                {
                    SPXTEST_REQUIRE(!String.IsNullOrEmpty(profile.Id));

                    using (var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath()))
                    {
                        var result = await client.EnrollProfileAsync(profile, audioInput);
                        var json_string = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        SPXTEST_REQUIRE(!String.IsNullOrEmpty(json_string));
                        SPXTEST_REQUIRE(result.ProfileId == profile.Id);
                        SPXTEST_REQUIRE(result.EnrollmentsCount == 1);
                        SPXTEST_REQUIRE(result.EnrollmentsSpeechLength >= new TimeSpan((long)239200000));
                        SPXTEST_REQUIRE(result.Reason == ResultReason.EnrolledVoiceProfile);
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task text_independent_identification_enrollment()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us"))
            {
                try
                {
                    SPXTEST_REQUIRE(!String.IsNullOrEmpty(profile.Id));

                    using (var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath()))
                    {
                        var result = await client.EnrollProfileAsync(profile, audioInput);
                        var json_string = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        SPXTEST_REQUIRE(!String.IsNullOrEmpty(json_string));
                        SPXTEST_REQUIRE(result.ProfileId == profile.Id);
                        SPXTEST_REQUIRE(result.EnrollmentsCount == 1);
                        SPXTEST_REQUIRE(result.RemainingEnrollmentsCount == null);
                        SPXTEST_REQUIRE(result.RemainingEnrollmentsSpeechLength >= new TimeSpan((long)0));
                        SPXTEST_REQUIRE(result.EnrollmentsSpeechLength >= new TimeSpan((long)239200000));
                        SPXTEST_REQUIRE(result.Reason == ResultReason.EnrolledVoiceProfile);
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task text_dependent_verification_enrollment()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextDependentVerification, "en-us"))
            {
                try
                {
                    SPXTEST_REQUIRE(!String.IsNullOrEmpty(profile.Id));

                    using (var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath()))
                    {
                        var result = await client.EnrollProfileAsync(profile, audioInput);
                        var json_string = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        SPXTEST_REQUIRE(!String.IsNullOrEmpty(json_string));
                        SPXTEST_REQUIRE(result.Reason == ResultReason.Canceled);
                        var details = VoiceProfileEnrollmentCancellationDetails.FromResult(result);
                        SPXTEST_REQUIRE(details.ErrorCode == CancellationErrorCode.BadRequest);
                        SPXTEST_REQUIRE(details.Reason == CancellationReason.Error);
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task text_dependent_verification_enrollment_good_case()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextDependentVerification, "en-us"))
            {
                try
                {
                    SPXTEST_REQUIRE(!String.IsNullOrEmpty(profile.Id));

                    using (var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SPEAKER_VERIFYCATION_ENGLISH].FilePath.GetRootRelativePath()))
                    {
                        var result = await client.EnrollProfileAsync(profile, audioInput);
                        var json_string = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        SPXTEST_REQUIRE(!String.IsNullOrEmpty(json_string));
                        SPXTEST_REQUIRE(result.Reason == ResultReason.EnrollingVoiceProfile);
                        SPXTEST_REQUIRE(result.RemainingEnrollmentsCount >= 0);
                        SPXTEST_REQUIRE(result.RemainingEnrollmentsSpeechLength == null);
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task ResetVoiceProfile1()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextDependentVerification, "en-us"))
            {
                try
                {
                    var result = await client.ResetProfileAsync(profile);
                    SPXTEST_REQUIRE(result.Reason == ResultReason.ResetVoiceProfile);

                    var details = VoiceProfileCancellationDetails.FromResult(result);
                    SPXTEST_REQUIRE(details.ErrorCode == CancellationErrorCode.NoError);
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
        }
        }

        [TestMethod]
        public async Task ResetVoiceProfile2()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextIndependentVerification, "en-us"))
            {
                try
                {
                    var result = await client.ResetProfileAsync(profile);
                    SPXTEST_REQUIRE(result.Reason == ResultReason.ResetVoiceProfile);
                    SPXTEST_REQUIRE(String.IsNullOrEmpty(result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult)));
                    var details = VoiceProfileCancellationDetails.FromResult(result);
                    SPXTEST_REQUIRE(details.ErrorCode == CancellationErrorCode.NoError);
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task ResetDeleteResetVoiceProfile()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us"))
            {
                try
                {
                    var result = await client.ResetProfileAsync(profile);
                    SPXTEST_REQUIRE(result.Reason == ResultReason.ResetVoiceProfile);

                    result = await client.DeleteProfileAsync(profile);
                    SPXTEST_REQUIRE(result.Reason == ResultReason.DeletedVoiceProfile);

                    result = await client.DeleteProfileAsync(profile);
                    SPXTEST_REQUIRE(result.Reason == ResultReason.Canceled);
                    var details = VoiceProfileCancellationDetails.FromResult(result);

                    var expectedSubstring = "can't be found";
                    Assert.IsTrue(details.ErrorDetails.Contains(expectedSubstring),
                        $"Didn't find expected substring '{expectedSubstring}' in error details.\nActual: {details.ErrorDetails}");
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task SpeakerVerification()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextIndependentVerification, "en-us"))
            {
                try
                {
                    SPXTEST_REQUIRE(!String.IsNullOrEmpty(profile.Id));

                    using (var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath()))
                    {
                        var result = await client.EnrollProfileAsync(profile, audioInput);
                        var json_string = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        SPXTEST_REQUIRE(!String.IsNullOrEmpty(json_string));
                        SPXTEST_REQUIRE(result.Reason == ResultReason.EnrolledVoiceProfile);

                        using (var speakerRecognizer = new SpeakerRecognizer(config, audioInput))
                        using (var model = SpeakerVerificationModel.FromProfile(profile))
                        {
                            var result2 = await speakerRecognizer.RecognizeOnceAsync(model);
                            SPXTEST_REQUIRE(result2.Score > 0.5);
                            SPXTEST_REQUIRE(result2.Reason == ResultReason.RecognizedSpeaker);
                            SPXTEST_REQUIRE(result2.ProfileId == profile.Id);
                            using (var wrongAudioFile = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath()))
                            using (var recognizer2 = new SpeakerRecognizer(config, wrongAudioFile))
                            {
                                var result3 = await recognizer2.RecognizeOnceAsync(model);
                                SPXTEST_REQUIRE(result3.Reason == ResultReason.NoMatch);
                                var rawJsonStringFromService = result3.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                                SPXTEST_REQUIRE(rawJsonStringFromService.IndexOf("reject", StringComparison.OrdinalIgnoreCase) >= 0);
                            }
                        }
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        [TestMethod]
        public async Task SpeakerIdentification()
        {
            var config = this.GetProdSpeakerRecognitionSubscriptionConfig();

            using (var client = new VoiceProfileClient(config))
            using (var profile = await client.CreateProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us"))
            {
                try
                {
                    SPXTEST_REQUIRE(!String.IsNullOrEmpty(profile.Id));

                    using (var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.MULTIPLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath()))
                    {
                        var result = await client.EnrollProfileAsync(profile, audioInput);
                        var json_string = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        SPXTEST_REQUIRE(!String.IsNullOrEmpty(json_string));
                        SPXTEST_REQUIRE(result.Reason == ResultReason.EnrolledVoiceProfile);

                        using (var profile2 = await client.CreateProfileAsync(VoiceProfileType.TextIndependentIdentification, "en-us"))
                        {
                            SPXTEST_REQUIRE(!string.IsNullOrEmpty(profile2.Id));
                            var enrollResult2 = await client.EnrollProfileAsync(profile2, audioInput);
                            SPXTEST_REQUIRE(enrollResult2.Reason == ResultReason.EnrolledVoiceProfile);

                            var profiles = new List<VoiceProfile>();
                            profiles.Add(profile);
                            profiles.Add(profile2);
                            using (var model = SpeakerIdentificationModel.FromProfiles(profiles))
                            {
                                var recognizer = new SpeakerRecognizer(config, audioInput);
                                var result3 = await recognizer.RecognizeOnceAsync(model);
                                SPXTEST_REQUIRE(result3.Reason == ResultReason.RecognizedSpeakers);
                            }
                        }
                    }
                }
                finally
                {
                    await client.DeleteProfileAsync(profile);
                }
            }
        }

        private SpeechConfig GetProdSpeakerRecognitionSubscriptionConfig()
        {
            var index = SubscriptionsRegionsKeys.SPEAKER_RECOGNITION_SUBSCRIPTION;

            var subscriptionKey = SubscriptionsRegionsMap[index].Key;
            var subscriptionRegion = SubscriptionsRegionsMap[index].Region;

            Assert.IsTrue(!string.IsNullOrEmpty(subscriptionKey),
                $"Couldn't find a speaker recognition subscription key. Check the test settings JSON for '{index}'.");
            Assert.IsTrue(!string.IsNullOrEmpty(subscriptionRegion),
                $"Couldn't find a speaker recognition subscription region. Check the test settings JSON. for '{index}'.");

            return SpeechConfig.FromSubscription(subscriptionKey, subscriptionRegion);
        }
    }
}
