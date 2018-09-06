//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class SpeechRecognitionTestsHelper
    {
        public int ErrorEventCount { get; set; }

        public int FinalResultEventCount { get; set; }

        public int SessionStartedEventCount { get; set; }

        public int SessionStoppedEventCount { get; set; }

        public int SpeechEndedEventCount { get; set; }

        public int SpeechStartedEventCount { get; set; }

        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromMinutes(6);

        public SpeechRecognitionTestsHelper()
        {
            ErrorEventCount = 0;
            FinalResultEventCount = 0;
            SessionStartedEventCount = 0;
            SessionStoppedEventCount = 0;
            SpeechEndedEventCount = 0;
            SpeechStartedEventCount = 0;
            taskCompletionSource = new TaskCompletionSource<int>();
        }

        public async Task CompleteContinuousRecognition(SpeechRecognizer recognizer)
        {
            taskCompletionSource = new TaskCompletionSource<int>();
            recognizer.OnSessionEvent += (s, e) =>
            {
                if (e.EventType == SessionEventType.SessionStoppedEvent)
                {
                    taskCompletionSource.TrySetResult(0);
                }
            };
            string error = string.Empty;
            recognizer.RecognitionErrorRaised += (s, e) => { error = e.ToString(); };

            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

            if (!string.IsNullOrEmpty(error))
            {
                Assert.Fail($"Error received: {error}");
            }
        }

        public async Task<string> GetFirstRecognizerResult(SpeechRecognizer speechRecognizer)
        {
            List<string> recognizedText = new List<string>();
            speechRecognizer.FinalResultReceived += (s, e) =>
            {
                Console.WriteLine($"Received result '{e.ToString()}'");
                if (e.Result.Text.Length > 0)
                {
                    recognizedText.Add(e.Result.Text);
                }
            };

            await CompleteContinuousRecognition(speechRecognizer);

            speechRecognizer.Dispose();
            return recognizedText.Count > 0 ? recognizedText[0] : string.Empty;
        }

        private void FinalResultEventCounter(object sender, SpeechRecognitionResultEventArgs e)
        {
            FinalResultEventCount++;
        }

        private void ErrorEventCounter(object sender, RecognitionErrorEventArgs e)
        {
            ErrorEventCount++;
        }

        private void SessionEventCounter(object sender, SessionEventArgs e)
        {
            if (e.EventType == SessionEventType.SessionStartedEvent)
            {
                SessionStartedEventCount++;
            }
            else if (e.EventType == SessionEventType.SessionStoppedEvent)
            {
                SessionStoppedEventCount++;
            }
        }

        private void SpeechEventCounter(object sender, RecognitionEventArgs e)
        {
            if (e.EventType == RecognitionEventType.SpeechEndDetectedEvent)
            {
                SpeechEndedEventCount++;
            }
            else if (e.EventType == RecognitionEventType.SpeechStartDetectedEvent)
            {
                SpeechStartedEventCount++;
            }
        }

        public static SpeechRecognizer TrackSessionId(SpeechRecognizer recognizer)
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

        public void SubscribeToCounterEventHandlers(SpeechRecognizer recognizer)
        {
            recognizer.FinalResultReceived += FinalResultEventCounter;
            recognizer.RecognitionErrorRaised += ErrorEventCounter;
            recognizer.OnSessionEvent += SessionEventCounter;
            recognizer.OnSpeechDetectedEvent += SpeechEventCounter;
        }

        public void UnsubscribeFromCounterEventHandlers(SpeechRecognizer recognizer)
        {
            recognizer.FinalResultReceived -= FinalResultEventCounter;
            recognizer.RecognitionErrorRaised -= ErrorEventCounter;
            recognizer.OnSessionEvent -= SessionEventCounter;
            recognizer.OnSpeechDetectedEvent -= SpeechEventCounter;
        }

        public static void AssertMatching(string expectedText, string actualText)
        {
            string plainActualText = Normalize(actualText);
            string plainExpectedText = Normalize(expectedText);

            if (plainActualText.Length <= plainExpectedText.Length)
            {
                Assert.IsTrue(
                    plainExpectedText.Contains(plainActualText),
                    $"'{plainExpectedText}' does not contain '{plainActualText}' as expected");
            }
            else
            {
                Assert.IsTrue(
                    plainActualText.Contains(plainExpectedText),
                    $"'{plainActualText}' does not contain '{plainExpectedText}' as expected");
            }
        }

        public static string Normalize(string str)
        {
            str = str.ToLower();
            return Regex.Replace(str, "[^a-z0-9' ]+", "", RegexOptions.Compiled);
        }
    }
}
