//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.CognitiveServices.Speech.Transcription;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static CatchUtils;

    public static class ConversationTranslatorExtensionMethods
    {
        private const string FileLoggerClassName = "Microsoft.CognitiveServices.Speech.Internal.FileLogger";

        private static readonly Regex FILE_NAME_SANITIZER = new Regex(
            string.Format("[{0}]+", Regex.Escape(new string(System.IO.Path.GetInvalidFileNameChars()))),
            RegexOptions.Compiled);
        private static readonly JsonSerializerSettings SERIALIZER_SETTINGS = new JsonSerializerSettings()
        {
            Formatting = Formatting.Indented,
            Converters = new[]
            {
                new Newtonsoft.Json.Converters.StringEnumConverter()
            },
            DateFormatString = "o"
        };

        private static readonly Lazy<Func<string, bool, string, uint, TimeSpan?, IntPtr>> _startLogging =
            new Lazy<Func<string, bool, string, uint, TimeSpan?, IntPtr>>(() =>
                (Func<string, bool, string, uint, TimeSpan?, IntPtr>)GetStaticInternalMethod(
                    FileLoggerClassName,
                    "StartLogging",
                    typeof(Func<string, bool, string, uint, TimeSpan?, IntPtr>)));

        private static Lazy<Func<IntPtr>> _stopLogging =
            new Lazy<Func<IntPtr>>(() =>
                (Func<IntPtr>)GetStaticInternalMethod(
                    FileLoggerClassName,
                    "StopLogging",
                    typeof(Func<IntPtr>)));

        public static IntPtr StartLogging(this RecognitionTestBase _, string logFile) => _startLogging.Value(logFile, false, null, 0, null);
        public static IntPtr StopLogging(this RecognitionTestBase _) => _stopLogging.Value();

        public static bool TryGetDefaultSystemProxy(out Uri server, out string username, out string password)
        {
            server = null;
            username = null;
            password = null;

            try
            {
                Uri dummy = new Uri("https://not.a.real.uri.com");
                IWebProxy webProxy = WebRequest.GetSystemWebProxy();

                Uri uri = webProxy?.GetProxy(dummy);
                if (uri == null || uri == dummy)
                {
                    return false;
                }

                server = uri;

                ICredentials credentials = webProxy?.Credentials;
                var cred = credentials?.GetCredential(dummy, string.Empty);
                if (cred != null)
                {
                    username = cred.UserName;
                    password = cred.Password;
                }

                return true;
            }
            catch
            {
                server = null;
                username = null;
                password = null;
                return false;
            }
        }

        public static async Task ThrowsException<TException>(this Task task, string match = null)
        {
            try
            {
                await task.ConfigureAwait(false);
            }
            catch (Exception e)
            {
                if (e is TException)
                {
                    if (string.IsNullOrWhiteSpace(match) || e.Message.IndexOf(match, StringComparison.OrdinalIgnoreCase) >= 0)
                    {
                        return;
                    }

                    Assert.Fail($"Expected a {typeof(TException).FullName} with message containing '{match}'. Got: {e.GetType().Name} from '{e.Source}' {e.ToString()}");
                }

                Assert.Fail($"Expected a {typeof(TException).FullName}. Got a {e.GetType().FullName} from '{e.Source}' {e.ToString()}");
            }

            Assert.Fail($"Expected a {typeof(TException).FullName} exception. None was thrown");
        }

        public static TValue GetOrDefault<TKey, TValue>(this IDictionary<TKey, TValue> dict, TKey key)
        {
            TValue value;
            if (dict == null || !dict.TryGetValue(key, out value))
            {
                return default(TValue);
            }

            return value;
        }

        public static void Add<TKey, TValue>(this IDictionary<TKey, IList<TValue>> dict, TKey key, TValue value)
        {
            IList<TValue> list;
            if (!dict.TryGetValue(key, out list))
            {
                list = new List<TValue>();
                dict[key] = list;
            }

            list.Add(value);
        }

        public static string FileNameSanitize(this string s)
        {
            if (s == null)
            {
                return string.Empty;
            }

            return FILE_NAME_SANITIZER.Replace(s, "_");
        }

        public static T DumpToDebugOutput<T>(this T obj, string message = null, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0)
        {
            Debug.Write($"({DateTime.UtcNow.ToString("yyyy-MM-dd HH::mm::ss.ff")}) [{caller}:{line}] ");
            try
            {
                Debug.Indent();
                if (message != null)
                {
                    Debug.Write($"{message} ");
                }

                string json = JsonConvert.SerializeObject(obj, SERIALIZER_SETTINGS);
                json = System.Text.RegularExpressions.Regex.Replace(json, "^", "    ", System.Text.RegularExpressions.RegexOptions.Multiline);
                Debug.WriteLine(json);

                return obj;
            }
            finally
            {
                Debug.Unindent();
            }
        }

        /// <summary>
        /// Create a delegate to call static internal or static private methods that are normally inaccessible.
        /// This only works when the method names are not overloaded.
        /// </summary>
        /// <param name="className">The full name of the class that contains the method you want to call</param>
        /// <param name="methodName">The name of the method you want to call</param>
        /// <param name="delegateType">The type of the delegate to create (i.e. the method signature)</param>
        /// <returns>The delegate to invoke, or null if the method could not be found</returns>
        private static Delegate GetStaticInternalMethod(string className, string methodName, Type delegateType)
        {
            return typeof(SpeechConfig)
                .Assembly
                .GetType(className, false)
                ?.GetMethod(methodName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static)
                ?.CreateDelegate(delegateType);
        }
    }

    public class ExpectedTranscription
    {
        public ExpectedTranscription(string id, string text, string lang, int min = 0, IDictionary<string, string> translations = null)
        {
            this.ParticipantId = id;
            this.Text = text;
            this.Lang = lang;
            this.MinPartials = Math.Max(0, min);

            if (translations != null)
            {
                this.Translations = new Dictionary<string, string>(translations);
            }
            else
            {
                this.Translations = new Dictionary<string, string>();
            }
        }

        public string ParticipantId { get; }
        public string Text { get; }
        public string Lang { get; }
        public int MinPartials { get; }
        public IReadOnlyDictionary<string, string> Translations { get; }
    }

    public class ConversationTranslatorCallbacks
    {
        private TaskCompletionSource<bool> _audioStreamCompleted;

        public ConversationTranslatorCallbacks(ConversationTranslator convTrans)
        {
            _audioStreamCompleted = new TaskCompletionSource<bool>();

            SessionStarted = new List<SessionEventArgs>();
            SessionStopped = new List<SessionEventArgs>();
            Canceled = new List<ConversationTranslationCanceledEventArgs>();
            ParticipantsChanged = new List<ConversationParticipantsChangedEventArgs>();
            ConversationExpiration = new List<ConversationExpirationEventArgs>();
            Transcribing = new List<ConversationTranslationResult>();
            Transcribed = new List<ConversationTranslationResult>();
            TextMessageReceived = new List<ConversationTranslationResult>();

            convTrans.SessionStarted += (s, e) => SessionStarted.Add(e.DumpToDebugOutput("SessionStarted"));
            convTrans.SessionStopped += (s, e) => SessionStopped.Add(e.DumpToDebugOutput("SessionStopped"));
            convTrans.ParticipantsChanged += (s, e) => ParticipantsChanged.Add(e.DumpToDebugOutput("ParticipantsChanged"));
            convTrans.ConversationExpiration += (s, e) => ConversationExpiration.Add(e.DumpToDebugOutput("ConversationExpiration"));
            convTrans.Transcribing += (s, e) => Transcribing.Add(e.Result.DumpToDebugOutput("Transcribing"));
            convTrans.Transcribed += (s, e) => Transcribed.Add(e.Result.DumpToDebugOutput("Transcribed"));
            convTrans.TextMessageReceived += (s, e) => TextMessageReceived.Add(e.Result.DumpToDebugOutput("TextMessageReceived"));

            convTrans.Canceled += (s, e) =>
            {
                Canceled.Add(e.DumpToDebugOutput("Canceled"));

                try
                {
                    if (e.Reason == CancellationReason.EndOfStream)
                    {
                        _audioStreamCompleted.SetResult(true);
                    }
                    else
                    {
                        _audioStreamCompleted.SetException(new Exception($"cancel message indicating error: {e.ErrorCode.ToString()}, {e.ErrorDetails}"));
                    }
                }
                catch { }
            };
        }

        public IList<SessionEventArgs> SessionStarted { get; }
        public IList<SessionEventArgs> SessionStopped { get; }
        public IList<ConversationTranslationCanceledEventArgs> Canceled { get; }
        public IList<ConversationParticipantsChangedEventArgs> ParticipantsChanged { get; }
        public IList<ConversationExpirationEventArgs> ConversationExpiration { get; }
        public IList<ConversationTranslationResult> Transcribing { get; }
        public IList<ConversationTranslationResult> Transcribed { get; }
        public IList<ConversationTranslationResult> TextMessageReceived { get; }

        public Task WaitForAudioStreamCompletion(int maxWaitMs, int waitAfter = 0)
            => WaitForAudioStreamCompletion(TimeSpan.FromMilliseconds(maxWaitMs), TimeSpan.FromMilliseconds(waitAfter));
        public Task WaitForAudioStreamCompletion(TimeSpan maxWait, TimeSpan waitAfter = default(TimeSpan))
        {
            return Task.Run(() =>
            {
                try
                {
                    bool success = _audioStreamCompleted.Task.Wait(maxWait);
                    if (success)
                    {
                        Thread.Sleep(waitAfter);
                    }
                    else
                    {
                        throw new OperationCanceledException("Timed out waiting for audio to stop streaming");
                    }
                }
                catch (AggregateException ae)
                {
                    ae = ae.Flatten();
                    if (ae.InnerExceptions.Count == 1)
                    {
                        throw ae.InnerException;
                    }
                }
            });
        }

        public void VerifyBasicEvents(bool expectEndOfStream, string name, bool isHost, out string participantId)
        {
            SPXTEST_REQUIRE(SessionStarted.Count > 0);
            SPXTEST_REQUIRE(SessionStopped.Count > 0);

            if (expectEndOfStream)
            {
                SPXTEST_REQUIRE(Canceled.Count == 1);
                SPXTEST_REQUIRE(Canceled[0].Reason == CancellationReason.EndOfStream);
                SPXTEST_REQUIRE(Canceled[0].ErrorCode == CancellationErrorCode.NoError);
            }
            else
            {
                SPXTEST_REQUIRE(Canceled.Count == 0);
            }

            SPXTEST_REQUIRE(ParticipantsChanged.Count >= 2);
            SPXTEST_REQUIRE(ParticipantsChanged[0].Reason == Transcription.ParticipantChangedReason.JoinedConversation);
            SPXTEST_REQUIRE(ParticipantsChanged[1].Reason == Transcription.ParticipantChangedReason.Updated);

            // get the participant ID from the name changed event
            participantId = ParticipantsChanged[1].Participants.First(x => x.DisplayName == name).Id.ToUpperInvariant();

            if (isHost)
            {
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.Count == 1);
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.First().IsHost);
            }
            else
            {
                string id = participantId;
                SPXTEST_REQUIRE(ParticipantsChanged[0].Participants.Count >= 2);
                REQUIRE_NOTHROW(() => ParticipantsChanged[0].Participants.First(x => string.Equals(x.Id, id, StringComparison.OrdinalIgnoreCase)));
            }

            SPXTEST_REQUIRE(ParticipantsChanged[1].Participants.Count == 1);
            SPXTEST_REQUIRE(ParticipantsChanged[1].Participants.First().DisplayName == name);
        }

        public void VerifyTranscriptions(string participantId, params ExpectedTranscription[] expectedTranscriptions)
        {
            Dictionary<string, IList<ConversationTranslationResult>> partials = new Dictionary<string, IList<ConversationTranslationResult>>(StringComparer.OrdinalIgnoreCase);
            Dictionary<string, IList<ConversationTranslationResult>> finals = new Dictionary<string, IList<ConversationTranslationResult>>(StringComparer.OrdinalIgnoreCase);

            foreach (var evt in Transcribing)
            {
                partials.Add(evt.ParticipantId, evt);
            }

            foreach (var evt in Transcribed)
            {
                finals.Add(evt.ParticipantId, evt);
            }

            foreach (var e in expectedTranscriptions)
            {
                var expectedParticipantId = e.ParticipantId;
                var p = partials.GetOrDefault(expectedParticipantId) ?? new ConversationTranslationResult[0];
                var f = finals.GetOrDefault(expectedParticipantId) ?? new ConversationTranslationResult[0];

                var expectedPartialReason = expectedParticipantId == participantId
                    ? ResultReason.TranslatingSpeech
                    : ResultReason.TranslatingParticipantSpeech;
                var expectedFinalReason = expectedParticipantId == participantId
                    ? ResultReason.TranslatedSpeech
                    : ResultReason.TranslatedParticipantSpeech;

                // find an utterance that matches the expected string
                ConversationTranslationResult match = null;
                REQUIRE_NOTHROW(() => match = f.First(x => string.Equals(x.Text, e.Text, StringComparison.InvariantCultureIgnoreCase)));

                REQUIRE_THAT(match.Text, Catch.Equals(e.Text, Catch.CaseSensitive.No));
                REQUIRE(match.OriginalLang == e.Lang);
                REQUIRE_THAT(match.ParticipantId, Catch.Equals(e.ParticipantId, Catch.CaseSensitive.No));
                REQUIRE(match.Reason == expectedFinalReason);
                REQUIRE(match.Translations.Count >= e.Translations.Count);
                foreach (var expectedEntry in e.Translations)
                {
                    REQUIRE_THAT(match.Translations[expectedEntry.Key], Catch.Equals(expectedEntry.Value, Catch.CaseSensitive.No));
                }

                int numPartials = 0;
                for (int i = 0; i<p.Count; i++)
                {
                    if (p[i].ResultId == match.ResultId)
                    {
                        numPartials++;
                        REQUIRE(p[i].Reason == expectedPartialReason);
                    }
                }

                REQUIRE(numPartials >= e.MinPartials);
            }
        }

        public void VerifyIms(string participantId, params ExpectedTranscription[] expectedIms)
        {
            var ims = new Dictionary<string, IList<ConversationTranslationResult>>(StringComparer.OrdinalIgnoreCase);
            foreach (var evt in TextMessageReceived)
            {
                ims.Add(evt.ParticipantId, evt);
            }

            foreach (var expected in expectedIms)
            {
                var expectedParticipantId = expected.ParticipantId;
                var receivedFromUser = ims.GetOrDefault(expectedParticipantId) ?? new ConversationTranslationResult[0];

                var expectedReason = expectedParticipantId == participantId
                    ? ResultReason.TranslatedInstantMessage
                    : ResultReason.TranslatedParticipantInstantMessage;

                // find an IM that matches the expected string
                ConversationTranslationResult match = null;
                REQUIRE_NOTHROW(() => match = receivedFromUser.First(x => string.Equals(expected.Text, x.Text, StringComparison.InvariantCultureIgnoreCase)));

                REQUIRE_THAT(match.Text, Catch.Equals(expected.Text, Catch.CaseSensitive.No));
                REQUIRE(match.OriginalLang == expected.Lang);
                REQUIRE_THAT(match.ParticipantId, Catch.Equals(expected.ParticipantId, Catch.CaseSensitive.No));
                REQUIRE(match.Reason == expectedReason);
                REQUIRE(match.Translations.Count >= expected.Translations.Count);
                foreach (var expectedEntry in expected.Translations)
                {
                    REQUIRE_THAT(match.Translations[expectedEntry.Key], Catch.Equals(expectedEntry.Value, Catch.CaseSensitive.No));
                }
            }
        }
    }

    public class TestConversationParticipant : Internal.DisposableBase
    {
        private SpeechConfig m_config;
        private Action<ConversationTranslator, bool> m_setConfig;

        public TestConversationParticipant(SpeechConfig config, string nickname)
        {
            m_config = config;
            Name = nickname;
            Lang = config.SpeechRecognitionLanguage;
            IsHost = true;
        }

        public TestConversationParticipant(string nickname, string lang, TestConversationParticipant other, Action<ConversationTranslator, bool> setConfig)
        {
            ConversationId = other.Conversation.ConversationId;
            IsHost = false;
            Name = nickname;
            Lang = lang;
            m_setConfig = setConfig;
        }

        public string Name { get; }
        public string Lang { get; }
        public bool IsHost { get; }
        public string ConversationId { get; private set; }
        public string ParticipantId { get; private set; }
        public Conversation Conversation { get; private set; }
        public ConversationTranslator Translator { get; private set; }
        public ConversationTranslatorCallbacks Events { get; private set; }

        public async Task JoinAsync(AudioConfig audioConfig)
        {
            if (audioConfig == null)
            {
                // create one that points to an audio file to avoid mic errors in the CI tests
                audioConfig = AudioConfig.FromWavFileInput(Config.AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
            }

            if (IsHost)
            {
                SPX_TRACE_INFO(">> [{0}] Creating conversation", Name);
                Conversation = await Conversation.CreateConversationAsync(m_config);
                ConversationId = Conversation.ConversationId;
                SPX_TRACE_INFO(">> [{0}] Joining conversation '{1}'", Name, ConversationId);
                SPXTEST_REQUIRE(!string.IsNullOrWhiteSpace(ConversationId));

                SPX_TRACE_INFO(">> [{0}] Starting conversation", Name);
                await Conversation.StartConversationAsync();

                SPX_TRACE_INFO(">> [{0}] Creating conversation translator", Name);
                Translator = audioConfig == null
                    ? new ConversationTranslator()
                    : new ConversationTranslator(audioConfig);
                Events = new ConversationTranslatorCallbacks(Translator);

                SPX_TRACE_INFO(">> [{0}] Joining conversation", Name);
                await Translator.JoinConversationAsync(Conversation, Name);
            }
            else
            {
                SPX_TRACE_INFO(">> [{0}] Creating conversation translator", Name);

                Translator = audioConfig == null
                    ? new ConversationTranslator()
                    : new ConversationTranslator(audioConfig);
                Events = new ConversationTranslatorCallbacks(Translator);

                SPX_TRACE_INFO(">> [{0}] Joining conversation '{1}'", Name, ConversationId);
                await Translator.JoinConversationAsync(ConversationId, Name, Lang);
                m_setConfig(Translator, true);
            }
        }

        public async Task StartAudioAsync()
        {
            SPX_TRACE_INFO(">> [{0}] Starting audio", Name);
            await Translator.StartTranscribingAsync();
        }

        public async Task WaitForAudioToFinish(TimeSpan? maxWait = null)
        {
            if (maxWait == null)
            {
                maxWait = TimeSpan.FromSeconds(15);
            }

            SPX_TRACE_INFO(">> [{0}] Waiting up to {1} for audio to complete", Name, maxWait);
            await Events.WaitForAudioStreamCompletion(maxWait.Value, TimeSpan.FromSeconds(2));
            SPX_TRACE_INFO(">> [{0}] Audio has completed", Name);
        }

        public async Task StopAudioAsync()
        {
            SPX_TRACE_INFO(">> [{0}] Stopping audio", Name);
            await Translator.StopTranscribingAsync();
        }

        public async Task LeaveAsync()
        {
            if (Translator != null)
            {
                SPX_TRACE_INFO(">> [{0}] Leaving conversation", Name);
                await Translator.LeaveConversationAsync();
            }

            if (Conversation != null)
            {
                SPX_TRACE_INFO(">> [{0}] Ending conversation", Name);
                await Conversation.EndConversationAsync();
                SPX_TRACE_INFO(">> [{0}] Deleting conversation", Name);
                await Conversation.DeleteConversationAsync();

                SPX_TRACE_INFO(">> [{0}] DONE!", Name);
            }
        }

        public void VerifyBasicEvents(bool expectEndOfStream)
        {
            string partId;
            Events.VerifyBasicEvents(expectEndOfStream, Name, IsHost, out partId);
            ParticipantId = partId;
        }

        public void VerifyTranscriptions(params ExpectedTranscription[] expectedTranscriptions)
        {
            Events.VerifyTranscriptions(ParticipantId, expectedTranscriptions);
        }

        public void VerifyIms(params ExpectedTranscription[] expectedIms)
        {
            Events.VerifyIms(ParticipantId, expectedIms);
        }

        protected override void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                Conversation?.Dispose();
                Conversation = null;
                Translator?.Dispose();
                Translator = null;
            }
        }
    };


    
}
