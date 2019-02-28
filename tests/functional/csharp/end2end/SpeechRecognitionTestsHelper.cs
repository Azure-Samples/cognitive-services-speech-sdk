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
    using Microsoft.CognitiveServices.Speech.Audio;

    sealed class SpeechRecognitionTestsHelper
    {
        public int ErrorEventCount { get; set; }

        public int RecognizedEventCount { get; set; }

        public int SessionStartedEventCount { get; set; }

        public int SessionStoppedEventCount { get; set; }

        public int SpeechEndedEventCount { get; set; }

        public int SpeechStartedEventCount { get; set; }

        public int ConnectedEventCount { get; set; }

        public int DisconnectedEventCount { get; set; }

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
            ConnectedEventCount = 0;
            DisconnectedEventCount = 0;
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

        public async Task<SpeechRecognitionResult> CompleteRecognizeOnceAsync(SpeechRecognizer recognizer, Connection connection = null)
        {
            taskCompletionSource = new TaskCompletionSource<int>();
            recognizer.SessionStopped += (s, e) =>
            {
                SessionStoppedEventCounter(s, e);
                taskCompletionSource.TrySetResult(0);
            };
            string canceled = string.Empty;
            recognizer.Canceled += (s, e) =>
            {
                CanceledEventCounter(s, e);
                canceled = e.ErrorDetails;
                taskCompletionSource.TrySetResult(0);
            };
            if (connection != null)
            {
                connection.Connected += ConnectedEventCounter;
                connection.Disconnected += DisconnectedEventCounter;
            }

            var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));

            if (connection != null)
            {
                connection.Connected -= ConnectedEventCounter;
                connection.Disconnected -= DisconnectedEventCounter;
            }

            Assert.IsTrue(SessionStoppedEventCount == 1 || ErrorEventCount == 1);
            if (!string.IsNullOrEmpty(canceled))
            {
                Assert.Fail($"Recognition Canceled: {canceled}");
            }

            return result;
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

        private void ConnectedEventCounter(object sender, ConnectionEventArgs e)
        {
            Console.WriteLine($"Connected: SessionId: {e.SessionId}");
            ConnectedEventCount++;
        }

        private void DisconnectedEventCounter(object sender, ConnectionEventArgs e)
        {
            Console.WriteLine($"Disconnected: SessionId: {e.SessionId}");
            DisconnectedEventCount++;
        }

        public static SpeechRecognizer TrackSessionId(SpeechRecognizer recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }

        public void SubscribeToCounterEventHandlers(SpeechRecognizer recognizer, Connection connection = null)
        {
            recognizer.Recognized += RecognizedEventCounter;
            recognizer.Canceled += CanceledEventCounter;
            recognizer.SessionStarted += SessionStartedEventCounter;
            recognizer.SessionStopped += SessionStoppedEventCounter;
            recognizer.SpeechStartDetected += SpeechStartEventCounter;
            recognizer.SpeechEndDetected += SpeechEndEventCounter;
            if (connection != null)
            {
                connection.Connected += ConnectedEventCounter;
                connection.Disconnected += DisconnectedEventCounter;
            }
        }

        public void UnsubscribeFromCounterEventHandlers(SpeechRecognizer recognizer, Connection connection = null)
        {
            recognizer.Recognized -= RecognizedEventCounter;
            recognizer.Canceled -= CanceledEventCounter;
            recognizer.SessionStarted -= SessionStartedEventCounter;
            recognizer.SessionStopped -= SessionStoppedEventCounter;
            recognizer.SpeechStartDetected -= SpeechStartEventCounter;
            recognizer.SpeechEndDetected -= SpeechEndEventCounter;
            if (connection != null)
            {
                connection.Connected -= ConnectedEventCounter;
                connection.Disconnected -= DisconnectedEventCounter;
            }
        }

        public static void AssertEqual(int expected, int actual, string errorMessage = "")
        {
            if (expected != actual)
            {
                Assert.Fail($"Actual value {actual} does not match the expected value {expected}. {errorMessage}");
            }
        }

        public static void AssertMatching(string expectedText, string actualText)
        {
            Assert.IsFalse(actualText.Length == 0, $"Actual text should not be empty, expected '{expectedText}'");

            string plainActualText = Normalize(actualText);
            string plainExpectedText = Normalize(expectedText);

            if (plainActualText.Length <= plainExpectedText.Length)
            {
                Assert.IsTrue(
                    plainExpectedText.Contains(plainActualText),
                    $"'{plainExpectedText}' (expected)\n does not contain \n'{plainActualText}' (actual)");
            }
            else
            {
                Assert.IsTrue(
                    plainActualText.Contains(plainExpectedText),
                    $"'{plainActualText}' (actual)\n does not contain \n'{plainExpectedText}' (expected)");
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

        public static void AssertConnectionCountMatching(int connectedEventCount, int disconnectedEventCount)
        {
            Console.WriteLine($"ConnectedEventCount: {connectedEventCount}, DisconnectedEventCount: {disconnectedEventCount}");
            Assert.IsTrue(connectedEventCount > 0, AssertOutput.ConnectedEventCountMustNotBeZero);
            Assert.IsTrue(connectedEventCount == disconnectedEventCount || connectedEventCount == disconnectedEventCount + 1, AssertOutput.ConnectedDisconnectedEventUnmatch);
        }

        public static async Task AssertConnectionError(SpeechConfig speechConfig, CancellationErrorCode expectedErrorCode, string expectedErrorMessage)
        {
            var audioInput = AudioConfig.FromWavFileInput(TestData.English.Weather.AudioFile);
            int connectedEventCount = 0;
            using (var recognizer = TrackSessionId(new SpeechRecognizer(speechConfig, audioInput)))
            {
                var connection = Connection.FromRecognizer(recognizer);
                connection.Connected += (s, e) =>
                {
                    connectedEventCount++;
                };
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                AssertEqual(0, connectedEventCount, AssertOutput.WrongConnectedEventCount);
                Assert.AreEqual(ResultReason.Canceled, result.Reason);
                var cancellation = CancellationDetails.FromResult(result);
                Assert.AreEqual(cancellation.Reason, CancellationReason.Error);
                Assert.AreEqual(cancellation.ErrorCode, expectedErrorCode);
                AssertHelpers.AssertStringContains(cancellation.ErrorDetails, expectedErrorMessage);
            }
        }

        static public void AssertStringWordEdits(string expectedString, string comparisonString, int deltaPercentage)
        // Using standard implementation for Levenshtein distance. Caveat: not optimized for memory consumption
        //   but sufficient for our test case and the string length we are expecting (less 600 words)
        // 
        // expectedString - normalized expected string
        // comparisonString - the normalized comparison string
        // deltaPercentage - how many word operations are allowed to change the input string into the comparison string
        //   expressed as a percentage of words
        //   Example: if input string contains 200 words, deltaPercantage is 5, 10 edit operations would be allowed
        {
            String[] wordsExpected = expectedString.Split(' ');
            String[] wordsComparison = comparisonString.Split(' ');

            const int maxWordsExpected = 800;  // just an arbitrary bound for this operation
                                               // reconsider memory usage (or alternative algorithm) if you need to increase this significantly
            Assert.IsTrue((wordsExpected.Length < maxWordsExpected) && (wordsExpected.Length > 0), $"number of words in expectedString out of bounds: '{wordsExpected.Length}'");
            Assert.IsTrue((wordsComparison.Length < maxWordsExpected) && (wordsComparison.Length > 0), $"number of words in wordsComparison out of bounds: '{wordsComparison.Length}'");

            int allowedEdits = deltaPercentage * wordsExpected.Length / 100;

            int[][] matrix = new int[wordsExpected.Length + 1][];

            // initialize matrix
            for (int i = 0; i < wordsExpected.Length + 1; i++)
            {
                matrix[i] = new int[wordsComparison.Length + 1];
                for (int j = 0; j < wordsComparison.Length + 1; j++)
                {
                    if (i == 0)
                        matrix[0][j] = j;
                    else if (j == 0)
                        matrix[i][0] = i;
                }
            }

            // compute matrix with distances to compute the shortest path 
            // using substitutions, inserts, deletes
            for (int i = 1; i < wordsExpected.Length + 1; i++)
            {
                for (int j = 1; j < wordsComparison.Length + 1; j++)
                {
                    if (wordsExpected[i - 1] == wordsComparison[j - 1])
                        matrix[i][j] = matrix[i - 1][j - 1];
                    else
                    {
                        var substitution = matrix[i - 1][j - 1] + 1;
                        var insertion = matrix[i][j - 1] + 1;
                        var deletion = matrix[i - 1][j] + 1;
                        matrix[i][j] = Math.Min(substitution, Math.Min(insertion, deletion));
                    }
                }
            }

            int edits = matrix[wordsExpected.Length][wordsComparison.Length];

            Assert.IsTrue(edits <= allowedEdits,$"Number of edit operations '{edits}' exceeding allowed edits '{allowedEdits}'\ninput:  '{expectedString}'compare: '{comparisonString}'\n");
        }
    }
}
