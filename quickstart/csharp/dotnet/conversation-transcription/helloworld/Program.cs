//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;
using System.Net.Http;
using System.Runtime.Serialization;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Transcription;
using Newtonsoft.Json;

namespace helloworld
{
    class Program
    {
        public static async Task TranscribeConversationsAsync(string subscriptionKey, string endpoint)
        {
            var config = SpeechConfig.FromEndpoint(new Uri(endpoint), subscriptionKey);
            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create an audio stream from a wav file using 16-bit PCM audio format. Replace with your own audio file.
            // Different PCM formats (1ch, stereo, 4ch, 8ch) with different sampling rates (8KHz, 16KHz, 48KHz) 
            // are supported. Default format is single channel 16KHz.
            using (var audioInput = AudioStreamReader.OpenWavFile("katiesteve_mono.wav"))
            // To get audio from microphone input, use the following line instead:
            //using (var audioInput = AudioConfig.FromDefaultMicrophoneInput())
            {
                // Create a conversation transcriber using audio stream input
                using (var conversationTranscriber = new ConversationTranscriber(config, audioInput))
                {
                    // Subscribe to events
                    conversationTranscriber.Transcribing += (s, e) =>
                    {
                        Console.WriteLine($"TRANSCRIBING: Text={e.Result.Text}");
                    };

                    conversationTranscriber.Transcribed += (s, e) =>
                    {
                        if (e.Result.Reason == ResultReason.RecognizedSpeech)
                        {
                            Console.WriteLine($"TRANSCRIBED: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                        }
                        else if (e.Result.Reason == ResultReason.NoMatch)
                        {
                            Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                        }
                    };

                    conversationTranscriber.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");

                        if (e.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            stopRecognition.TrySetResult(0);
                        }
                    };

                    conversationTranscriber.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine($"\nSession started event. SessionId={e.SessionId}");
                    };

                    conversationTranscriber.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine($"\nSession stopped event. SessionId={e.SessionId}");
                        Console.WriteLine("\nStop recognition.");
                        stopRecognition.TrySetResult(0);
                    };

                    // Starts transcribing of the conversation. Uses StopTranscribingAsync() to stop transcribing.
                    await conversationTranscriber.StartTranscribingAsync().ConfigureAwait(false);

                    // Waits for completion.
                    // Use Task.WaitAny to keep the task rooted.
                    Task.WaitAny(new[] { stopRecognition.Task });

                    // Stop transcribing the conversation.
                    await conversationTranscriber.StopTranscribingAsync().ConfigureAwait(false);
                }
            }
        }

        static async Task Main()
        {
            var subscriptionKey = "YourSubscriptionKey";
            var endpoint = "https://YourServiceRegion.api.cognitive.microsoft.com";

            await TranscribeConversationsAsync(subscriptionKey, endpoint);
            Console.WriteLine("Please press <Return> to continue.");
            Console.ReadLine();
        }
    }
}
