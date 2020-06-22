//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Transcription;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static CatchUtils;
    using static Config;
    using static ConversationTranslatorTestConstants;
    using TRANS = Dictionary<string, string>;

    [TestClass]
    public class ConversationTranslatorTests : RecognitionTestBase
    {
        internal static Uri ManagementEndpoint { get; private set; }
        internal static Uri WebSocketEndpoint { get; private set; }

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
            WriteLine($"\tEndpoint:        <{DefaultSettingsMap[DefaultSettingKeys.ENDPOINT]}>");
            WriteLine($"\tConvTransHost:   <{DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_HOST]}>");
            WriteLine($"\tConvTransRegion: <{SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Region}>");
            WriteLine($"\tConvTransKey:    <{SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Key?.Length ?? -1}>");

            if (!string.IsNullOrWhiteSpace(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_HOST]))
            {
                ManagementEndpoint = new Uri($"https://{DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_HOST]}/capito/room");
                WebSocketEndpoint = new Uri($"wss://{DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_HOST]}/capito/translate");
            }

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
        public async Task CT_Conversation_WithoutTranslations()
        {
            var speechConfig = CreateConfig(Language.EN);
            using (var conversation = await Conversation.CreateConversationAsync(speechConfig))
            {
                REQUIRE(!string.IsNullOrWhiteSpace(conversation.ConversationId));
                WriteLine($"Created room {conversation.ConversationId}");
                await conversation.DeleteConversationAsync();
            }
        }

        [TestMethod]
        public async Task CT_Conversation_WithTranslations()
        {
            var speechConfig = CreateConfig(Language.EN, Language.FR);
            using (var conversation = await Conversation.CreateConversationAsync(speechConfig))
            {
                REQUIRE(!string.IsNullOrWhiteSpace(conversation.ConversationId));
                WriteLine($"Created room {conversation.ConversationId}");
                await conversation.DeleteConversationAsync();
                WriteLine($"Deleted room");
            }
        }

        [TestMethod]
        public async Task CT_Conversation_Dispose()
        {
            var speechConfig = CreateConfig(Language.EN, Language.FR, "ar");
            using (var conversation = await Conversation.CreateConversationAsync(speechConfig))
            {
                REQUIRE(!string.IsNullOrWhiteSpace(conversation.ConversationId));
                WriteLine($"Created room {conversation.ConversationId}");

                // don't delete
            }
        }

        [TestMethod]
        public async Task CT_Conversation_DisposeAfterStart()
        {
            var speechConfig = CreateConfig(Language.EN, Language.FR, "ar");
            using (var conversation = await Conversation.CreateConversationAsync(speechConfig))
            {
                REQUIRE(!string.IsNullOrWhiteSpace(conversation.ConversationId));
                WriteLine($"Created room {conversation.ConversationId}");

                await conversation.StartConversationAsync();
                WriteLine($"Started room {conversation.ConversationId}");
            }
        }

        [TestMethod]
        public async Task CT_Conversation_MethodsWhileNotStarted()
        {
            var speechConfig = CreateConfig(Language.EN, Language.FR, "ar");
            using (var conversation = await Conversation.CreateConversationAsync(speechConfig))
            {
                REQUIRE(!string.IsNullOrWhiteSpace(conversation.ConversationId));
                WriteLine($"Created room {conversation.ConversationId}");

                WriteLine($"Trying to lock room");
                await conversation.LockConversationAsync().ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");
                WriteLine($"Trying to unlock room");
                await conversation.UnlockConversationAsync().ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");
                WriteLine($"Trying to mute a participant");
                await conversation.MuteParticipantAsync("id").ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");
                WriteLine($"Trying to unmute a participant");
                await conversation.UnmuteParticipantAsync("id").ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");
                WriteLine($"Trying to mute all participants");
                await conversation.MuteAllParticipantsAsync().ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");
                WriteLine($"Trying to unmute all participants");
                await conversation.UnmuteAllParticipantsAsync().ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");
                WriteLine($"Trying to remove a participant");
                await conversation.RemoveParticipantAsync("id").ThrowsException<ApplicationException>("SPXERR_INVALID_STATE");

                // these should not throw exceptions
                WriteLine($"Trying to get auth token");
                string _ = conversation.AuthorizationToken;
                WriteLine($"Trying to set auth token");
                conversation.AuthorizationToken = "random";

                WriteLine($"Ending conversation");
                await conversation.EndConversationAsync();
                WriteLine($"Deleting conversation");
                await conversation.DeleteConversationAsync();
            }
        }

        [TestMethod]
        public async Task CT_Conversation_CallUnsupportedMethods()
        {
            WriteLine($"Checking methods on Conversation instance for the ConversationTranslator");
            var speechConfig = CreateConfig(Language.EN, Language.FR, "ar");
            using (var conv = await Conversation.CreateConversationAsync(speechConfig))
            {
                WriteLine($"Created room {conv.ConversationId}");
                await conv.StartConversationAsync();
                WriteLine($"Started room");

                var user = User.FromUserId("userId");
                var part = Participant.From("userId", Language.EN, null);

                WriteLine($"Trying to add a user");
                await conv.AddParticipantAsync(user).ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to add a participant");
                await conv.AddParticipantAsync(part).ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to add a user Id");
                await conv.AddParticipantAsync("userId").ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");

                // should not throw
                WriteLine($"Ending conversation");
                await conv.EndConversationAsync();
                WriteLine($"Deleting conversation");
                await conv.DeleteConversationAsync();
            }

            WriteLine($"Checking methods on Conversation instance for the ConversationTranscriber");
            speechConfig.SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
            using (var conv = await Conversation.CreateConversationAsync(speechConfig))
            {
                WriteLine($"Created room {conv.ConversationId}");

                var user = User.FromUserId("userId");
                var part = Participant.From("userId", Language.EN, null);

                WriteLine($"Trying to start a conversation");
                await conv.StartConversationAsync().ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to lock a conversation");
                await conv.LockConversationAsync().ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to unlock a conversation");
                await conv.UnlockConversationAsync().ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to mute all participants");
                await conv.MuteAllParticipantsAsync().ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to unmute all participants");
                await conv.UnmuteAllParticipantsAsync().ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to mute a participant");
                await conv.MuteParticipantAsync("userId").ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to unmute a participant");
                await conv.UnmuteParticipantAsync("userId").ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR");
                WriteLine($"Trying to delete conversation");
                await conv.DeleteConversationAsync().ThrowsException<ApplicationException>("SPXERR_UNSUPPORTED_API_ERROR"); ;
            }
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_HostAudio()
        {
            var audioUtterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH];

            string speechLang = Language.EN;
            string hostname = "TheHost";
            var toLangs = new[] { Language.FR, Language.DE };
            var speechConfig = CreateConfig(speechLang, toLangs);

            SPX_TRACE_INFO("Creating conversation");
            var conversation = await Conversation.CreateConversationAsync(speechConfig);
            SPX_TRACE_INFO($"Starting {conversation.ConversationId} conversation");
            await conversation.StartConversationAsync();

            var audioConfig = AudioConfig.FromWavFileInput(audioUtterance.FilePath.GetRootRelativePath());
            SPX_TRACE_INFO("Creating ConversationTranslator");
            var conversationTranslator = new ConversationTranslator(audioConfig);

            var eventHandlers = new ConversationTranslatorCallbacks(conversationTranslator);

            SPX_TRACE_INFO("Joining conversation");
            await conversationTranslator.JoinConversationAsync(conversation, hostname);
            SPX_TRACE_INFO("Start transcribing");
            await conversationTranslator.StartTranscribingAsync();

            await eventHandlers.WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);


            SPX_TRACE_INFO("Stop Transcribing");
            await conversationTranslator.StopTranscribingAsync();

            SPX_TRACE_INFO("Leave conversation");
            await conversationTranslator.LeaveConversationAsync();

            SPX_TRACE_INFO("End conversation");
            await conversation.EndConversationAsync();
            SPX_TRACE_INFO("Delete conversation");
            await conversation.DeleteConversationAsync();

            SPX_TRACE_INFO("Verifying callbacks");

            string participantId;
            eventHandlers.VerifyBasicEvents(true, hostname, true, out participantId);
            eventHandlers.VerifyTranscriptions(participantId,
                new ExpectedTranscription(participantId, audioUtterance.Utterances[Language.EN][0].Text, speechLang)
            );
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_JoinWithTranslation()
        {
            string hostLang = Language.EN;
            string hostName = "TheHost";
            var hostToLangs = new[] { Language.DE };
            string bobLang = Language.ZH_CN;
            string bobName = "Bob";
            string hostId;
            string bobId;

            // Create room
            var speechConfig = CreateConfig(hostLang, hostToLangs);

            SPX_TRACE_INFO("Creating host conversation");
            var conversation = await Conversation.CreateConversationAsync(speechConfig);
            SPX_TRACE_INFO($"Starting host {conversation.ConversationId} conversation");
            await conversation.StartConversationAsync();

            var audioConfig = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            SPX_TRACE_INFO("Creating host ConversationTranslator");
            var hostTranslator = new ConversationTranslator(audioConfig);
            SPX_TRACE_INFO("Attaching event handlers");
            var hostEvents = new ConversationTranslatorCallbacks(hostTranslator);
            SPX_TRACE_INFO("Joining host");
            await hostTranslator.JoinConversationAsync(conversation, hostName);


            // Join room
            var bobAudioConfig = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CHINESE].FilePath.GetRootRelativePath());
            SetParticipantConfig(bobAudioConfig);
            SPX_TRACE_INFO("Creating bob ConversationTranslator");
            var bobTranslator = new ConversationTranslator(bobAudioConfig);
            var bobEvents = new ConversationTranslatorCallbacks(bobTranslator);
            SPX_TRACE_INFO("Bob joining {0}", conversation.ConversationId);
            await bobTranslator.JoinConversationAsync(conversation.ConversationId, bobName, bobLang);


            // do audio playback
            SPX_TRACE_INFO("Starting host audio");
            await hostTranslator.StartTranscribingAsync();
            SPX_TRACE_INFO("Starting bob audio");
            await bobTranslator.StartTranscribingAsync();

            SPX_TRACE_INFO("Waiting for host audio to complete");
            await hostEvents.WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE);
            SPX_TRACE_INFO("Waiting for bob host audio to complete");
            await bobEvents.WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);

            SPX_TRACE_INFO("Stopping host audio");
            await bobTranslator.StopTranscribingAsync();
            SPX_TRACE_INFO("Stopping bob audio");
            await hostTranslator.StopTranscribingAsync();


            // bob leaves
            SPX_TRACE_INFO("Bob leaves");
            await bobTranslator.LeaveConversationAsync();


            // host leaves
            await Task.Delay(1000);
            SPX_TRACE_INFO("Host leaves");
            await hostTranslator.LeaveConversationAsync();
            SPX_TRACE_INFO("Host ends conversation");
            await conversation.EndConversationAsync();
            SPX_TRACE_INFO("Host deletes conversation");
            await conversation.DeleteConversationAsync();

            // verify events
            SPX_TRACE_INFO("Verifying callbacks");
            bobEvents.VerifyBasicEvents(true, bobName, false, out bobId);
            hostEvents.VerifyBasicEvents(true, hostName, true, out hostId);
            bobEvents.VerifyTranscriptions(bobId,
                new ExpectedTranscription(bobId, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.ZH_CN][0].Text, bobLang),
                new ExpectedTranscription(hostId, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, hostLang)
            );
            hostEvents.VerifyTranscriptions(hostId,
                new ExpectedTranscription(bobId, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.ZH_CN][0].Text, bobLang, 0, new TRANS() { { Language.EN, "Weather." }, { Language.DE, "wetter." } }),
                new ExpectedTranscription(hostId, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, hostLang, 0, new TRANS() { { Language.DE, "Wie ist das Wetter?" } })
            );
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_HostSendsIm()
        {
            var speechConfig = CreateConfig(Language.EN, "ja", "ar");
            var host = new TestConversationParticipant(speechConfig, "Host");

            await host.JoinAsync(null);

            SPX_TRACE_INFO($">> [{host.Name}] Sends IM");
            await host.Translator.SendTextMessageAsync("This is a test"); // Translations for this are hard coded
            await Task.Delay(TimeSpan.FromSeconds(3));

            await host.LeaveAsync();

            // verify events
            host.VerifyBasicEvents(false);
            host.VerifyIms(
                new ExpectedTranscription(host.ParticipantId, "This is a test", host.Lang, 1, new TRANS() { { "ja", "これはテストです" }, { "ar", "هذا اختبار" } })
            );
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_HostAndParticipantSendIms()
        {
            var speechConfig = CreateConfig(Language.EN);

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(null);

            var alice = new TestConversationParticipant("Alice", Language.FR, host, SetParticipantConfig);
            await alice.JoinAsync(null);

            SPX_TRACE_INFO($">> [{host.Name}] Sends IM");
            await host.Translator.SendTextMessageAsync("This is a test");
            await Task.Delay(TimeSpan.FromSeconds(1));
            SPX_TRACE_INFO($">> [{alice.Name}] Sends IM in French");
            await alice.Translator.SendTextMessageAsync("C'est un test");

            await Task.Delay(TimeSpan.FromSeconds(3));

            await alice.LeaveAsync();
            await host.LeaveAsync();

            // verify events
            alice.VerifyBasicEvents(false);
            host.VerifyBasicEvents(false);

            var expectedIms = new[]
            {
                new ExpectedTranscription(host.ParticipantId, "This is a test", host.Lang, 1, new TRANS() {{ alice.Lang, "C'est un test" }}),
                new ExpectedTranscription(alice.ParticipantId, "C'est un test", alice.Lang, 1, new TRANS() {{ host.Lang, "This is a test" }})
            };

            alice.VerifyIms(expectedIms);
            host.VerifyIms(expectedIms);
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_JoinLockedRoom()
        {
            var speechConfig = CreateConfig(Language.EN);

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(null);
            await host.Conversation.LockConversationAsync();

            await Task.Delay(1000);

            var alice = new TestConversationParticipant("Alice", Language.FR, host, SetParticipantConfig);

            REQUIRE_THROWS_WITH(alice.JoinAsync(null), Catch.Contains("HTTP 400", Catch.CaseSensitive.No));

            await host.LeaveAsync();
            host.VerifyBasicEvents(false);
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_CallMethodsWhenNotJoined()
        {
            SPX_TRACE_INFO("========== Host ==========");
            {
                var speechConfig = CreateConfig("de-DE");
                var conversation = await Conversation.CreateConversationAsync(speechConfig);
                await conversation.StartConversationAsync();

                var translator = new ConversationTranslator();

                REQUIRE_THROWS_MATCHES(
                    translator.StartTranscribingAsync(),
                    typeof(ApplicationException),
                    Catch.HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.StopTranscribingAsync(),
                    typeof(ApplicationException),
                    Catch.HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.SendTextMessageAsync("This is a test"),
                    typeof(ApplicationException),
                    Catch.HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_NOTHROW(translator.LeaveConversationAsync());
            }

            SPX_TRACE_INFO("========== Participant ==========");
            {
                var translator = new ConversationTranslator();

                REQUIRE_THROWS_MATCHES(
                    translator.StartTranscribingAsync(),
                    typeof(ApplicationException),
                    Catch.HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.StopTranscribingAsync(),
                    typeof(ApplicationException),
                    Catch.HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.SendTextMessageAsync("This is a test"),
                    typeof(ApplicationException),
                    Catch.HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_NOTHROW(translator.LeaveConversationAsync());
            }
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_DoubleJoinShouldFail()
        {
            var hostSpeechConfig = CreateConfig(Language.EN);

            var host = new TestConversationParticipant (hostSpeechConfig, "Host");
            await host.JoinAsync(null);

            // host tries to join again. should fail
            REQUIRE_THROWS_MATCHES(
                host.Translator.JoinConversationAsync(host.Conversation, host.Name),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE"));

            var alice = new TestConversationParticipant("Alice", Language.ZH_CN, host, SetParticipantConfig);
            await alice.JoinAsync(null);

            // Alice tries to join again, should fail
            REQUIRE_THROWS_MATCHES(
                alice.Translator.JoinConversationAsync(host.ConversationId, alice.Name, alice.Lang),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE"));

            await alice.LeaveAsync();
            await host.LeaveAsync();
        }

        [TestMethod]
        public void ConversationTranslator_ConnectionBeforeJoin()
        {
            var audioConfig = AudioConfig.FromWavFileInput(Config.AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var conversationTranslator = new ConversationTranslator(audioConfig);

            var connection = Connection.FromConversationTranslator(conversationTranslator);

            REQUIRE_THROWS_MATCHES(
                () => connection.Open(false),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE")
            );

            REQUIRE_THROWS_MATCHES(
                () => connection.Open(true),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE")
            );

            // Close should not throw exceptions
            REQUIRE_NOTHROW(() => connection.Close());
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_ConnectionAfterLeave()
        {
            var speechConfig = CreateConfig(Language.EN);

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(null);

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            await host.LeaveAsync();

            // connecting should fail now
            REQUIRE_THROWS_MATCHES(
                () => host.Connection.Open(false),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE")
            );

            REQUIRE_THROWS_MATCHES(
                () => host.Connection.Open(true),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE")
            );

            // Close should not throw exceptions
            REQUIRE_NOTHROW(() => host.Connection.Close());
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_RecognizerEventsAndMethods()
        {
            var speechConfig = CreateConfig(Language.EN);
            var audioConfig = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(null);

            var evts = new List<ConnectionMessage>();

            host.Connection.MessageReceived += (s, e) =>
            {
                evts.Add(e.Message.Dump("Recognizer message received"));
            };

            await host.StartAudioAsync();

            await host.WaitForAudioToFinish();

            // send message
            await host.Connection.SendMessageAsync("speech.context", "{\"translationcontext\":{\"to\":[\"en-US\"]}}");

            await host.LeaveAsync();

            SPXTEST_REQUIRE(evts.Count > 0);
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_HostLeaveRejoin()
        {
            var audioFile = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH];

            var speechLang = Language.EN;
            var hostname = "TheHost";

            var toLangs = new[] { "fr", "de" };
            var speechConfig = CreateConfig(speechLang, toLangs);

            SPX_TRACE_INFO("Creating conversation");
            var conversation = await Conversation.CreateConversationAsync(speechConfig);
            SPX_TRACE_INFO("Starting conversation");
            await conversation.StartConversationAsync();

            var audioConfig = AudioConfig.FromWavFileInput(audioFile.FilePath.GetRootRelativePath());
            var conversationTranslator = new ConversationTranslator(audioConfig);

            var eventHandlers = new ConversationTranslatorCallbacks(conversationTranslator);

            SPX_TRACE_INFO("Joining conversation");
            await conversationTranslator.JoinConversationAsync(conversation, hostname);

            SPX_TRACE_INFO("Getting connection object");
            var connection = Connection.FromConversationTranslator(conversationTranslator);
            eventHandlers.AddConnectionCallbacks(connection);

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            SPX_TRACE_INFO("Disconnecting conversation");
            connection.Close();

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            SPX_TRACE_INFO("Reconnecting conversation");
            connection.Open(false);

            SPX_TRACE_INFO("Start transcribing after reconnect");
            await conversationTranslator.StartTranscribingAsync();

            SPX_TRACE_INFO("Send text message after reconnect");
            await conversationTranslator.SendTextMessageAsync("This is a test");

            await eventHandlers.WaitForAudioStreamCompletion(MAX_WAIT_FOR_AUDIO_TO_COMPLETE, WAIT_AFTER_AUDIO_COMPLETE);

            SPX_TRACE_INFO("Stop Transcribing");
            await conversationTranslator.StopTranscribingAsync();

            SPX_TRACE_INFO("Leave conversation");
            await conversationTranslator.LeaveConversationAsync();

            SPX_TRACE_INFO("End conversation");
            await conversation.EndConversationAsync();
            SPX_TRACE_INFO("Delete conversation");
            await conversation.DeleteConversationAsync();

            SPX_TRACE_INFO("Verifying callbacks");

            string participantId;
            eventHandlers.VerifyBasicEvents(true, hostname, true, out participantId);
            eventHandlers.VerifyConnectionEvents(1, 2);
            eventHandlers.VerifyTranscriptions(participantId,
                new ExpectedTranscription(participantId, audioFile.Utterances["en-US"][0].Text, speechLang)
            );
            eventHandlers.VerifyIms(participantId,
                new ExpectedTranscription(participantId, "This is a test", speechLang, new TRANS() { { "fr", "C'est un test" }, { "de", "Dies ist ein Test" } })
            );
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_CantCallMethodsAfterDisconnected()
        {
            var speechConfig = CreateConfig("en-US");

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(null);

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            host.Connection.Close();

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            REQUIRE_THROWS_MATCHES(
                host.Translator.StartTranscribingAsync(),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE"));

            REQUIRE_THROWS_MATCHES(
                host.Translator.StopTranscribingAsync(),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE"));

            REQUIRE_THROWS_MATCHES(
                host.Translator.SendTextMessageAsync("This is a short test"),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE"));

            await host.LeaveAsync();
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_ParticipantRejoin()
        {
            var hostUtterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH];
            var hostSpeechConfig = CreateConfig("en-US");
            var hostAudioConfig = AudioConfig.FromWavFileInput(hostUtterance.FilePath.GetRootRelativePath());

            var aliceUtterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CHINESE];
            var aliceAudioConfig = AudioConfig.FromWavFileInput(aliceUtterance.FilePath.GetRootRelativePath());

            var host = new TestConversationParticipant(hostSpeechConfig, "Host");
            await host.JoinAsync(hostAudioConfig);

            var alice = new TestConversationParticipant("Alice", "zh-CN", host, SetParticipantConfig);
            await alice.JoinAsync(aliceAudioConfig);

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            // Alice disconnects
            SPX_TRACE_INFO("Alice disconnecting");
            alice.Connection.Close();
            await Task.Delay(TimeSpan.FromMilliseconds(400));

            // Alice reconnects
            SPX_TRACE_INFO("Alice reconnecting");
            alice.Connection.Open(false);

            // Do audio playback for both host and participant
            await host.StartAudioAsync();
            await alice.StartAudioAsync();

            await host.WaitForAudioToFinish();
            await alice.WaitForAudioToFinish();

            await Task.Delay(TimeSpan.FromSeconds(2));

            // Disconnect both
            await alice.LeaveAsync();
            await host.LeaveAsync();

            // validate events
            SPX_TRACE_INFO("Validating host basic events");
            host.VerifyBasicEvents(true);
            SPX_TRACE_INFO("Validating Alice basic events");
            alice.VerifyBasicEvents(true);
            SPXTEST_REQUIRE(alice.Events.Connected.Count == 2);
            SPXTEST_REQUIRE(alice.Events.Disconnected.Count == 2);

            var expectedTranscriptions = new[]
            {
                new ExpectedTranscription(host.ParticipantId, hostUtterance.Utterances["en-US"][0].Text, host.Lang),
                new ExpectedTranscription(alice.ParticipantId, aliceUtterance.Utterances["zh-CN"][0].Text, alice.Lang)
            };

            SPX_TRACE_INFO("Validating host transcriptions");
            host.VerifyTranscriptions(expectedTranscriptions);
            SPX_TRACE_INFO("Validating Alice transcriptions");
            alice.VerifyTranscriptions(expectedTranscriptions);
        }

        [TestMethod]
        public async Task CT_ConversationTranslator_RejoinAfterDelete()
        {
            var hostUtterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH];
            var hostSpeechConfig = CreateConfig("en-US");
            var hostAudioConfig = AudioConfig.FromWavFileInput(hostUtterance.FilePath.GetRootRelativePath());

            var aliceUtterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_CHINESE];
            var aliceAudioConfig = AudioConfig.FromWavFileInput(aliceUtterance.FilePath.GetRootRelativePath());

            var host = new TestConversationParticipant(hostSpeechConfig, "Host");
            await host.JoinAsync(hostAudioConfig);

            var alice = new TestConversationParticipant("Alice", "zh-CN", host, SetParticipantConfig);
            await alice.JoinAsync(aliceAudioConfig);

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            // Alice disconnects. This prevents the conversation translator from detecting the conversation
            // has been deleted since we no longer have an active web socket connection
            SPX_TRACE_INFO("Alice disconnecting");
            alice.Connection.Close();
            await Task.Delay(TimeSpan.FromMilliseconds(400));

            // Delete the room
            await host.LeaveAsync();

            // Alice tries to reconnect
            SPX_TRACE_INFO("Alice reconnecting");
            REQUIRE_THROWS_MATCHES(
                () => alice.Connection.Open(false),
                typeof(ApplicationException),
                Catch.HasHR("BadRequest") & Catch.HasHR("WebSocket Upgrade failed"));

            await Task.Delay(TimeSpan.FromMilliseconds(200));

            // Make sure we got the correct cancelled event
            SPXTEST_REQUIRE(alice.Events.Canceled.Count > 0);
            var canceled = alice.Events.Canceled[0];
            SPXTEST_REQUIRE(canceled.Reason == CancellationReason.Error);
            SPXTEST_REQUIRE(canceled.ErrorCode == CancellationErrorCode.BadRequest);

            // Make sure we can't call open again
            REQUIRE_THROWS_MATCHES(
                () => alice.Connection.Open(false),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE")
            );

            REQUIRE_THROWS_MATCHES(
                () => alice.Connection.Open(true),
                typeof(ApplicationException),
                Catch.HasHR("SPXERR_INVALID_STATE")
            );
        }

        [TestMethod]
        [Ignore] // code is not yet live in PROD
        public async Task CT_ConversationTranslator_SetInvalidAuthorizationToken()
        {
            var speechConfig = await CreateAuthTokenConfigAsync(TimeSpan.FromMinutes(10), "en-US");

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(null);

            REQUIRE_THROWS_MATCHES(
                () => host.Translator.SetAuthorizationToken(""),
                typeof(ArgumentException),
                Catch.ExceptionMessageContains("authToken")
                    & Catch.ExceptionMessageContains("null")
                    & Catch.ExceptionMessageContains("empty")
            );

            REQUIRE_THROWS_MATCHES(
                () => host.Translator.SetAuthorizationToken("    "),
                typeof(ArgumentException),
                Catch.ExceptionMessageContains("authToken")
                    & Catch.ExceptionMessageContains("null")
                    & Catch.ExceptionMessageContains("empty")
            );

            await host.LeaveAsync();
        }

        [TestMethod]
        [Ignore] // code is not yet live in PROD
        public async Task CT_ConversationTranslator_HostUpdatesAuthorizationToken()
        {
            var authTokenValidity = TimeSpan.FromSeconds(15);
            string speechLang = "en-US";

            var speechConfig = await CreateAuthTokenConfigAsync(authTokenValidity, speechLang);

            var utterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH];
            var audioConfig = AudioConfig.FromWavFileInput(utterance.FilePath.GetRootRelativePath());

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(audioConfig);

            REQUIRE_THAT(host.Translator.AuthorizationToken, Catch.Equals(speechConfig.AuthorizationToken, Catch.CaseSensitive.Yes));
            REQUIRE(!string.IsNullOrWhiteSpace(host.Translator.ParticipantId));

            // wait for the current authentication token to expire, and then generate a new one
            await Task.Delay(authTokenValidity + TimeSpan.FromSeconds(5));

            var subsKey = GetSubscriptionKey();
            var region = GetRegion();
            var longAuthToken = await AzureSubscription.GenerateAuthorizationTokenAsync(
                subsKey, region, TimeSpan.FromSeconds(120));


            // update the authentication token and validate it was set properly
            host.Translator.SetAuthorizationToken(longAuthToken, region);
            await Task.Delay(TimeSpan.FromSeconds(5));

            REQUIRE_THAT(host.Translator.AuthorizationToken, Catch.Equals(longAuthToken, Catch.CaseSensitive.Yes));

            // now try to send audio and validate the transcriptions
            await host.StartAudioAsync();
            await host.WaitForAudioToFinish();
            await host.StopAudioAsync();

            await host.LeaveAsync();
            await Task.Delay(TimeSpan.FromSeconds(2));

            host.VerifyBasicEvents(true);
            host.VerifyTranscriptions(
                new ExpectedTranscription(host.ParticipantId, utterance.Utterances[speechLang][0].Text, speechLang)
            );
        }

        [TestMethod]
        [Ignore] // code is not yet live in PROD
        public async Task CT_ConversationTranslator_ParticipantReceivesUpdatedAuthToken()
        {
            var authTokenValidity = TimeSpan.FromSeconds(15);
            string speechLang = "en-US";

            var speechConfig = await CreateAuthTokenConfigAsync(authTokenValidity, speechLang);

            var utterance = AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH];
            var audioConfig = AudioConfig.FromWavFileInput(utterance.FilePath.GetRootRelativePath());

            var host = new TestConversationParticipant(speechConfig, "Host");
            await host.JoinAsync(audioConfig);

            var participant = new TestConversationParticipant("Participant", speechLang, host, SetParticipantConfig);
            await participant.JoinAsync(audioConfig);

            REQUIRE_THAT(participant.Translator.AuthorizationToken, Catch.Equals(speechConfig.AuthorizationToken, Catch.CaseSensitive.Yes));
            REQUIRE(!string.IsNullOrWhiteSpace(participant.Translator.ParticipantId));

            // wait for the current authentication token to expire, and then generate a new one
            await Task.Delay(authTokenValidity + TimeSpan.FromSeconds(5));

            var subsKey = GetSubscriptionKey();
            var region = GetRegion();
            var longAuthToken = await AzureSubscription.GenerateAuthorizationTokenAsync(
                subsKey, region, TimeSpan.FromSeconds(120));

            // update the authentication token and validate it was set properly
            host.Translator.SetAuthorizationToken(longAuthToken, region);
            await Task.Delay(TimeSpan.FromSeconds(5));

            REQUIRE_THAT(participant.Translator.AuthorizationToken, Catch.Equals(longAuthToken, Catch.CaseSensitive.Yes));

            // now try to send audio and validate the transcriptions
            await participant.StartAudioAsync();
            await participant.WaitForAudioToFinish();
            await participant.StopAudioAsync();

            await participant.LeaveAsync();
            await host.LeaveAsync();
            await Task.Delay(TimeSpan.FromSeconds(2));

            participant.VerifyBasicEvents(true);
            participant.VerifyTranscriptions(
                new ExpectedTranscription(participant.ParticipantId, utterance.Utterances[speechLang][0].Text, speechLang)
            );

            host.VerifyBasicEvents(false);
        }


        #region helper methods

            public SpeechTranslationConfig CreateConfig(string speechLang, params string[] translateTo)
        {
            string key = GetSubscriptionKey();
            string reg = GetRegion();

            SpeechTranslationConfig config;
            if (string.IsNullOrWhiteSpace(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]))
            {
                config = SpeechTranslationConfig.FromSubscription(key, reg);
            }
            else
            {
                config = SpeechTranslationConfig.FromEndpoint(new Uri(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]), key);
            }

            config.SpeechRecognitionLanguage = speechLang;
            for (int i = 0; translateTo != null && i < translateTo.Length; i++)
            {
                config.AddTargetLanguage(translateTo[i]);
            }

            config.SetProperty(PropertyId.Speech_SessionId, $"IntegrationTest:{Guid.NewGuid().ToString()}");
            config.SetSystemProxy();

            SetCommonConfig(config.SetProperty);

            return config;
        }

        public async Task<SpeechTranslationConfig> CreateAuthTokenConfigAsync(TimeSpan authTokenValidity, string lang, params string[] translateTo)
        {
            string subsKey = GetSubscriptionKey();
            string region = GetRegion();

            string authToken = await AzureSubscription.GenerateAuthorizationTokenAsync(
                subsKey, region, authTokenValidity, CancellationToken.None);

            SpeechTranslationConfig config;
            if (string.IsNullOrWhiteSpace(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]))
            {
                config = SpeechTranslationConfig.FromAuthorizationToken(authToken, region);
            }
            else
            {
                config = SpeechTranslationConfig.FromEndpoint(
                    new Uri(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]));
                config.AuthorizationToken = authToken;
            }

            config.SpeechRecognitionLanguage = lang;
            for (int i = 0; i < translateTo?.Length; i++)
            {
                config.AddTargetLanguage(translateTo[i]);
            }

            config.SetSystemProxy();
            SetCommonConfig(config.SetProperty);

            return config;
        }

        public void SetCommonConfig(Action<string, string> setter)
        {
            if (ManagementEndpoint != null)
            {
                setter("ConversationTranslator_RestEndpoint", ManagementEndpoint.ToString());
            }

            if (WebSocketEndpoint != null)
            {
                setter("ConversationTranslator_Endpoint", WebSocketEndpoint.ToString());
            }

            if (!string.IsNullOrWhiteSpace(SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Key))
            {
                setter("ConversationTranslator_SubscriptionKey", SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Key);
            }

            if (!string.IsNullOrWhiteSpace(SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Region))
            {
                setter("ConversationTranslator_Region", SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Region);
            }

            if (!string.IsNullOrWhiteSpace(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_CLIENTID]))
            {
                setter("ConversationTranslator_ClientId", DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_CLIENTID]);
            }
        }

        public void SetParticipantConfig(AudioConfig audioConfig)
        {
            if (!string.IsNullOrWhiteSpace(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]))
            {
                audioConfig.SetProperty(PropertyId.SpeechServiceConnection_Endpoint, DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]);
            }

            audioConfig.SetProperty(PropertyId.Speech_SessionId, $"IntegrationTest:{Guid.NewGuid().ToString()}");
            audioConfig.SetSystemProxy();

            SetCommonConfig(audioConfig.SetProperty);
        }

        private static string GetSubscriptionKey()
        {
            return new[]
            {
                SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Key,
                subscriptionKey
            }
            .FirstOrDefault(x => !string.IsNullOrWhiteSpace(x));
        }

        private static string GetRegion()
        {
            return new[]
            {
                SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Region,
                region
            }
            .FirstOrDefault(x => !string.IsNullOrWhiteSpace(x));
        }

        #endregion
    }
}
