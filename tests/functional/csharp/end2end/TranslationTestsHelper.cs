//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Translation;
using System;
using System.Collections.Generic; 
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class TranslationTestsHelper
    {
        private SpeechFactory factory;
        private TimeSpan timeout;

        public TranslationTestsHelper (SpeechFactory factory)
        {
            this.factory = factory;
            timeout = TimeSpan.FromSeconds(90);
        }

        TranslationRecognizer CreateTranslationRecognizer(string path, string fromLanguage, List<string> toLanguages, string voice=null)
        {
            if (string.IsNullOrEmpty(voice))
            {
                return this.factory.CreateTranslationRecognizerWithFileInput(path, fromLanguage, toLanguages);
            }
            else
            {
                return this.factory.CreateTranslationRecognizerWithFileInput(path, fromLanguage, toLanguages, voice);
            }
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

                recognizer.FinalResultReceived += (s, e) => textResultEvents.Add(e);
                recognizer.SynthesisResultReceived += (s, e) =>
                {
                    if (e.Result.Status == SynthesisStatus.Success)
                    {
                        synthesisResultEvents.Add(e);
                    }
                };

                recognizer.OnSessionEvent += (s, e) =>
                {
                    if (e.EventType.ToString().Equals("SessionStoppedEvent"))
                    {
                        tcs.TrySetResult(true);
                    }
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

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
                        receivedIntermediateResultEvents = new List<TranslationTextResultEventArgs>();
                    }
                    if (e.EventType.ToString().Equals("SessionStoppedEvent"))
                    {
                        tcs.TrySetResult(true);
                    }
                };
                recognizer.IntermediateResultReceived += (s, e) => receivedIntermediateResultEvents.Add(e);
                recognizer.FinalResultReceived += (s, e) =>
                {
                    listOfIntermediateResults.Add(receivedIntermediateResultEvents);
                    receivedIntermediateResultEvents = new List<TranslationTextResultEventArgs>();
                };

                await recognizer.StartContinuousRecognitionAsync();
                await Task.WhenAny(tcs.Task, Task.Delay(timeout));
                await recognizer.StopContinuousRecognitionAsync();

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
