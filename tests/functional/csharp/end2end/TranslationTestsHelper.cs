//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.VisualStudio.TestTools.UnitTesting;
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

        public SpeechTranslatorConfig GetConfig(string path, string fromLanguage, List<string> toLanguages, string voice)
        {
            var config = SpeechTranslatorConfig.FromSubscription(this.subscriptionKey, this.region);
            config.SpeechRecognitionLanguage = fromLanguage;
            toLanguages.ForEach(l => config.AddTargetLanguage(l));

            if (!string.IsNullOrEmpty(voice))
            {
                config.VoiceName = voice;
            }
            return config;
        }

        TranslationRecognizer CreateTranslationRecognizer(string path, string fromLanguage, List<string> toLanguages, string voice = null)
        {
            var audioInput = AudioConfig.FromWavFileInput(path);
            return new TranslationRecognizer(GetConfig(path, fromLanguage, toLanguages, voice), audioInput);
        }

        public async Task<EventArgs> GetTranslationFinalResultEvents(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages)))
            {
                EventArgs eventArgs = null;
                recognizer.FinalResultReceived += (s, e) => eventArgs = e;
                recognizer.SynthesisResultReceived += (s, e) => eventArgs = e;
                recognizer.IntermediateResultReceived += (s, e) => eventArgs = e;
                await Task.WhenAny(recognizer.RecognizeAsync(), Task.Delay(timeout));
                return eventArgs;
            }
        }

        public async Task<TranslationTextResult> GetTranslationFinalResult(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages)))
            {
                TranslationTextResult result = null;
                await Task.WhenAny(recognizer.RecognizeAsync().ContinueWith(t => result = t.Result), Task.Delay(timeout));
                return result;
            }
        }

        public async Task<Dictionary<ResultType, List<EventArgs>>> GetTranslationFinalResultContinuous(string path, string fromLanguage, List<string> toLanguages, string voice=null)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice)))
            {
                var tcs = new TaskCompletionSource<bool>();
                var receivedFinalResultEvents = new Dictionary<ResultType, List<EventArgs>>(); ;
                var textResultEvents = new List<EventArgs>();
                var synthesisResultEvents = new List<EventArgs>();

                string error = string.Empty;
                recognizer.RecognitionErrorRaised += (s, e) => { error = e.ToString(); };
                recognizer.FinalResultReceived += (s, e) =>
                {
                    Console.WriteLine($"Received final result event: {e.ToString()}");
                    textResultEvents.Add(e);
                };

                recognizer.SynthesisResultReceived += (s, e) =>
                {
                    Console.WriteLine($"Received synthesis event: {e.ToString()}");
                    if (e.Result.Status == SynthesisStatus.Success)
                    {
                        synthesisResultEvents.Add(e);
                    }
                };

                recognizer.OnSessionEvent += (s, e) =>
                {
                    Console.WriteLine($"Received session event: {e.ToString()}");
                    if (e.EventType == SessionEventType.SessionStoppedEvent)
                    {
                        tcs.TrySetResult(true);
                    }
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

                if (!string.IsNullOrEmpty(error))
                {
                    Assert.Fail($"Error received: {error}");
                }

                receivedFinalResultEvents.Add(ResultType.Text, textResultEvents);
                receivedFinalResultEvents.Add(ResultType.Synthesis, synthesisResultEvents);
                return receivedFinalResultEvents;
            }
        }

        public async Task<List<List<TranslationTextResultEventArgs>>> GetTranslationIntermediateResultContinuous(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages)))
            {
                var tcs = new TaskCompletionSource<bool>();
                var listOfIntermediateResults = new List<List<TranslationTextResultEventArgs>>();
                List<TranslationTextResultEventArgs> receivedIntermediateResultEvents = null;

                recognizer.OnSessionEvent += (s, e) =>
                {
                    if (e.EventType.ToString().Equals("SessionStartedEvent"))
                    {
                        Console.WriteLine($"Session started {e.ToString()}");
                        receivedIntermediateResultEvents = new List<TranslationTextResultEventArgs>();
                    }
                    if (e.EventType.ToString().Equals("SessionStoppedEvent"))
                    {
                        Console.WriteLine($"Session stopped {e.ToString()}");
                        tcs.TrySetResult(true);
                    }
                };
                recognizer.IntermediateResultReceived += (s, e) =>
                {
                    Console.WriteLine($"Got intermediate result {e.ToString()}");
                    receivedIntermediateResultEvents.Add(e);
                };

                recognizer.FinalResultReceived += (s, e) =>
                {
                    Console.WriteLine($"Got final result {e.ToString()}");
                    listOfIntermediateResults.Add(receivedIntermediateResultEvents);
                    receivedIntermediateResultEvents = new List<TranslationTextResultEventArgs>();
                };

                string error = string.Empty;
                recognizer.RecognitionErrorRaised += (s, e) =>
                {
                    error = e.ToString();
                    tcs.TrySetResult(false);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

                if (!string.IsNullOrEmpty(error))
                {
                    Assert.Fail($"Error received: {error}");
                }

                return listOfIntermediateResults;
            }
        }

        public static TranslationRecognizer TrackSessionId(TranslationRecognizer recognizer)
        {
            recognizer.OnSessionEvent += (s, e) =>
            {
                if (e.EventType == SessionEventType.SessionStartedEvent)
                {
                    Console.WriteLine("SessionId: " + e.SessionId);
                }
            };
            return recognizer;
        }
    }

    enum ResultType
    {
        Text,
        Synthesis
    }
}
