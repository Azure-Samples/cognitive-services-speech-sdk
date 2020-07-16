//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Dialog;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class DialogServiceConnectorTestsHelper
    {
        public int ErrorEventCount { get; set; }

        public int RecognizingEventCount { get; set; }

        public int RecognizedEventCount { get; set; }

        public int SessionStartedEventCount { get; set; }

        public int SessionStoppedEventCount { get; set; }

        public int ActivityReceivedEventCount { get; set; }

        public int ConnectedEventCount { get; set; }

        public int DisconnectedEventCount { get; set; }

        private Connection connectorConnection;
        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromMinutes(6);

        public DialogServiceConnectorTestsHelper()
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

        public async Task CompleteListenOnceAsync(DialogServiceConnector dialogServiceConnector)
        {
            string activityReceived = null;
            string canceled = string.Empty;

            connectorConnection = Connection.FromDialogServiceConnector(dialogServiceConnector);
            Assert.IsTrue(connectorConnection != null);
            connectorConnection.Connected += (_, e) => this.ConnectedEventCounter(null, e);

            dialogServiceConnector.SessionStarted += (_, e) => this.SessionStartedEventCounter(null, e);
            dialogServiceConnector.SessionStopped += (_, e) => this.SessionStoppedEventCounter(null, e);
            dialogServiceConnector.ActivityReceived += (s, e) =>
            {
                ActivityReceivedEventCounter(s, e);
                activityReceived = e.Activity;
                taskCompletionSource.TrySetResult(0);
            };
            dialogServiceConnector.Canceled += (s, e) =>
            {
                CanceledEventCounter(s, e);
                canceled = e.ErrorDetails;
                taskCompletionSource.TrySetResult(0);
            };

            await dialogServiceConnector.ConnectAsync().ConfigureAwait(false);

            await dialogServiceConnector.ListenOnceAsync().ConfigureAwait(false);

            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));

            Assert.IsTrue(SessionStoppedEventCount == 1 || ErrorEventCount == 1 || ActivityReceivedEventCount == 1,
                String.Format("Expected stop, error, or activity; actual counts: {0} | {1} | {2}",
                    this.SessionStoppedEventCount,
                    this.ErrorEventCount,
                    this.ActivityReceivedEventCount));
            Assert.IsTrue(this.SessionStartedEventCount == 0 ^ this.ConnectedEventCount > 0,
                String.Format("Expected 0 sessions or >0 connections. Actual: {0}, {1}",
                    this.SessionStartedEventCount,
                    this.ConnectedEventCount));
            
            if (!string.IsNullOrEmpty(canceled))
            {
                Assert.Fail($"Dialog interaction canceled: {canceled}");
            }

            Assert.AreNotEqual(activityReceived, string.Empty);
        }

        public async Task CompleteSendActivity(DialogServiceConnector dialogServiceConnector)
        {
            string activityReceived = null;
            string canceled = string.Empty;
            bool audioPresent = false;
            var buffer = new byte[800];
            uint totalAudioBytes = 0;

            SubscribeToCounterEventHandlers(dialogServiceConnector);

            dialogServiceConnector.ActivityReceived += (s, e) =>
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

            string speakActivity = @"{""type"":""message"",""text"":""This is reply text"", ""speak"":""This is a spoken reply"" }";

            await dialogServiceConnector.SendActivityAsync(speakActivity);

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

        public static DialogServiceConnector TrackSessionId(DialogServiceConnector dialogServiceConnector)
        {
            dialogServiceConnector.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return dialogServiceConnector;
        }

        public void SubscribeToCounterEventHandlers(DialogServiceConnector dialogServiceConnector)
        {
            dialogServiceConnector.Recognizing += RecognizingEventCounter;
            dialogServiceConnector.Recognized += RecognizedEventCounter;
            dialogServiceConnector.Canceled += CanceledEventCounter;
            dialogServiceConnector.SessionStarted += SessionStartedEventCounter;
            dialogServiceConnector.SessionStopped += SessionStoppedEventCounter;
            dialogServiceConnector.ActivityReceived += ActivityReceivedEventCounter;
        }

        public void UnsubscribeFromCounterEventHandlers(DialogServiceConnector dialogServiceConnector)
        {
            dialogServiceConnector.Recognizing -= RecognizingEventCounter;
            dialogServiceConnector.Recognized -= RecognizedEventCounter;
            dialogServiceConnector.Canceled -= CanceledEventCounter;
            dialogServiceConnector.SessionStarted -= SessionStartedEventCounter;
            dialogServiceConnector.SessionStopped -= SessionStoppedEventCounter;
            dialogServiceConnector.ActivityReceived -= ActivityReceivedEventCounter;
        }
    }
}
