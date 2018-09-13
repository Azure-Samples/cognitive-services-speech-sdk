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

        public SpeechTranslationConfig GetConfig(string path, string fromLanguage, List<string> toLanguages, string voice)
        {
            var config = SpeechTranslationConfig.FromSubscription(this.subscriptionKey, this.region);
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

        public async Task<EventArgs> GetTranslationRecognizedEvents(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages)))
            {
                EventArgs eventArgs = null;
                recognizer.Recognized += (s, e) => eventArgs = e;
                recognizer.Synthesized += (s, e) => eventArgs = e;
                recognizer.Recognizing += (s, e) => eventArgs = e;
                await Task.WhenAny(recognizer.RecognizeOnceAsync(), Task.Delay(timeout));
                return eventArgs;
            }
        }

        public async Task<TranslationTextResult> GetTranslationFinalResult(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages)))
            {
                TranslationTextResult result = null;
                await Task.WhenAny(recognizer.RecognizeOnceAsync().ContinueWith(t => result = t.Result), Task.Delay(timeout));
                return result;
            }
        }

        public async Task<Dictionary<ResultType, List<EventArgs>>> GetTranslationRecognizedContinuous(string path, string fromLanguage, List<string> toLanguages, string voice=null)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages, voice)))
            {
                var tcs = new TaskCompletionSource<bool>();
                var receivedRecognizedEvents = new Dictionary<ResultType, List<EventArgs>>(); ;
                var textResultEvents = new List<EventArgs>();
                var synthesisResultEvents = new List<EventArgs>();

                string canceled = string.Empty;
                recognizer.Canceled += (s, e) => { canceled = e.ToString(); };
                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Received final result event: {e.ToString()}");
                    textResultEvents.Add(e);
                };

                recognizer.Synthesized += (s, e) =>
                {
                    Console.WriteLine($"Received synthesis event: {e.ToString()}");
                    if (e.Result.Audio.Length > 0)
                    {
                        synthesisResultEvents.Add(e);
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

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition Canceled: {canceled}");
                }

                receivedRecognizedEvents.Add(ResultType.Text, textResultEvents);
                receivedRecognizedEvents.Add(ResultType.Synthesis, synthesisResultEvents);
                return receivedRecognizedEvents;
            }
        }

        public async Task<List<List<TranslationTextResultEventArgs>>> GetTranslationRecognizingContinuous(string path, string fromLanguage, List<string> toLanguages)
        {
            using (var recognizer = TrackSessionId(CreateTranslationRecognizer(path, fromLanguage, toLanguages)))
            {
                var tcs = new TaskCompletionSource<bool>();
                var listOfIntermediateResults = new List<List<TranslationTextResultEventArgs>>();
                List<TranslationTextResultEventArgs> receivedRecognizingEvents = null;

                recognizer.SessionStarted += (s, e) =>
                {
                    Console.WriteLine($"Session started {e.ToString()}");
                    receivedRecognizingEvents = new List<TranslationTextResultEventArgs>();
                };
                recognizer.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"Session stopped {e.ToString()}");
                    tcs.TrySetResult(true);
                };
                recognizer.Recognizing += (s, e) =>
                {
                    Console.WriteLine($"Got intermediate result {e.ToString()}");
                    receivedRecognizingEvents.Add(e);
                };

                recognizer.Recognized += (s, e) =>
                {
                    Console.WriteLine($"Got final result {e.ToString()}");
                    listOfIntermediateResults.Add(receivedRecognizingEvents);
                    receivedRecognizingEvents = new List<TranslationTextResultEventArgs>();
                };

                string canceled = string.Empty;
                recognizer.Canceled += (s, e) =>
                {
                    canceled = e.ToString();
                    tcs.TrySetResult(false);
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

                if (!string.IsNullOrEmpty(canceled))
                {
                    Assert.Fail($"Recognition canceled: {canceled}");
                }

                return listOfIntermediateResults;
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
        Text,
        Synthesis
    }
}
