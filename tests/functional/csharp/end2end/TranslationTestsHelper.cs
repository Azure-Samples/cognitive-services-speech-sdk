//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using MicrosoftSpeechSDKSamples;
using System;
using System.Collections.Generic; 
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

        public SpeechTranslationConfig GetConfig(string path, string fromLanguage, List<string> toLanguages, string voice, string endpointId = null)
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
            return config;
        }

        public TranslationRecognizer CreateTranslationRecognizer(string path, string fromLanguage, List<string> toLanguages, string voice = null, string endpointId = null)
        {
            var audioInput = AudioConfig.FromWavFileInput(path);
            return new TranslationRecognizer(GetConfig(path, fromLanguage, toLanguages, voice, endpointId), audioInput);
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
                var connection = Connection.FromRecognizer(recognizer);
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

        public async Task<Dictionary<ResultType, List<EventArgs>>> GetTranslationRecognizedContinuous(string path, string fromLanguage, List<string> toLanguages, string voice=null, bool requireTranslatedSpeech = true)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice)))
            { 
                var connection = Connection.FromRecognizer(recognizer);
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
                recognizer.Canceled += (s, e) => { canceled = e.ErrorDetails; };
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

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine($"Received session started event: {e.ToString()}");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Received stopped session event: {e.ToString()}");
                    tcs.TrySetResult(true);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();
                // It is not required to close the conneciton explictly. But it is also used to keep the connection object alive to ensure that
                // connected and disconencted events can be received.
                connection.Close();

                SpeechRecognitionTestsHelper.AssertConnectionCountMatching(connectedEventCount, disconnectedEventCount);

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition Canceled: {canceled}");
                }

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
    }

    enum ResultType
    {
        RecognizedText,
        RecognizingText,
        Synthesis,
        Cancelled
    }
}
