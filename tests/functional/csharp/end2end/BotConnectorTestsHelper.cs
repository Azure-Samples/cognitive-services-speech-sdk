//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.Threading;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Dialog;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class BotConnectorTestsHelper
    {
        public int ErrorEventCount { get; set; }

        public int RecognizingEventCount { get; set; }

        public int RecognizedEventCount { get; set; }

        public int SessionStartedEventCount { get; set; }

        public int SessionStoppedEventCount { get; set; }

        public int ActivityReceivedEventCount { get; set; }

        public int ConnectedEventCount { get; set; }

        public int DisconnectedEventCount { get; set; }

        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromMinutes(6);

        public BotConnectorTestsHelper()
        {
            ErrorEventCount = 0;
            RecognizingEventCount = 0;
            RecognizedEventCount = 0;
            SessionStartedEventCount = 0;
            SessionStoppedEventCount = 0;
            ActivityReceivedEventCount = 0;
            ConnectedEventCount = 0;
            DisconnectedEventCount = 0;
            taskCompletionSource = new TaskCompletionSource<int>();
        }

        public async Task CompleteListenOnceAsync(SpeechBotConnector botConnector)
        {
            string activityReceived = null;
            string canceled = string.Empty;

            botConnector.ActivityReceived += (s, e) =>
            {
                ActivityReceivedEventCounter(s, e);
                activityReceived = e.Activity;
                taskCompletionSource.TrySetResult(0);
            };
            botConnector.Canceled += (s, e) =>
            {
                CanceledEventCounter(s, e);
                canceled = e.ErrorDetails;
                taskCompletionSource.TrySetResult(0);
            };

            await botConnector.ConnectAsync().ConfigureAwait(false);

            await botConnector.ListenOnceAsync().ConfigureAwait(false);

            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));

            Assert.IsTrue(SessionStoppedEventCount == 1 || ErrorEventCount == 1 || ActivityReceivedEventCount == 1);
            if (!string.IsNullOrEmpty(canceled))
            {
                Assert.Fail($"Bot interaction canceled: {canceled}");
            }

            Assert.AreNotEqual(activityReceived, string.Empty);
        }

        public async Task CompleteSendActivity(SpeechBotConnector botConnector)
        {
            string activityReceived = null;
            string canceled = string.Empty;
            bool audioPresent = false;
            var buffer = new byte[800];
            uint totalAudioBytes = 0;

            SubscribeToCounterEventHandlers(botConnector);

            botConnector.ActivityReceived += (s, e) =>
            {
                ActivityReceivedEventCounter(s, e);
                activityReceived = e.Activity;
                audioPresent = e.HasAudio;
                var audio = e.Audio;
                if(audioPresent)
                {
                    uint bytesRead = 0;
                    while ((bytesRead = audio.Read(buffer)) > 0)
                    {
                        totalAudioBytes += bytesRead;
                        bytesRead = audio.Read(buffer);
                    }
                }
                taskCompletionSource.TrySetResult(0);
            };

            string speakActivity = @"{""type"":""message"",""text"":""yo"", ""speak"":""hello"" }";

            await botConnector.SendActivityAsync(speakActivity);

            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));

            Assert.IsTrue(ActivityReceivedEventCount > 0);
            Assert.IsTrue(audioPresent);
            Assert.IsTrue(totalAudioBytes > 0);
        }

        private void RecognizingEventCounter(object sender, SpeechRecognitionEventArgs e)
        {
            RecognizingEventCount++;
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

        private void ActivityReceivedEventCounter(object sender, ActivityReceivedEventArgs e)
        {
            ActivityReceivedEventCount++;
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

        public static SpeechBotConnector TrackSessionId(SpeechBotConnector botConnector)
        {
            botConnector.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return botConnector;
        }

        public void SubscribeToCounterEventHandlers(SpeechBotConnector botConnector)
        {
            botConnector.Recognizing += RecognizingEventCounter;
            botConnector.Recognized += RecognizedEventCounter;
            botConnector.Canceled += CanceledEventCounter;
            botConnector.SessionStarted += SessionStartedEventCounter;
            botConnector.SessionStopped += SessionStoppedEventCounter;
            botConnector.ActivityReceived += ActivityReceivedEventCounter;
        }

        public void UnsubscribeFromCounterEventHandlers(SpeechBotConnector botConnector)
        {
            botConnector.Recognizing -= RecognizingEventCounter;
            botConnector.Recognized -= RecognizedEventCounter;
            botConnector.Canceled -= CanceledEventCounter;
            botConnector.SessionStarted -= SessionStartedEventCounter;
            botConnector.SessionStopped -= SessionStoppedEventCounter;
            botConnector.ActivityReceived -= ActivityReceivedEventCounter;
        }
    }
}
