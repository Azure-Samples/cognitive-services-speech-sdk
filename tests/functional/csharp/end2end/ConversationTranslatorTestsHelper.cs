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
using System.Linq;
using System.Net;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using System.Globalization;
    using static CatchUtils;
    using static ConversationTranslatorTestConstants;

    public static class ConversationTranslatorTestConstants
    {
        public static TimeSpan MAX_WAIT_FOR_AUDIO_TO_COMPLETE => TimeSpan.FromSeconds(20);
        public static TimeSpan WAIT_AFTER_AUDIO_COMPLETE => TimeSpan.FromSeconds(2);
    }

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

        private static readonly Lazy<Func<string, IntPtr>> _startLogging =
            new Lazy<Func<string, IntPtr>>(() =>
                (Func<string, IntPtr>)GetStaticInternalMethod(
                    FileLoggerClassName,
                    "StartLogging",
                    typeof(Func<string, IntPtr>)));

        private static Lazy<Func<IntPtr>> _stopLogging =
            new Lazy<Func<IntPtr>>(() =>
                (Func<IntPtr>)GetStaticInternalMethod(
                    FileLoggerClassName,
                    "StopLogging",
                    typeof(Func<IntPtr>)));

        private static Lazy<Func<IntPtr>> _resetLogging =
            new Lazy<Func<IntPtr>>(() =>
                (Func<IntPtr>)GetStaticInternalMethod(
                    FileLoggerClassName,
                    "ResetLogging",
                    typeof(Func<IntPtr>)));

        public static IntPtr StartLogging(this RecognitionTestBase _, string logFile) => _startLogging.Value(logFile);
        public static IntPtr StopLogging(this RecognitionTestBase _) => _stopLogging.Value();
        public static IntPtr ResetLogging() => _resetLogging.Value();

        private static bool TryGetDefaultSystemProxy(out Uri server, out string username, out string password)
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

        /// <summary>
        /// Sets the current system proxy
        /// </summary>
        /// <param name="config">The speech config to set the proxy on</param>
        public static SpeechConfig SetSystemProxy(this SpeechConfig config)
        {
            Uri proxy;
            string username;
            string password;
            if (config != null && TryGetDefaultSystemProxy(out proxy, out username, out password))
            {
                config.SetProxy(proxy.IdnHost, proxy.Port, username, password);
            }

            return config;
        }

        /// <summary>
        /// Sets the current system proxy
        /// </summary>
        /// <param name="config">The audio config to set the proxy on</param>
        public static AudioConfig SetSystemProxy(this AudioConfig config)
        {
            Uri proxy;
            string username;
            string password;
            if (config != null && TryGetDefaultSystemProxy(out proxy, out username, out password))
            {
                config.SetProperty(PropertyId.SpeechServiceConnection_ProxyHostName, proxy.IdnHost);
                config.SetProperty(PropertyId.SpeechServiceConnection_ProxyPort, proxy.Port.ToString(CultureInfo.InvariantCulture));
                if (!string.IsNullOrWhiteSpace(username))
                {
                    config.SetProperty(PropertyId.SpeechServiceConnection_ProxyUserName, username);
                    if (!string.IsNullOrWhiteSpace(password))
                    {
                        config.SetProperty(PropertyId.SpeechServiceConnection_ProxyPassword, password);
                    }
                }
            }

            return config;
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

        public static T Dump<T>(this T obj, string message = null, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0)
        {
            Console.Write($"({DateTime.UtcNow.ToString("yyyy-MM-dd HH::mm::ss.ff")}) [{caller}:{line}] ");

            if (message != null)
            {
                Console.Write($"{message} ");
            }

            string json = JsonConvert.SerializeObject(obj, SERIALIZER_SETTINGS);
            json = Regex.Replace(json, "^", "    ", RegexOptions.Multiline);
            Console.WriteLine(json);

            return obj;
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
        public ExpectedTranscription(string id, string text, string lang, IDictionary<string, string> translations)
            : this(id, text, lang, 0, translations)
        {
        }

        public ExpectedTranscription(string id, string text, string lang, int min = 0, IDictionary<string, string> translations = null)
        {
            this.ParticipantId = id;
            this.Text = text;
            this.Lang = TranslatorTextLanguage.Parse(lang);
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
        public TranslatorTextLanguage Lang { get; }
        public int MinPartials { get; }
        public IReadOnlyDictionary<string, string> Translations { get; }
    }

    public class ConversationTranslatorCallbacks
    {
        private TaskCompletionSource<bool> _audioStreamCompleted;
        private bool _hasConnection;

        public ConversationTranslatorCallbacks(ConversationTranslator convTrans)
        {
            _audioStreamCompleted = new TaskCompletionSource<bool>();

            convTrans.SessionStarted += (s, e) => SessionStarted.Add(e.Dump("SessionStarted"));
            convTrans.SessionStopped += (s, e) => SessionStopped.Add(e.Dump("SessionStopped"));
            convTrans.ParticipantsChanged += (s, e) => ParticipantsChanged.Add(e.Dump("ParticipantsChanged"));
            convTrans.ConversationExpiration += (s, e) => ConversationExpiration.Add(e.Dump("ConversationExpiration"));
            convTrans.Transcribing += (s, e) => Transcribing.Add(e.Result.Dump("Transcribing"));
            convTrans.Transcribed += (s, e) => Transcribed.Add(e.Result.Dump("Transcribed"));
            convTrans.TextMessageReceived += (s, e) => TextMessageReceived.Add(e.Result.Dump("TextMessageReceived"));

            convTrans.Canceled += (s, e) =>
            {
                Canceled.Add(e.Dump("Canceled"));

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

        public IList<SessionEventArgs> SessionStarted { get; } = new List<SessionEventArgs>();
        public IList<SessionEventArgs> SessionStopped { get; } = new List<SessionEventArgs>();
        public IList<ConnectionEventArgs> Connected { get; } = new List<ConnectionEventArgs>();
        public IList<ConnectionEventArgs> Disconnected { get; } = new List<ConnectionEventArgs>();
        public IList<ConversationTranslationCanceledEventArgs> Canceled { get; } = new List<ConversationTranslationCanceledEventArgs>();
        public IList<ConversationParticipantsChangedEventArgs> ParticipantsChanged { get; } = new List<ConversationParticipantsChangedEventArgs>();
        public IList<ConversationExpirationEventArgs> ConversationExpiration { get; } = new List<ConversationExpirationEventArgs>();
        public IList<ConversationTranslationResult> Transcribing { get; } = new List<ConversationTranslationResult>();
        public IList<ConversationTranslationResult> Transcribed { get; } = new List<ConversationTranslationResult>();
        public IList<ConversationTranslationResult> TextMessageReceived { get; } = new List<ConversationTranslationResult>();

        public void AddConnectionCallbacks(Connection connection)
        {
            connection.Connected += (s, e) => Connected.Add(e.Dump("Connected"));
            connection.Disconnected += (s, e) => Disconnected.Add(e.Dump("Disconnected"));

            _hasConnection = true;
        }

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

        public void VerifySessionAndConnectEvents(bool expectEndOfStream)
        {
            SPXTEST_REQUIRE(SessionStarted.Count > 0);
            SPXTEST_REQUIRE(SessionStopped.Count > 0);

            if (_hasConnection)
            {
                SPXTEST_REQUIRE(Connected.Count > 0);
                SPXTEST_REQUIRE(Disconnected.Count > 0);
            }

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
        }

        public void VerifyBasicEvents(bool expectEndOfStream, string name, bool isHost, out string participantId)
        {
            VerifySessionAndConnectEvents(expectEndOfStream);

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

        public void VerifyConnectionEvents(int expectedConnections, int expectedDisconnections)
        {
            SPXTEST_REQUIRE(Connected.Count == expectedConnections);
            SPXTEST_REQUIRE(Disconnected.Count == expectedDisconnections);
        }

        private static bool TryGetValueLower(IReadOnlyDictionary<string, string> dict, string key, out string val)
        {
            return dict.TryGetValue(key, out val) || dict.TryGetValue(key.ToLower(), out val);
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
                ConversationTranslationResult match = f.FirstOrDefault(x =>
                    string.Equals(x.Text, e.Text, StringComparison.InvariantCultureIgnoreCase)
                    || SpeechRecognitionTestsHelper.IsWithinStringWordEditPercentage(e.Text, x.Text));
                if (match == null)
                {
                    FAIL($"Could not final that matches '{e.Text}'\nFinals found:\n"
                        + string.Join("\n", f.Select(t => $"  [{t.ParticipantId}] '{t.Text}'")));
                }

                REQUIRE(TranslatorTextLanguage.Parse(match.OriginalLang).Equals(e.Lang));
                REQUIRE_THAT(match.ParticipantId, Catch.Equals(e.ParticipantId, Catch.CaseSensitive.No));
                REQUIRE(match.Reason == expectedFinalReason);
                REQUIRE(match.Translations.Count >= e.Translations.Count);
                foreach (var expectedEntry in e.Translations)
                {
                    TranslatorTextLanguage transLang = TranslatorTextLanguage.Parse(expectedEntry.Key);
                    string actualTranslation;

                    // First lookup the full language code (e.g. zh-Hans-CH), then just the language code
                    // and script (zh-Hans), and finally just the language code (e.g. zh)
                    if (!TryGetValueLower(match.Translations, transLang.ToString(), out actualTranslation)
                        && !TryGetValueLower(match.Translations, $"{transLang.Code}-{transLang.Script}", out actualTranslation)
                        && !TryGetValueLower(match.Translations, transLang.Code, out actualTranslation))
                    {
                        actualTranslation = "<NO MATCH FOUND FOR LANGUAGE CODE>";
                    }

                    REQUIRE_THAT(actualTranslation, Catch.FuzzyMatch(expectedEntry.Value));
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
                ConversationTranslationResult match = receivedFromUser.FirstOrDefault(x =>
                    string.Equals(expected.Text, x.Text, StringComparison.InvariantCultureIgnoreCase)
                    || SpeechRecognitionTestsHelper.IsWithinStringWordEditPercentage(expected.Text, x.Text));
                if (match == null)
                {
                    FAIL($"Could not instant message that matches '{expected.Text}'\nFinals found:\n"
                        + string.Join("\n", receivedFromUser.Select(im => $"  [{im.ParticipantId}] '{im.Text}'")));
                }

                REQUIRE(TranslatorTextLanguage.Parse(match.OriginalLang) == expected.Lang);
                REQUIRE_THAT(match.ParticipantId, Catch.Equals(expected.ParticipantId, Catch.CaseSensitive.No));
                REQUIRE(match.Reason == expectedReason);
                REQUIRE(match.Translations.Count >= expected.Translations.Count);
                foreach (var expectedEntry in expected.Translations)
                {
                    TranslatorTextLanguage transLang = TranslatorTextLanguage.Parse(expectedEntry.Key);
                    string actualTranslation;

                    // First lookup the full language code (e.g. zh-Hans-CH), then just the language code
                    // and script (zh-Hans), and finally just the language code (e.g. zh)
                    if (!TryGetValueLower(match.Translations, transLang.ToString(), out actualTranslation)
                        && !TryGetValueLower(match.Translations, $"{transLang.Code}-{transLang.Script}", out actualTranslation)
                        && !TryGetValueLower(match.Translations, transLang.Code, out actualTranslation))
                    {
                        actualTranslation = "<NO MATCH FOUND FOR LANGUAGE CODE>";
                    }

                    REQUIRE_THAT(actualTranslation, Catch.FuzzyMatch(expectedEntry.Value));
                }
            }
        }
    }

    public class TestConversationParticipant : Internal.DisposableBase
    {
        private SpeechConfig m_config;
        private Action<AudioConfig> m_setConfig;

        public TestConversationParticipant(SpeechConfig config, string nickname)
        {
            m_config = config;
            Name = nickname;
            Lang = config.SpeechRecognitionLanguage;
            IsHost = true;
        }

        public TestConversationParticipant(string nickname, string lang, TestConversationParticipant other, Action<AudioConfig> setConfig)
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
        public Connection Connection { get; private set; }

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
                Translator = new ConversationTranslator(audioConfig);
                Events = new ConversationTranslatorCallbacks(Translator);

                SPX_TRACE_INFO(">> [{0}] Creating connection", Name);
                Connection = Connection.FromConversationTranslator(Translator);
                Events.AddConnectionCallbacks(Connection);

                SPX_TRACE_INFO(">> [{0}] Joining conversation", Name);
                await Translator.JoinConversationAsync(Conversation, Name);
            }
            else
            {
                SPX_TRACE_INFO(">> [{0}] Creating conversation translator", Name);

                // Apply configuration before connecting
                m_setConfig(audioConfig);

                Translator = new ConversationTranslator(audioConfig);
                Events = new ConversationTranslatorCallbacks(Translator);

                SPX_TRACE_INFO(">> [{0}] Creating connection", Name);
                Connection = Connection.FromConversationTranslator(Translator);
                Events.AddConnectionCallbacks(Connection);

                SPX_TRACE_INFO(">> [{0}] Joining conversation '{1}'", Name, ConversationId);
                await Translator.JoinConversationAsync(ConversationId, Name, Lang);
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
                maxWait = MAX_WAIT_FOR_AUDIO_TO_COMPLETE;
            }

            SPX_TRACE_INFO(">> [{0}] Waiting up to {1} for audio to complete", Name, maxWait);
            await Events.WaitForAudioStreamCompletion(maxWait.Value, WAIT_AFTER_AUDIO_COMPLETE);
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

    public abstract class TranslatorLanguageBase : IEquatable<TranslatorLanguageBase>, IComparable<TranslatorLanguageBase>
    {
        protected TranslatorLanguageBase(string code, string script = null, string region = null)
        {
            if (string.IsNullOrWhiteSpace(code))
            {
                throw new ArgumentException(nameof(code) + " cannot be null, empty or consist only of white space");
            }

            this.Code = code;
            this.Script = string.IsNullOrWhiteSpace(script) ? string.Empty : script?.Trim();
            this.Region = string.IsNullOrWhiteSpace(region) ? string.Empty : region?.Trim();
        }

        public string Code { get; }

        public string Script { get; set; }

        public string Region { get; set; }

        public override bool Equals(object obj) => this.Equals(obj as TranslatorLanguageBase);

        public abstract bool Equals(TranslatorLanguageBase lang);

        public static bool operator ==(TranslatorLanguageBase left, TranslatorLanguageBase right)
        {
            if (object.ReferenceEquals(left, null))
            {
                return object.ReferenceEquals(right, null);
            }

            return left.Equals(right);
        }

        public static bool operator !=(TranslatorLanguageBase left, TranslatorLanguageBase right)
        {
            return !(left == right);
        }

        public override int GetHashCode()
        {
            // a bit of hack. In .net standard we could just do HashCode.Combine(...)
            var tuple = new Tuple<string, string, string>(Code, Script ?? string.Empty, Region ?? string.Empty);
            return tuple.GetHashCode();
        }

        public abstract int CompareTo(TranslatorLanguageBase other);

        public override string ToString()
        {
            var builder = new StringBuilder((Code?.Length + Script?.Length + Region?.Length) ?? 0 + 2);
            builder.Append(this.Code);

            if (!string.IsNullOrEmpty(Script))
            {
                builder.Append("-");
                builder.Append(Script);
            }

            if (!string.IsNullOrEmpty(Region))
            {
                builder.Append("-");
                builder.Append(Region);
            }

            return builder.ToString();
        }

        internal static bool TryParseRawLanguageCode(string rawLangCode, out string lang, out string script, out string region)
        {
            lang = null;
            script = null;
            region = null;

            if (string.IsNullOrWhiteSpace(rawLangCode))
            {
                return false;
            }

            string[] parts = rawLangCode.Split('-');

            // the first part should always be the language
            lang = parts[0].Trim().ToLowerInvariant();
            if (string.IsNullOrWhiteSpace(lang))
            {
                lang = null;
                return false;
            }

            // now extract the region codes and scripts. Please note that this uses
            // VERY simplified parsing that may not be accurate in all cases but is
            // sufficient for our needs
            for (var i = 1; i < parts.Length && script == null && region == null; i++)
            {
                if (parts[i].Length == 2)
                {
                    // this is the region code
                    region = parts[i].ToUpperInvariant();

                    if (lang == "zh" && region == "CN")
                    {
                        script = "Hans";
                    }
                    else if (lang == "zh" && region == "TW")
                    {
                        script = "Hant";
                    }
                }
                else if (parts[i].Length == 3 && region == null)
                {
                    // special case for us to handle zh-CHS and zh-CHT which are non-standard.
                    // Script should always come before the region code
                    if (parts[i].ToUpperInvariant() == "CHS")
                    {
                        script = "Hans";
                    }
                    else if (parts[i].ToUpperInvariant() == "CHT")
                    {
                        script = "Hant";
                    }
                }
                else if (parts[i].Length == 4 && region == null)
                {
                    // We found the script. This should always come before the region code
                    script = char.ToUpperInvariant(parts[i][0]) + parts[i].Substring(1).ToLowerInvariant();
                }
            }

            // Text translation is using the V2 APIs
            // Speech translation is using the V4 APIs
            // Different language codes are returned for Norwegian and Bosnian from each API
            if (lang == "no")
            {
                lang = "nb";
            }
            else if (lang == "bs" && script == "Latn")
            {
                lang = "bs";
                script = string.Empty;
            }

            return true;
        }

        protected int CompareTo(TranslatorLanguageBase other, string comparisons)
        {
            if (other == null)
            {
                return -1;
            }

            int comp = string.CompareOrdinal(this.Code, other.Code);
            if (comp != 0)
            {
                return comp;
            }

            foreach (var c in comparisons)
            {
                switch (c)
                {
                    case 's':
                    case 'S':
                        comp = string.CompareOrdinal(this.Script, other.Script);
                        break;

                    case 'r':
                    case 'R':
                        comp = string.CompareOrdinal(this.Region, other.Region);
                        break;

                    default:
                        throw new ArgumentException(nameof(comparisons));
                }

                if (comp != 0)
                {
                    return comp;
                }
            }

            return comp;
        }
    }

    public class TranslatorTextLanguage : TranslatorLanguageBase
    {
        public static bool TryParse(string rawLangCode, out TranslatorTextLanguage lang)
        {
            string code, script, region;

            if (TryParseRawLanguageCode(rawLangCode, out code, out script, out region))
            {
                lang = new TranslatorTextLanguage(code, script, region);
                return true;
            }

            lang = null;
            return false;
        }

        public static TranslatorTextLanguage Parse(string rawLangCode)
        {
            TranslatorTextLanguage lang;
            if (TryParse(rawLangCode, out lang))
            {
                return lang;
            }

            throw new ArgumentException("Invalid language code");
        }

        private TranslatorTextLanguage(string code, string script, string region)
            : base(code, script, region)
        {
        }

        public override bool Equals(TranslatorLanguageBase other)
        {
            return other != null
                && this.Code == other.Code
                && this.Script == other.Script;
        }

        public override int CompareTo(TranslatorLanguageBase other) => this.CompareTo(other, "S");
    }

    public class TranslatorSpeechLanguage : TranslatorLanguageBase
    {
        public static bool TryParse(string rawLangCode, out TranslatorSpeechLanguage lang)
        {
            string code, script, region;

            if (TryParseRawLanguageCode(rawLangCode, out code, out script, out region))
            {
                lang = new TranslatorSpeechLanguage(code, script, region);
                return true;
            }

            lang = null;
            return false;
        }

        public static TranslatorSpeechLanguage Parse(string rawLangCode)
        {
            TranslatorSpeechLanguage lang;
            if (TryParse(rawLangCode, out lang))
            {
                return lang;
            }

            throw new ArgumentException("Invalid language code");
        }

        private TranslatorSpeechLanguage(string code, string script, string region)
            : base(code, script, region)
        {
        }

        public override bool Equals(TranslatorLanguageBase other)
        {
            return other != null
                && this.Code == other.Code
                && this.Region == other.Region;
        }

        public override int CompareTo(TranslatorLanguageBase other) => this.CompareTo(other, "R");
    }
}
