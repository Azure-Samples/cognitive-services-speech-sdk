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
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static CatchUtils;
    using static Config;
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
        public async Task Conversation_WithoutTranslations()
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
        public async Task Conversation_WithTranslations()
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
        public async Task Conversation_Dispose()
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
        public async Task Conversation_DisposeAfterStart()
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
        public async Task Conversation_MethodsWhileNotStarted()
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
        public async Task Conversation_CallUnsupportedMethods()
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
        public async Task ConversationTranslator_HostAudio()
        {
            string speechLang = Language.EN;
            string hostname = "TheHost";
            var toLangs = new[] { Language.FR, Language.DE };
            var speechConfig = CreateConfig(speechLang, toLangs);

            SPX_TRACE_INFO("Creating conversation");
            var conversation = await Conversation.CreateConversationAsync(speechConfig);
            SPX_TRACE_INFO($"Starting {conversation.ConversationId} conversation");
            await conversation.StartConversationAsync();

            var audioConfig = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            SPX_TRACE_INFO("Creating ConversationTranslator");
            var conversationTranslator = new ConversationTranslator(audioConfig);

            var eventHandlers = new ConversationTranslatorCallbacks(conversationTranslator);

            SPX_TRACE_INFO("Joining conversation");
            await conversationTranslator.JoinConversationAsync(conversation, hostname);
            SPX_TRACE_INFO("Start transcribing");
            await conversationTranslator.StartTranscribingAsync();

            await eventHandlers.WaitForAudioStreamCompletion(15000, 2000);


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
            eventHandlers.VerifyBasicEvents(true, true, hostname, true, out participantId);
            eventHandlers.VerifyTranscriptions(participantId,
                new ExpectedTranscription(participantId, AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].Utterances[Language.EN][0].Text, speechLang)
            );
        }

        [TestMethod]
        public async Task ConversationTranslator_JoinWithTranslation()
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
            SPX_TRACE_INFO("Creating bob ConversationTranslator");
            var bobTranslator = new ConversationTranslator(bobAudioConfig);
            var bobEvents = new ConversationTranslatorCallbacks(bobTranslator);
            SPX_TRACE_INFO("Bob joining {0}", conversation.ConversationId);
            await bobTranslator.JoinConversationAsync(conversation.ConversationId, bobName, bobLang);
            SetParticipantConfig(bobTranslator, false);


            // do audio playback
            SPX_TRACE_INFO("Starting host audio");
            await hostTranslator.StartTranscribingAsync();
            SPX_TRACE_INFO("Starting bob audio");
            await bobTranslator.StartTranscribingAsync();

            SPX_TRACE_INFO("Waiting for host audio to complete");
            await hostEvents.WaitForAudioStreamCompletion(15000);
            SPX_TRACE_INFO("Waiting for bob host audio to complete");
            await bobEvents.WaitForAudioStreamCompletion(15000, 2000);

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
            bobEvents.VerifyBasicEvents(true, false, bobName, false, out bobId);
            hostEvents.VerifyBasicEvents(true, true, hostName, true, out hostId);
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
        public async Task ConversationTranslator_HostSendsIm()
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
        public async Task ConversationTranslator_HostAndParticipantSendIms()
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

            await Task.Delay(TimeSpan.FromSeconds(2));

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
        public async Task ConversationTranslator_JoinLockedRoom()
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
        public async Task ConversationTranslator_CallMethodsWhenNotJoined()
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
                    HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.StopTranscribingAsync(),
                    typeof(ApplicationException),
                    HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.SendTextMessageAsync("This is a test"),
                    typeof(ApplicationException),
                    HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_NOTHROW(translator.LeaveConversationAsync());
            }

            SPX_TRACE_INFO("========== Participant ==========");
            {
                var translator = new ConversationTranslator();

                REQUIRE_THROWS_MATCHES(
                    translator.StartTranscribingAsync(),
                    typeof(ApplicationException),
                    HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.StopTranscribingAsync(),
                    typeof(ApplicationException),
                    HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_THROWS_MATCHES(
                    translator.SendTextMessageAsync("This is a test"),
                    typeof(ApplicationException),
                    HasHR("SPXERR_UNINITIALIZED"));

                REQUIRE_NOTHROW(translator.LeaveConversationAsync());
            }
        }

        #region helper methods

        public SpeechTranslationConfig CreateConfig(string speechLang, params string[] translateTo)
        {
            string key = new[] { SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Key, subscriptionKey }.FirstOrDefault(x => !string.IsNullOrWhiteSpace(x));
            string reg = new[] { SubscriptionsRegionsMap[SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR].Region, region }.FirstOrDefault(x => !string.IsNullOrWhiteSpace(x));

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

            SetCommonConfig(
                (id, val) => config.SetProperty(id, val),
                (h, p, u, w) => config.SetProxy(h, p, u, w));

            return config;
        }

        public void SetCommonConfig(Action<string, string> setter, Action<string, int, string, string> setProxy)
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

            Uri proxy;
            string proxyName;
            string proxyPass;
            if (ConversationTranslatorExtensionMethods.TryGetDefaultSystemProxy(out proxy, out proxyName, out proxyPass))
            {
                setProxy(proxy.IdnHost, proxy.Port, proxyName, proxyPass);
            }
        }

        public void SetParticipantConfig(ConversationTranslator convTrans, bool setEndpoint)
        {
            if (setEndpoint && !string.IsNullOrWhiteSpace(DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]))
            {
                convTrans.Properties.SetProperty(PropertyId.SpeechServiceConnection_Endpoint, DefaultSettingsMap[DefaultSettingKeys.CONVERSATION_TRANSLATOR_SPEECH_ENDPOINT]);
            }

            convTrans.Properties.SetProperty(PropertyId.Speech_SessionId, $"IntegrationTest:{Guid.NewGuid().ToString()}");

            SetCommonConfig(
                (id, val) => convTrans.Properties.SetProperty(id, val),
                (host, port, username, pwd) =>
                {
                    convTrans.Properties.SetProperty(PropertyId.SpeechServiceConnection_ProxyHostName, host);
                    convTrans.Properties.SetProperty(PropertyId.SpeechServiceConnection_ProxyPort, port.ToString(CultureInfo.InvariantCulture));
                    if (!string.IsNullOrWhiteSpace(username))
                    {
                        convTrans.Properties.SetProperty(PropertyId.SpeechServiceConnection_ProxyUserName, username);
                        if (!string.IsNullOrWhiteSpace(pwd))
                        {
                            convTrans.Properties.SetProperty(PropertyId.SpeechServiceConnection_ProxyPassword, pwd);
                        }
                    }
                });
        }

        #endregion
    }
}
