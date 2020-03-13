//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.CognitiveServices.Speech.Transcription;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    sealed class ConversationTranscriberTestsHelper
    {
        private TaskCompletionSource<int> taskCompletionSource;
        private TimeSpan timeout = TimeSpan.FromSeconds(10);

        public ConversationTranscriberTestsHelper()
        {
            taskCompletionSource = new TaskCompletionSource<int>();
        }

        public async Task CompleteContinuousRecognition(ConversationTranscriber recognizer, string conversationId)
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
                Assert.Fail($"Recognition Canceled(meetingID={conversationId}): {canceled}");
            }
        }
        public bool FindTheRef(List<string> phrases, string reference)
        {
            foreach( string phrase in phrases)
            {
                var candidate = SpeechRecognitionTestsHelper.Normalize(phrase);
                reference = SpeechRecognitionTestsHelper.Normalize(reference);
                return candidate == reference;
            }
            return false;
        }

        public async Task<List<string>> GetRecognizerResult(ConversationTranscriber recognizer, string conversationId)
        {
            List<string> recognizedText = new List<string>();
            recognizer.Transcribed += (s, e) =>
            {
                
                Console.WriteLine($"Received result '{e.ToString()}' 'e.Result.UserId' 'e.Result.UtteranceId'");
                if (e.Result.Text.Length > 0)
                {
                    recognizedText.Add(e.Result.Text);
                }
            };

            recognizer.Transcribing += (s, e) =>
            {
                Console.WriteLine($"Received result '{e.ToString()}'");
            };

            await CompleteContinuousRecognition(recognizer, conversationId);

            recognizer.Dispose();
            return recognizedText;
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
