//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Conversation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using MicrosoftSpeechSDKSamples;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static ConversationTranscriberTestsHelper;
    using static SpeechRecognitionTestsHelper;

    [TestClass]
    public class ConversationTranscriberTests : RecognitionTestBase
    {
        private ConversationTranscriberTestsHelper helper;
        private static string conversationTranscriptionMultiAudioEndpoint;

        [ClassInitialize]
        public static void TestClassinitialize(TestContext context)
        {
            BaseClassInit(context);
            conversationTranscriptionMultiAudioEndpoint = conversationTranscriptionEndpoint + "/multiaudio?";
        }

        [TestInitialize]
        public void Initialize()
        {
            helper = new ConversationTranscriberTestsHelper();
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public void ConversationIdWithChinese()
        {
            var config = SpeechConfig.FromSubscription(subscriptionKey, region);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather8Channels.AudioFile);
            using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(config, audioInput)))
            {
                //the UTF8 decoding of 的 is \xe7\x9a\x84, which will be shown in the debugger in the C++ side.
                string myConversationId = "的";
                conversationTranscriber.ConversationId = myConversationId;
                var gotId = conversationTranscriber.ConversationId;
                Assert.AreEqual(myConversationId, gotId);
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public void ConversationIdWithAnsiOnly()
        {
            var config = SpeechConfig.FromSubscription(subscriptionKey, region);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather8Channels.AudioFile);
            using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(config, audioInput)))
            {
                string myConversationId = "123 456";
                conversationTranscriber.ConversationId = myConversationId;
                var gotId = conversationTranscriber.ConversationId;
                Assert.AreEqual(myConversationId, gotId);
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public void ConversationCreateUsers()
        {
            string myId = "xyz@example.com";
            var user = User.FromUserId(myId);
            Assert.AreEqual(myId, user.UserId);
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public void ConversationCreateParticipants()
        {
            var participant2 = Participant.From("xyz@example.com", "", string.Empty);
            participant2.PreferredLanguage = "zh-cn";
            bool exception = false;
            try
            {
                participant2.PreferredLanguage = "";
            }
            catch (Exception ex)
            {
                Console.WriteLine("Got Exception: " + ex.Message.ToString());
                exception = true;
            }
            Assert.AreEqual(exception, true);

            // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
            string voice = "1.1, 2.2";
            participant2.VoiceSignature = voice;

            exception = false;
            try
            {
                participant2.VoiceSignature = string.Empty;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Got Exception: " + ex.Message.ToString());
                exception = true;
            }
            Assert.AreEqual(exception, true);
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ConversationAddParticipant()
        {
            var config = SpeechConfig.FromEndpoint(new Uri(conversationTranscriptionMultiAudioEndpoint), subscriptionKey);

            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather8Channels.AudioFile);

            using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(config, audioInput)))
            {
                conversationTranscriber.ConversationId = "TestCreatingParticipantByUserClass";

                conversationTranscriber.AddParticipant("OneUserByUserId");

                var user = User.FromUserId("CreateUserFromId and then add it");
                conversationTranscriber.AddParticipant(user);

                // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
                string voice = "1.1, 2.2";
                var participant = Participant.From("userIdForParticipant", "en-us", voice);
                conversationTranscriber.AddParticipant(participant);

                var result = await helper.GetFirstRecognizerResult(conversationTranscriber);
                AssertMatching(TestData.English.Weather8Channels.Utterance, result);
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ConversationRemoveParticipant()
        {
            var config = SpeechConfig.FromEndpoint(new Uri(conversationTranscriptionMultiAudioEndpoint), subscriptionKey);
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather8Channels.AudioFile);
            using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(config, audioInput)))
            {
                conversationTranscriber.ConversationId = "TestCreatingParticipantByUserClass";
                bool exception = false;
                try
                {
                    conversationTranscriber.RemoveParticipant("NoneExist");
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Got Exception: " + ex.Message.ToString());
                    exception = true;
                }
                Assert.AreEqual(exception, true);

                conversationTranscriber.AddParticipant("OneUserByUserId");
                conversationTranscriber.RemoveParticipant("OneUserByUserId");

                var user = User.FromUserId("user object created from User.FromUserId");
                conversationTranscriber.AddParticipant(user);
                conversationTranscriber.RemoveParticipant(user);

                // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
                string voice = "1.1, 2.2";
                var participant = Participant.From("userIdForParticipant", "en-us", voice);
                conversationTranscriber.AddParticipant(participant);
                conversationTranscriber.RemoveParticipant(participant);

                var result = await helper.GetFirstRecognizerResult(conversationTranscriber);
                AssertMatching(TestData.English.Weather8Channels.Utterance, result);
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ConversationPushStream()
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri(conversationTranscriptionMultiAudioEndpoint), subscriptionKey);
            var stopRecognition = new TaskCompletionSource<int>();
            string recoResult = string.Empty;

            using (var pushStream = AudioInputStream.CreatePushStream())
            {
                using (var audioInput = AudioConfig.FromStreamInput(pushStream))
                {
                    using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(config, audioInput)))
                    {
                        conversationTranscriber.ConversationId = "ConversationPushStreamTest";

                        conversationTranscriber.AddParticipant("xyz@example.com");

                        conversationTranscriber.Recognizing += (s, e) =>
                        {
                            Console.WriteLine($"RECOGNIZING: Text={e.Result.Text}");
                        };

                        conversationTranscriber.Recognized += (s, e) =>
                        {
                            if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                                if (string.IsNullOrEmpty(recoResult))
                                {
                                    recoResult = e.Result.Text;
                                }
                            }
                            else if (e.Result.Reason == ResultReason.NoMatch)
                            {
                                Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                            }
                        };

                        conversationTranscriber.Canceled += (s, e) =>
                        {
                            Console.WriteLine($"CANCELED: Reason={e.Reason}");

                            if (e.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            }

                            stopRecognition.TrySetResult(0);
                        };

                        conversationTranscriber.SessionStarted += (s, e) =>
                        {
                            Console.WriteLine("\nSession started event.");
                        };

                        conversationTranscriber.SessionStopped += (s, e) =>
                        {
                            Console.WriteLine("\nSession stopped event.");
                            Console.WriteLine("\nStop recognition.");
                            stopRecognition.TrySetResult(0);
                        };

                        // Starts continuous recognition. Uses StopTranscribingAsync() to stop transcribing.
                        await conversationTranscriber.StartTranscribingAsync().ConfigureAwait(false);

                        // open and read the wave file and push the buffers into the recognizer
                        using (var reader = Util.CreateWavReader(TestData.English.Weather8Channels.AudioFile))
                        {
                            byte[] buffer = new byte[3200];
                            while (true)
                            {
                                var readSamples = reader.Read(buffer, (uint)buffer.Length);
                                if (readSamples == 0)
                                {
                                    break;
                                }
                                pushStream.Write(buffer, readSamples);
                                pushStream.SetProperty(PropertyId.ConversationTranscribingService_DataBufferTimeStamp, "fakeTimeStamp");
                                pushStream.SetProperty("DataChunk_SpeakerId", "fakeSpeakerId");
                            }
                        }
                        pushStream.Close();

                        // Waits for completion.
                        // Use Task.WaitAny to keep the task rooted.
                        Task.WaitAny(new[] { stopRecognition.Task });

                        await conversationTranscriber.StopTranscribingAsync().ConfigureAwait(false);
                        Assert.IsFalse(string.IsNullOrEmpty(recoResult));
                    }
                }
            }
        }

        [TestMethod, TestCategory(TestCategory.LongRunning)]
        public async Task ConversationPullStream()
        {
            var config = SpeechConfig.FromEndpoint(new Uri(conversationTranscriptionMultiAudioEndpoint), subscriptionKey);
            var stopRecognition = new TaskCompletionSource<int>();
            bool bGotReco = false;

            using (var audioInput = Util.OpenWavFile(TestData.English.Weather8Channels.AudioFile))
            {
                using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(config, audioInput)))
                {
                    conversationTranscriber.ConversationId = "ConversationPullStreamTest";
                    conversationTranscriber.AddParticipant("xyz@example.com");

                    conversationTranscriber.Recognizing += (s, e) =>
                    {
                        Console.WriteLine($"RECOGNIZING: Text={e.Result.Text} SpeakerId={e.Result.UserId}");
                    };

                    conversationTranscriber.Recognized += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"RECOGNIZED: Text={e.Result.Text} SpeakerId={e.Result.UserId}");
                            bGotReco = true;
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    conversationTranscriber.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }

                        stopRecognition.TrySetResult(0);
                    };

                    conversationTranscriber.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine("\nSession started event.");
                    };

                    conversationTranscriber.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine("\nSession stopped event.");
                        Console.WriteLine("\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    };

                    // Starts continuous recognition. Uses StopTranscribingAsync() to stop transcribing.
                    await conversationTranscriber.StartTranscribingAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopRecognition.Task });

                    await conversationTranscriber.StopTranscribingAsync().ConfigureAwait(false);
                    Assert.IsTrue(bGotReco);
                }
            }
        }
    }
}
