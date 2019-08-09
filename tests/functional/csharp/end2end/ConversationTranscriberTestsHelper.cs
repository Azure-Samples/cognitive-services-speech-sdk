//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using MicrosoftSpeechSDKSamples;
using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using Microsoft.CognitiveServices.Speech.Conversation;

    sealed class ConversationTranscriberTestsHelper
    {
        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromSeconds(10);

        public ConversationTranscriberTestsHelper()
        {
            taskCompletionSource = new TaskCompletionSource<int>();
        }

        public async Task CompleteContinuousRecognition(ConversationTranscriber recognizer)
        {
            recognizer.SessionStopped += (s, e) =>
            {
                taskCompletionSource.TrySetResult(0);
            };
            string canceled = string.Empty;

            recognizer.Canceled += (s, e) => {
                canceled = e.ErrorDetails;
                if (e.Reason == CancellationReason.Error)
                {
                    taskCompletionSource.TrySetResult(0);
                }
            };

            await recognizer.StartTranscribingAsync().ConfigureAwait(false);
            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(timeout));
            await recognizer.StopTranscribingAsync().ConfigureAwait(false);

            if (!string.IsNullOrEmpty(canceled))
            {
                Assert.Fail($"Recognition Canceled(meetingID={recognizer.ConversationId}): {canceled}");
            }
        }

        public async Task<string> GetFirstRecognizerResult(ConversationTranscriber recognizer)
        {
            List<string> recognizedText = new List<string>();
            recognizer.Recognized += (s, e) =>
            {
                Console.WriteLine($"Received result '{e.ToString()}'");
                if (e.Result.Text.Length > 0)
                {
                    recognizedText.Add(e.Result.Text);
                }
            };

            recognizer.Recognizing += (s, e) =>
            {
                Console.WriteLine($"Received result '{e.ToString()}'");
            };

            await CompleteContinuousRecognition(recognizer);

            recognizer.Dispose();
            return recognizedText.Count > 0 ? recognizedText[0] : string.Empty;
        }

        public static ConversationTranscriber TrackSessionId(ConversationTranscriber recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }
    }
}
