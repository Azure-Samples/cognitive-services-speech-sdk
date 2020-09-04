//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using Microsoft.CognitiveServices.Speech.Audio;
    using static Config;

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
            recognizer.Canceled += (s, e) =>
            {
                canceled = e.ErrorDetails;
                taskCompletionSource.TrySetResult(0);
            };

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

        public static void AssertOneEqual(string[] expected, string actual)
        {
            var expectedString = String.Join("', '", expected);
            Assert.IsTrue(
                expected.Contains(actual),
                $"'{actual}' (actual) is not a member of '{expectedString}' (expected)");
        }

        public static void AssertIfNotContains(string result, string substring)
        {
            Assert.IsTrue(result.Contains(substring), $"Error: '{result}' does not contain expected substring '{substring}'");
        }
        public static void AssertIfContains(string result, string substring)
        {
            Assert.IsFalse(result.Contains(substring), $"Error: '{result}' contain unexpected substring '{substring}'");
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

        public static void AssertFuzzyMatching(Utterance[] expectedUtterances, string[] actualUtterances, int tolerance = 10)
        {
            List<string> expected = new List<string>(expectedUtterances.Length);

            foreach (var utterance in expectedUtterances)
            {
                expected.Add(utterance.Text);
            }

            AssertFuzzyMatching(expected.ToArray(), actualUtterances, tolerance);
        }

        public static void AssertFuzzyMatching(string[] expectedUtterances, string[] actualUtterances, int tolerance = 10)
        {
            // Checking text results.
            var texts = actualUtterances.Select(t => t).Where(t => !string.IsNullOrEmpty(t)).ToList();
            var expected = string.Join(" ", expectedUtterances);
            expected = Normalize(expected);

            var actual = string.Join(" ", texts.ToArray());
            actual = Normalize(actual);
            // dont do a hard string comparison, we allow a small percentage of word edits (word insert/delete/move)
            Assert.IsTrue(LevenshteinRatio(expected, actual) > TestData.Levenshtein.SimilarityScoreThreshold);
        }

        public static void AssertFuzzyMatching(string expectedUtterance, string actualUtterance, int tolerance = 10)
        {
            AssertFuzzyMatching(new string[] { expectedUtterance }, new string[] { actualUtterance }, tolerance);
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
            str = Regex.Replace(str, @"[^\w ]+", "", RegexOptions.Compiled);
            return Regex.Replace(str, @"\s+", " ", RegexOptions.Compiled);
        }

        public static string StripPunctuationForProfanity(string str)
        {
            str = str.ToLower();
            // strip all punctuation except *, which is used to mask profanity
            str = Regex.Replace(str, @"[!'#$%&'()+,-./:;<=>?@\[/\]^_{|}~]+", "", RegexOptions.Compiled);
            return Regex.Replace(str, @"\s+", " ", RegexOptions.Compiled);
        }

        public static void AssertConnectionCountMatching(int connectedEventCount, int disconnectedEventCount)
        {
            Console.WriteLine($"ConnectedEventCount: {connectedEventCount}, DisconnectedEventCount: {disconnectedEventCount}");
            Assert.IsTrue(connectedEventCount > 0, AssertOutput.ConnectedEventCountMustNotBeZero);
            Assert.IsTrue(connectedEventCount == disconnectedEventCount || connectedEventCount == disconnectedEventCount + 1, AssertOutput.ConnectedDisconnectedEventUnmatch);
        }

        public static async Task AssertConnectionError(SpeechConfig speechConfig, CancellationErrorCode expectedErrorCode, params string[] expectedErrorSubstrings)
            => await AssertConnectionError(speechConfig, expectedErrorCode, new List<string>(expectedErrorSubstrings));

        public static async Task AssertConnectionError(SpeechConfig speechConfig, CancellationErrorCode expectedErrorCode, string expectedErrorMessage, StringComparison comparison = StringComparison.InvariantCultureIgnoreCase)
            => await AssertConnectionError(speechConfig, expectedErrorCode, new List<string> { expectedErrorMessage }, comparison);

        public static async Task AssertConnectionError(SpeechConfig speechConfig, CancellationErrorCode expectedErrorCode, IEnumerable<string> expectedErrorSubstrings, StringComparison comparison = StringComparison.InvariantCultureIgnoreCase)
        {
            var audioInput = AudioConfig.FromWavFileInput(AudioUtterancesMap[AudioUtteranceKeys.SINGLE_UTTERANCE_ENGLISH].FilePath.GetRootRelativePath());
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
                Assert.AreEqual(CancellationReason.Error, cancellation.Reason);
                Assert.AreEqual(expectedErrorCode, cancellation.ErrorCode);
                foreach (var expectedSubstring in expectedErrorSubstrings)
                {
                    AssertHelpers.AssertStringContains(cancellation.ErrorDetails, expectedSubstring, comparison);
                }
            }
        }

        /// <summary>
        /// Using standard implementation for Levenshtein distance. Caveat: not optimized for memory consumption
        /// but sufficient for our test case and the string length we are expecting (less 10000 words)
        /// </summary>
        /// <param name="expectedString">normalized expected string</param>
        /// <param name="comparisonString">the normalized comparison string</param>
        /// <param name="deltaPercentage">how many word operations are allowed to change the input string into the comparison string
        /// expressed as a percentage of words. Example: if input string contains 200 words, deltaPercantage is 5, 10 edit operations would be allowed</param>
        /// <param name="absoluteAllowedErrorCount">an absolute lower limit on the number of allowed errors</param>
        /// <remarks>The actual number of allowed errors is the higher number of the relative or the absolute counts.</remarks>
        static public void AssertStringWordEditPercentage(string expectedString, string comparisonString, int deltaPercentage, int absoluteAllowedErrorCount = 1)
        {
            string[] wordsExpected = expectedString.Split(' ');

            int allowedEdits = Math.Max(deltaPercentage * wordsExpected.Length / 100, absoluteAllowedErrorCount);
            AssertStringWordEditCount(expectedString, comparisonString, allowedEdits);
        }

        public static bool IsWithinStringWordEditPercentage(string expectedString, string comparisonString, int deltaPercentage = 10, int absoluteAllowedErrorCount = 1)
        {
            string[] wordsExpected = expectedString.Split(' ');

            int allowedEdits = Math.Max(deltaPercentage * wordsExpected.Length / 100, absoluteAllowedErrorCount);
            return GetStringWordEditCount(expectedString, comparisonString) <= allowedEdits;
        }

        public static int GetStringWordEditCount(string expectedString, string comparisonString)
        {
            // Using standard implementation for Levenshtein distance. Caveat: not optimized for memory consumption
            //   but sufficient for our test case and the string length we are expecting (less 10000 words)
            // 
            // expectedString - normalized expected string
            // comparisonString - the normalized comparison string
            // allowedEdits - number of word-edits which are tolerated

            String[] wordsExpected = expectedString.Split(' ');
            String[] wordsComparison = comparisonString.Split(' ');

            const int maxWordsExpected = 10000;  // just an arbitrary bound for this operation
                                                 // reconsider memory usage (or alternative algorithm) if you need to increase this significantly
            Assert.IsTrue((wordsExpected.Length < maxWordsExpected) && (wordsExpected.Length > 0), $"number of words in expectedString out of bounds: '{wordsExpected.Length}'");
            Assert.IsTrue((wordsComparison.Length < maxWordsExpected) && (wordsComparison.Length > 0), $"number of words in wordsComparison out of bounds: '{wordsComparison.Length}'");

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

            return matrix[wordsExpected.Length][wordsComparison.Length];
        }

        static public void AssertStringWordEditCount(string expectedString, string comparisonString, int allowedEdits)
        {
            int edits = GetStringWordEditCount(expectedString, comparisonString);
            Assert.IsTrue(
                edits <= allowedEdits,
                $"Number of edit operations '{edits}' exceeding allowed edits '{allowedEdits}'\ninput:   '{expectedString}'\ncompare: '{comparisonString}'\n");
        }

        static public float LevenshteinRatio(string s, string t)
        {
            int n = s.Length;
            int m = t.Length;
            float lensum = (float)(m + n);
            int[,] d = new int[n + 1, m + 1];

            if (n == 0)
            {
                return m;
            }

            if (m == 0)
            {
                return n;
            }

            for (int i = 0; i <= n; i++)
                d[i, 0] = i;
            for (int j = 0; j <= m; j++)
                d[0, j] = j;

            for (int j = 1; j <= m; j++)
                for (int i = 1; i <= n; i++)
                    if (s[i - 1] == t[j - 1])
                        d[i, j] = d[i - 1, j - 1];  //no operation
                    else
                        d[i, j] = Math.Min(Math.Min(
                            d[i - 1, j] + 1,    //a deletion
                            d[i, j - 1] + 1),   //an insertion
                            d[i - 1, j - 1] + 1 //a substitution
                            );
            var distance = d[n, m];
            var ratio = (lensum - distance) / lensum;
            return ratio;
        }

        public static void AssertDetailedResult(SpeechRecognitionResult result)
        {
            var bestResults = result.Best().ToArray();
            Assert.IsTrue(bestResults.Length > 0);
            var detailedRecognitionText = bestResults[0].Text;
            var detailedRecognitionNormalizedForm = bestResults[0].NormalizedForm;
            var detailedRecognitionLexicalForm = bestResults[0].LexicalForm;
            var detailedRecognitionMaskedForm = bestResults[0].MaskedNormalizedForm;

            Assert.IsTrue(detailedRecognitionText.Length > 0);
            Assert.IsTrue(detailedRecognitionNormalizedForm.Length > 0);
            Assert.IsTrue(detailedRecognitionLexicalForm.Length > 0);
            Assert.IsTrue(detailedRecognitionMaskedForm.Length > 0);
        }

        public static void WarnIfContains(string result, string substring)
        {
            if (result.Contains(substring))
            {
                Console.WriteLine($"Warning: '{result}' does not contain expected substring '{substring}'");
            }
        }
        public static void WarnIfNotContains(string result, string substring)
        {
            if (result.Contains(substring))
            {
                Console.WriteLine($"Warning: '{result}' is missing expected substring '{substring}'");
            }
        }
    }
}
