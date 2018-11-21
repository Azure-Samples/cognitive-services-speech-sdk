//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class SpeechRecognitionTestsHelper
    {
        public int ErrorEventCount { get; set; }

        public int RecognizedEventCount { get; set; }

        public int SessionStartedEventCount { get; set; }

        public int SessionStoppedEventCount { get; set; }

        public int SpeechEndedEventCount { get; set; }

        public int SpeechStartedEventCount { get; set; }

        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromMinutes(6);

        public SpeechRecognitionTestsHelper()
        {
            ErrorEventCount = 0;
            RecognizedEventCount = 0;
            SessionStartedEventCount = 0;
            SessionStoppedEventCount = 0;
            SpeechEndedEventCount = 0;
            SpeechStartedEventCount = 0;
            taskCompletionSource = new TaskCompletionSource<int>();
        }

        public async Task CompleteContinuousRecognition(SpeechRecognizer recognizer)
        {
            taskCompletionSource = new TaskCompletionSource<int>();
            recognizer.SessionStopped += (s, e) =>
            {
                taskCompletionSource.TrySetResult(0);
            };
            string canceled = string.Empty;
            recognizer.Canceled += (s, e) => { canceled = e.ErrorDetails; };

            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

            if (!string.IsNullOrEmpty(canceled))
            {
                Assert.Fail($"Recognition Canceled: {canceled}");
            }
        }

        public async Task<string> GetFirstRecognizerResult(SpeechRecognizer speechRecognizer)
        {
            List<string> recognizedText = new List<string>();
            speechRecognizer.Recognized += (s, e) =>
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

        private void RecognizedEventCounter(object sender, SpeechRecognitionEventArgs e)
        {
            RecognizedEventCount++;
        }

        private void CanceledEventCounter(object sender, SpeechRecognitionCanceledEventArgs e)
        {
            if (e.Reason != CancellationReason.EndOfStream)
            {
                ErrorEventCount++;
            }
        }

        private void SessionStartedEventCounter(object sender, SessionEventArgs e)
        {
            SessionStartedEventCount++;
        }

        private void SessionStoppedEventCounter(object sender, SessionEventArgs e)
        {
            SessionStoppedEventCount++;
        }

        private void SpeechStartEventCounter(object sender, RecognitionEventArgs e)
        {
            SpeechStartedEventCount++;
        }

        private void SpeechEndEventCounter(object sender, RecognitionEventArgs e)
        {
            SpeechEndedEventCount++;
        }

        public static SpeechRecognizer TrackSessionId(SpeechRecognizer recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }

        public void SubscribeToCounterEventHandlers(SpeechRecognizer recognizer)
        {
            recognizer.Recognized += RecognizedEventCounter;
            recognizer.Canceled += CanceledEventCounter;
            recognizer.SessionStarted += SessionStartedEventCounter;
            recognizer.SessionStopped += SessionStoppedEventCounter;
            recognizer.SpeechStartDetected += SpeechStartEventCounter;
            recognizer.SpeechEndDetected += SpeechEndEventCounter;
        }

        public void UnsubscribeFromCounterEventHandlers(SpeechRecognizer recognizer)
        {
            recognizer.Recognized -= RecognizedEventCounter;
            recognizer.Canceled -= CanceledEventCounter;
            recognizer.SessionStarted -= SessionStartedEventCounter;
            recognizer.SessionStopped -= SessionStoppedEventCounter;
            recognizer.SpeechStartDetected -= SpeechStartEventCounter;
            recognizer.SpeechEndDetected -= SpeechEndEventCounter;
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

        public SpeechRecognizer GetSpeechRecognizingAsyncNotAwaited(SpeechRecognizer recognizer)
        {
            using (var rec = recognizer)
            {
                rec.RecognizeOnceAsync();
                Thread.Sleep(100);
                return rec;
            }
        }

        public static string Normalize(string str)
        {
            str = str.ToLower();
            return Regex.Replace(str, "[^a-z0-9' ]+", "", RegexOptions.Compiled);
        }
    }
}
