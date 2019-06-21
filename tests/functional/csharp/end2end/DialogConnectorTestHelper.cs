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
    sealed class DialogConnectorTestsHelper
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

        public DialogConnectorTestsHelper()
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

        public async Task CompleteListenOnceAsync(DialogConnector dialogConnector)
        {
            string activityReceived = null;
            string canceled = string.Empty;

            dialogConnector.ActivityReceived += (s, e) =>
            {
                ActivityReceivedEventCounter(s, e);
                activityReceived = e.Activity;
                taskCompletionSource.TrySetResult(0);
            };
            dialogConnector.Canceled += (s, e) =>
            {
                CanceledEventCounter(s, e);
                canceled = e.ErrorDetails;
                taskCompletionSource.TrySetResult(0);
            };

            await dialogConnector.ConnectAsync().ConfigureAwait(false);

            await dialogConnector.ListenOnceAsync().ConfigureAwait(false);

            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));

            Assert.IsTrue(SessionStoppedEventCount == 1 || ErrorEventCount == 1 || ActivityReceivedEventCount == 1);
            if (!string.IsNullOrEmpty(canceled))
            {
                Assert.Fail($"Dialog interaction canceled: {canceled}");
            }

            Assert.AreNotEqual(activityReceived, string.Empty);
        }

        public async Task CompleteSendActivity(DialogConnector dialogConnector)
        {
            string activityReceived = null;
            string canceled = string.Empty;
            bool audioPresent = false;
            var buffer = new byte[800];
            uint totalAudioBytes = 0;

            SubscribeToCounterEventHandlers(dialogConnector);

            dialogConnector.ActivityReceived += (s, e) =>
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

            await dialogConnector.SendActivityAsync(speakActivity);

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

        public static DialogConnector TrackSessionId(DialogConnector dialogConnector)
        {
            dialogConnector.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return dialogConnector;
        }

        public void SubscribeToCounterEventHandlers(DialogConnector dialogConnector)
        {
            dialogConnector.Recognizing += RecognizingEventCounter;
            dialogConnector.Recognized += RecognizedEventCounter;
            dialogConnector.Canceled += CanceledEventCounter;
            dialogConnector.SessionStarted += SessionStartedEventCounter;
            dialogConnector.SessionStopped += SessionStoppedEventCounter;
            dialogConnector.ActivityReceived += ActivityReceivedEventCounter;
        }

        public void UnsubscribeFromCounterEventHandlers(DialogConnector dialogConnector)
        {
            dialogConnector.Recognizing -= RecognizingEventCounter;
            dialogConnector.Recognized -= RecognizedEventCounter;
            dialogConnector.Canceled -= CanceledEventCounter;
            dialogConnector.SessionStarted -= SessionStartedEventCounter;
            dialogConnector.SessionStopped -= SessionStoppedEventCounter;
            dialogConnector.ActivityReceived -= ActivityReceivedEventCounter;
        }
    }
}
