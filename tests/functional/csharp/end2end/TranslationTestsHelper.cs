//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class TranslationTestsHelper
    {
        private string subscriptionKey;
        private string region;
        private TimeSpan timeout;

        public TranslationTestsHelper(string subscriptionKey, string region)
        {
            this.subscriptionKey = subscriptionKey;
            this.region = region;
            timeout = TimeSpan.FromMinutes(6);
        }

        public SpeechTranslationConfig GetConfig(string fromLanguage, List<string> toLanguages, string voice, string endpointId = null)
        {
            var config = SpeechTranslationConfig.FromSubscription(this.subscriptionKey, this.region);
            config.SpeechRecognitionLanguage = fromLanguage;
            toLanguages.ForEach(l => config.AddTargetLanguage(l));

            if (!string.IsNullOrEmpty(voice))
            {
                config.VoiceName = voice;
            }

            if (!string.IsNullOrEmpty(endpointId))
            {
                config.EndpointId = endpointId;
            }

            // Set proxy if one is defined
            config.SetSystemProxy();

            return config;
        }

        public TranslationRecognizer CreateTranslationRecognizer(string path, string fromLanguage, List<string> toLanguages, string voice = null, string endpointId = null)
        {
            var audioInput = AudioConfig.FromWavFileInput(path);
            return new TranslationRecognizer(GetConfig(fromLanguage, toLanguages, voice, endpointId), audioInput);
        }

        public async Task<EventArgs> GetTranslationRecognizedEvents(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice:null)))
            {
                EventArgs eventArgs = null;
                recognizer.Recognized += (s, e) => eventArgs = e;
                recognizer.Synthesizing += (s, e) => eventArgs = e;
                recognizer.Recognizing += (s, e) => eventArgs = e;
                await Task.WhenAny(recognizer.RecognizeOnceAsync(), Task.Delay(timeout));
                return eventArgs;
            }
        }

        public async Task<TranslationRecognitionResult> GetTranslationFinalResult(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice:null)))
            {
                var tcs = new TaskCompletionSource<int>();
                recognizer.SessionStopped += (s, e) =>
                {
                    tcs.TrySetResult(0);
                };
                recognizer.Canceled += (s, e) =>
                {
                    Console.WriteLine("Canceled: " + e.SessionId);
                    tcs.TrySetResult(0);
                };
                using (var connection = Connection.FromRecognizer(recognizer))
                {

                    int connectedEventCount = 0;
                    int disconnectedEventCount = 0;
                    connection.Connected += (s, e) =>
                    {
                        connectedEventCount++;
                    };
                    connection.Disconnected += (s, e) =>
                    {
                        disconnectedEventCount++;
                    };

                    TranslationRecognitionResult result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                    await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                    SpeechRecognitionTestsHelper.AssertConnectionCountMatching(connectedEventCount, disconnectedEventCount);
                    // It is not required to close the conneciton explictly. But it is also used to keep the connection object alive to ensure that
                    // connected and disconencted events can be received.
                    connection.Close();
                    return result;
                }
            }
        }

        public async Task<Dictionary<ResultType, List<EventArgs>>> GetTranslationRecognizedContinuous(string path, string fromLanguage, List<string> toLanguages, string voice=null, bool requireTranslatedSpeech = true)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice)))
            {
                return await DoTranslationAsync(recognizer, requireTranslatedSpeech);
            }
        }

        public async Task<Dictionary<ResultType, List<EventArgs>>> DoTranslationAsync(TranslationRecognizer recognizer, bool requireTranslatedSpeech)
        {
            using (var connection = Connection.FromRecognizer(recognizer))
            {
                var tcs = new TaskCompletionSource<bool>();
                var receivedRecognizedEvents = new Dictionary<ResultType, List<EventArgs>>(); ;
                var textResultEvents = new List<EventArgs>();
                var synthesisResultEvents = new List<EventArgs>();
                bool synthesisFailed = false;

                int connectedEventCount = 0;
                int disconnectedEventCount = 0;
                connection.Connected += (s, e) =>
                {
                    connectedEventCount++;
                };
                connection.Disconnected += (s, e) =>
                {
                    disconnectedEventCount++;
                };

                string canceled = string.Empty;
                recognizer.Canceled += (s, e) =>
                {
                    if (e.Reason != CancellationReason.EndOfStream)
                    {
                        canceled = e.ErrorDetails;
                        tcs.TrySetResult(false);
                    }
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Received final result event: {e.ToString()}");
                    if (!requireTranslatedSpeech || e.Result.Reason == ResultReason.TranslatedSpeech)
                    {
                        textResultEvents.Add(e);
                    }
                };

                recognizer.Synthesizing += (s, e) =>
                {
                    Console.WriteLine($"Received synthesis event: {e.ToString()}");
                    if (e.Result.GetAudio().Length > 0)
                    {
                        synthesisResultEvents.Add(e);
                    }
                    else if (e.Result.GetAudio().Length == 0 && e.Result.Reason != ResultReason.SynthesizingAudioCompleted)
                    {
                        Console.WriteLine($"Synthesizing event failure: Reason:{0} Audio.Length={1}", e.Result.Reason, e.Result.GetAudio().Length);
                        synthesisFailed = true;
                    }
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Received stopped session event: {e.ToString()}");
                    tcs.TrySetResult(true);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

                GC.KeepAlive(connection);

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition Canceled: {canceled}");
                }

                SpeechRecognitionTestsHelper.AssertConnectionCountMatching(connectedEventCount, disconnectedEventCount);

                if (synthesisFailed)
                {
                    Assert.Fail($"Synthesis failed.");
                }

                receivedRecognizedEvents.Add(ResultType.RecognizedText, textResultEvents);
                receivedRecognizedEvents.Add(ResultType.Synthesis, synthesisResultEvents);
                return receivedRecognizedEvents;
            }
        }

        public async Task<Dictionary<ResultType, List<TranslationRecognitionEventArgs>>> GetTranslationRecognizingContinuous(string path, string fromLanguage, List<string> toLanguages, string endpointId = null, bool allowErrors = false)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, null, endpointId)))
            {
                var tcs = new TaskCompletionSource<bool>();
                var receivedEvents = new Dictionary<ResultType, List<TranslationRecognitionEventArgs>>();
                var recognizingEvents = new List<TranslationRecognitionEventArgs>();
                var recognizedEvents = new List<TranslationRecognitionEventArgs>();
                var cancelledEvents = new List<TranslationRecognitionEventArgs>();

                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine($"Got intermediate result {e.ToString()}");
                    recognizingEvents.Add(e);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Got final result {e.ToString()}");
                    recognizedEvents.Add(e);
                };

                string canceled = string.Empty;
                recognizer.Canceled += (s, e) =>
                {
                    canceled = e.ErrorDetails;
                    cancelledEvents.Add(e);
                    tcs.TrySetResult(false);
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Session stopped {e.ToString()}");
                    tcs.TrySetResult(true);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

                if (!allowErrors && !string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition canceled: {canceled}");
                }

                receivedEvents.Add(ResultType.RecognizedText, recognizedEvents);
                receivedEvents.Add(ResultType.RecognizingText, recognizingEvents);
                receivedEvents.Add(ResultType.Cancelled, cancelledEvents);
                return receivedEvents;
            }
        }

        public async Task<Tuple<List<TranslationSynthesisEventArgs>, TranslationRecognitionResult>> GetTranslationSynthesisAndFinalResult(string path, string fromLanguage, List<string> toLanguages, string voice)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice)))
            {
                var eventArgs = new List<TranslationSynthesisEventArgs>();
                var tcs = new TaskCompletionSource<bool>();
                recognizer.SessionStopped += (s, e) => tcs.TrySetResult(true);
                recognizer.Synthesizing += (s, e) => eventArgs.Add(e);
                var result = await recognizer.RecognizeOnceAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));

                return Tuple.Create(eventArgs, result);
            }
        }

        public static TranslationRecognizer TrackSessionId(TranslationRecognizer recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }

        public static void AssertDetailedOutput(TranslationRecognitionResult result, bool requireDetailedOutput)
        {
            var json = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
            Assert.IsFalse(string.IsNullOrEmpty(json), "Empty JSON result from translation recognition.");
            Assert.IsTrue(json.Contains("Text"), "Detailed result does not contain Text.");
            Assert.IsTrue(requireDetailedOutput == json.Contains("ITN"), "Detailed result does not contain ITN.");
            Assert.IsTrue(requireDetailedOutput == json.Contains("Lexical"), "Detailed result does not contain Lexical.");
            Assert.IsTrue(requireDetailedOutput == json.Contains("MaskedITN"), "Detailed result does not contain MaskedITN.");
            Assert.IsTrue(json.Contains("Text"), "Detailed result does not contain Text.");
        }
    }

    enum ResultType
    {
        RecognizedText,
        RecognizingText,
        Synthesis,
        Cancelled
    }

    public class TranslationRecognitionTestRun
    {
        private RecognitionTestBase _testClass;
        private TranslationRecognizer _recog;
        private Connection _conn;
        private TaskCompletionSource<bool> _completion = new TaskCompletionSource<bool>();
        private TaskCompletionSource<bool> _audio = new TaskCompletionSource<bool>();
        private List<TranslationRecognitionCanceledEventArgs> _canceled = new List<TranslationRecognitionCanceledEventArgs>();
        private List<TranslationRecognitionEventArgs> _recognized = new List<TranslationRecognitionEventArgs>();
        private List<TranslationRecognitionEventArgs> _recognizing = new List<TranslationRecognitionEventArgs>();
        private List<RecognitionEventArgs> _speechEndDetected = new List<RecognitionEventArgs>();
        private List<RecognitionEventArgs> _speechStartDetected = new List<RecognitionEventArgs>();
        private List<SessionEventArgs> _sessionStarted = new List<SessionEventArgs>();
        private List<SessionEventArgs> _sessionStopped = new List<SessionEventArgs>();
        private List<TranslationSynthesisEventArgs> _synthesizing = new List<TranslationSynthesisEventArgs>();
        private List<ConnectionEventArgs> _connected = new List<ConnectionEventArgs>();
        private List<ConnectionEventArgs> _disconnected = new List<ConnectionEventArgs>();
        private List<ConnectionMessageEventArgs> _messageReceived = new List<ConnectionMessageEventArgs>();
        private HashSet<string> _registeredHandlers = new HashSet<string>();

        public TranslationRecognitionTestRun(TranslationRecognizer recog, RecognitionTestBase testClass)
        {
            if (recog == null)
            {
                throw new ArgumentNullException(nameof(recog));
            }
            else if (testClass == null)
            {
                throw new ArgumentNullException(nameof(testClass));
            }

            _recog = recog;
            _testClass = testClass;

            AddEventHandler(nameof(TranslationRecognizer.SessionStarted));
        }

        public string SessionId => (
            _sessionStarted.FirstOrDefault(ss => !string.IsNullOrWhiteSpace(ss.SessionId))
                ?? _sessionStopped.FirstOrDefault(ss => !string.IsNullOrWhiteSpace(ss.SessionId))
        )?.SessionId;

        public IReadOnlyList<TranslationRecognitionCanceledEventArgs> Canceled => _canceled;
        public IReadOnlyList<SessionEventArgs> Recognized => _recognized;
        public IReadOnlyList<SessionEventArgs> Recognizing => _recognizing;
        public IReadOnlyList<RecognitionEventArgs> SpeechEndDetected => _speechEndDetected;
        public IReadOnlyList<RecognitionEventArgs> SpeechStartDetected => _speechStartDetected;
        public IReadOnlyList<SessionEventArgs> SessionStarted => _sessionStarted;
        public IReadOnlyList<SessionEventArgs> SessionStopped => _sessionStopped;
        public IReadOnlyList<TranslationSynthesisEventArgs> Synthesizing => _synthesizing;

        public IReadOnlyList<ConnectionEventArgs> Connected => _connected;
        public IReadOnlyList<ConnectionEventArgs> Disconnected => _disconnected;
        public IReadOnlyList<ConnectionMessageEventArgs> MessageReceived => _messageReceived;

        /// <summary>
        /// Registers an event handler
        /// </summary>
        /// <param name="eventName">The name of the event to register an event handler for. E.g. nameof(TranslationRecognizer.Canceled)</param>
        /// <returns>Reference to self for chaining</returns>
        public TranslationRecognitionTestRun AddEventHandler(string eventName)
        {
            if (string.IsNullOrWhiteSpace(eventName))
            {
                throw new ArgumentException(nameof(eventName) + " cannot be null, empty or consist only of white space");
            }
            else if (_registeredHandlers.Contains(eventName))
            {
                // prevent adding duplicate handlers
                return this;
            }

            switch (eventName)
            {
                default:
                    throw new ArgumentException("Unknown event: " + eventName);
                
                case nameof(TranslationRecognizer.Canceled):
                    _recog.Canceled += (s, e) =>
                    {
                        _canceled.Add(_testClass.LogEvent(e, "Canceled"));
                        switch (e.Reason)
                        {
                            case CancellationReason.Error:
                                _completion.TrySetResult(false);
                                break;

                            case CancellationReason.EndOfStream:
                                _audio.TrySetResult(true);
                                break;

                            case CancellationReason.CancelledByUser:
                                _audio.TrySetResult(false);
                                break;

                            default:
                                throw new NotSupportedException(
                                    "Don't know how to handle the following cancellation event: " + Newtonsoft.Json.JsonConvert.SerializeObject(e));
                        }
                    };
                    break;
                case nameof(TranslationRecognizer.Recognized):
                    _recog.Recognized += (s, e) => _recognized.Add(_testClass.LogEvent(e, "Recognized"));
                    break;
                case nameof(TranslationRecognizer.Recognizing):
                    _recog.Recognizing += (s, e) => _recognizing.Add(_testClass.LogEvent(e, "Recognizing"));
                    break;
                case nameof(TranslationRecognizer.SessionStarted):
                    _recog.SessionStarted += (s, e) => _sessionStarted.Add(_testClass.LogEvent(e, "SessionStarted"));
                    break;
                case nameof(TranslationRecognizer.SessionStopped):
                    _recog.SessionStopped += (s, e) =>
                    {
                        _sessionStopped.Add(_testClass.LogEvent(e, "SessionStopped"));
                        _audio.TrySetResult(false); // this should have already been set but just in case
                        _completion.TrySetResult(true);
                    };
                    break;
                case nameof(TranslationRecognizer.SpeechEndDetected):
                    _recog.SpeechEndDetected += (s, e) => _speechEndDetected.Add(_testClass.LogEvent(e, "SpeechEndDetected"));
                    break;
                case nameof(TranslationRecognizer.SpeechStartDetected):
                    _recog.SpeechStartDetected += (s, e) => _speechStartDetected.Add(_testClass.LogEvent(e, "SpeechStartDetected"));
                    break;
                case nameof(TranslationRecognizer.Synthesizing):
                    _recog.Synthesizing += (s, e) => _synthesizing.Add(_testClass.LogEvent(e, "Synthesizing"));
                    break;
                case nameof(Connection.Connected):
                    EnsureConnection().Connected += (s, e) => _connected.Add(_testClass.LogEvent(e, "Connected"));
                    break;
                case nameof(Connection.Disconnected):
                    EnsureConnection().Disconnected += (s, e) => _disconnected.Add(_testClass.LogEvent(e, "Disconnected"));
                    break;
                case nameof(Connection.MessageReceived):
                    EnsureConnection().MessageReceived += (s, e) => _messageReceived.Add(_testClass.LogEvent(e, "MessageReceived"));
                    break;
            }

            _registeredHandlers.Add(eventName);
            
            return this;
        }

        /// <summary>
        /// Waits for a canceled event indicating an error, or a session stopped event
        /// </summary>
        /// <param name="timeout">How long to wait. Default value of null waits forever</param>
        /// <returns>Asynchronous task</returns>
        /// <exception cref="TimeoutException">If the we timed out while waiting</exception>
        public async Task WaitForCompletion(TimeSpan? timeout = null)
        {
            // make sure we've registered for all required handlers
            if (!_registeredHandlers.Contains(nameof(TranslationRecognizer.Canceled))
                || !_registeredHandlers.Contains(nameof(TranslationRecognizer.SessionStopped)))
            {
                throw new InvalidOperationException("You must register at least the canceled and session stopped event handlers first");
            }

            await WaitFor(_completion.Task, timeout);

            // if we timed out an exception would have been thrown so we know the completion task is done
            await _completion.Task;
        }

        /// <summary>
        /// Waits for the end of stream cancelled event, or an error event
        /// </summary>
        /// <param name="timeout">How long to wait. Default value of null waits forever</param>
        /// <returns>Asynchronous task</returns>
        /// <exception cref="TimeoutException">If we timed out while waiting</exception>
        public async Task WaitForEndOfStream(TimeSpan? timeout = null)
        {
            // make sure we've registered for all required handlers
            if (!_registeredHandlers.Contains(nameof(TranslationRecognizer.Canceled))
                || !_registeredHandlers.Contains(nameof(TranslationRecognizer.SessionStopped)))
            {
                throw new InvalidOperationException("You must register at least the canceled and session stopped event handlers first");
            }

            await WaitFor(_audio.Task, timeout);

            // if we timed out an exception would have been thrown so we know the completion task is done
            await _audio.Task;
        }

        private static async Task WaitFor(Task<bool> completionTask, TimeSpan? timeout)
        {
            if (timeout != null && timeout != Timeout.InfiniteTimeSpan && timeout < TimeSpan.Zero)
            {
                throw new ArgumentException("Invalid timeout value: " + timeout);
            }

            if (timeout == null)
            {
                timeout = Timeout.InfiniteTimeSpan;
            }

            using (var cts = new CancellationTokenSource())
            {
                var tasks = new List<Task>() { completionTask };
                if (timeout != Timeout.InfiniteTimeSpan)
                {
                    tasks.Add(Task.Delay(timeout.Value, cts.Token));
                }

                var finishedTask = await Task.WhenAny(tasks);
                if (finishedTask != completionTask)
                {
                    throw new TimeoutException("Timed out waiting for a session stopped, or canceled error event. Timeout: " + timeout);
                }
                else
                {
                    // abort the delay task to avoid "consuming" a thread pointlessly
                    cts.Cancel();
                }
            }
        }

        private Connection EnsureConnection()
        {
            if (_conn == null)
            {
                _conn = Connection.FromRecognizer(_recog);
            }

            return _conn;
        }
    }
}
