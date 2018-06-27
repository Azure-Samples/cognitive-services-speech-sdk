//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
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
        private TimeSpan timeout = TimeSpan.FromSeconds(90);

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

            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }

        public async Task<bool> IsValidSimpleRecognizer(SpeechRecognizer speechRecognizer, string expectedRecognitionResult)
        {
            List<string> recognizedText = new List<string>();
            
            speechRecognizer.FinalResultReceived += (s, e) =>
            {
                if (e.Result.Text.Length > 0)
                {
                    recognizedText.Add(e.Result.Text);
                }
            };

            await CompleteContinuousRecognition(speechRecognizer);

            speechRecognizer.Dispose();
            if (recognizedText.Count > 0)
            {
                return AreResultsMatching(recognizedText[0], expectedRecognitionResult);
            }
            else
            {
                return false;
            }
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

        public bool AreResultsMatching(String actualText, String expectedText)
        {
            string plainActualText = Normalize(actualText);
            string plainExpectedText = Normalize(expectedText);

            if (plainActualText.Length <= plainExpectedText.Length)
            {
                return plainExpectedText.Contains(plainActualText);
            }
            else
            {
                return plainActualText.Contains(plainExpectedText);
            }
        }

        private string Normalize(string str)
        {
            str = str.ToLower();
            return Regex.Replace(str, "[^a-z0-9' ]+", "", RegexOptions.Compiled);
        }
    }
}
