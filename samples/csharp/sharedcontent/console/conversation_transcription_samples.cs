//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Transcription;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class ConversationTranscriptionSamples
    {
        // Continuous conversation transcription from microphone with speaker diarization.
        // The ConversationTranscriber automatically identifies different speakers and assigns
        // a SpeakerId (e.g. "Guest-1", "Guest-2") to each recognized segment.
        public static async Task TranscribeConversationFromMicrophoneAsync()
        {
            // <conversationFromMicrophone>
            // Creates an instance of a speech config with specified subscription key and region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            using (var audioConfig = AudioConfig.FromDefaultMicrophoneInput())
            using (var transcriber = new ConversationTranscriber(config, audioConfig))
            {
                // Subscribes to events.
                transcriber.Transcribing += (s, e) =>
                {
                    Console.WriteLine($"TRANSCRIBING: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                };

                transcriber.Transcribed += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"TRANSCRIBED: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be transcribed.");
                    }
                };

                transcriber.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");
                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                    stopRecognition.TrySetResult(0);
                };

                transcriber.SessionStarted += (s, e) =>
                {
                    Console.WriteLine($"\nSession started event. SessionId={e.SessionId}");
                };

                transcriber.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"\nSession stopped event. SessionId={e.SessionId}");
                    stopRecognition.TrySetResult(0);
                };

                Console.WriteLine("Speak into your microphone. Multiple speakers will be differentiated automatically.");
                Console.WriteLine("Press any key to stop...\n");

                // Starts transcribing.
                await transcriber.StartTranscribingAsync().ConfigureAwait(false);

                // Waits for completion. Use Task.WaitAny to keep the task rooted.
                Task.WaitAny(new Task[] { stopRecognition.Task, Task.Run(() => Console.ReadKey()) });

                // Stops transcribing.
                await transcriber.StopTranscribingAsync().ConfigureAwait(false);
            }
            // </conversationFromMicrophone>
        }

        // Conversation transcription from a WAV file with speaker diarization.
        // Demonstrates processing a pre-recorded multi-speaker audio file.
        public static async Task TranscribeConversationFromFileAsync()
        {
            // <conversationFromFile>
            // Creates an instance of a speech config with specified subscription key and region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create an audio config from a WAV file.
            // Replace with your own multi-speaker audio file. The file should be 16-bit PCM WAV format.
            // For best diarization results, use 16kHz or higher sample rate.
            using (var audioConfig = AudioConfig.FromWavFileInput("katiesteve.wav"))
            using (var transcriber = new ConversationTranscriber(config, audioConfig))
            {
                transcriber.Transcribing += (s, e) =>
                {
                    Console.WriteLine($"TRANSCRIBING: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                };

                transcriber.Transcribed += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"TRANSCRIBED: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be transcribed.");
                    }
                };

                transcriber.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");
                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                    }
                    stopRecognition.TrySetResult(0);
                };

                transcriber.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"\nSession stopped.");
                    stopRecognition.TrySetResult(0);
                };

                Console.WriteLine("Transcribing conversation from file with speaker diarization...\n");

                await transcriber.StartTranscribingAsync().ConfigureAwait(false);
                Task.WaitAny(new[] { stopRecognition.Task });
                await transcriber.StopTranscribingAsync().ConfigureAwait(false);
            }
            // </conversationFromFile>
        }

        // Conversation transcription from microphone with a specified language.
        // Demonstrates how to set the speech recognition language for conversation transcription.
        public static async Task TranscribeConversationWithLanguageAsync()
        {
            // <conversationWithLanguage>
            // Creates an instance of a speech config with specified subscription key and region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Set the speech recognition language. Default is "en-US".
            // Change to your target language, e.g., "zh-CN", "de-DE", "fr-FR", "ja-JP", etc.
            config.SpeechRecognitionLanguage = "en-US";

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            using (var audioConfig = AudioConfig.FromDefaultMicrophoneInput())
            using (var transcriber = new ConversationTranscriber(config, audioConfig))
            {
                transcriber.Transcribing += (s, e) =>
                {
                    Console.WriteLine($"TRANSCRIBING: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                };

                transcriber.Transcribed += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"TRANSCRIBED: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be transcribed.");
                    }
                };

                transcriber.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");
                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                    }
                    stopRecognition.TrySetResult(0);
                };

                transcriber.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"\nSession stopped.");
                    stopRecognition.TrySetResult(0);
                };

                Console.WriteLine($"Transcribing conversation in {config.SpeechRecognitionLanguage} with speaker diarization...");
                Console.WriteLine("Press any key to stop...\n");

                await transcriber.StartTranscribingAsync().ConfigureAwait(false);
                Task.WaitAny(new Task[] { stopRecognition.Task, Task.Run(() => Console.ReadKey()) });
                await transcriber.StopTranscribingAsync().ConfigureAwait(false);
            }
            // </conversationWithLanguage>
        }

        // Conversation transcription using PushAudioInputStream.
        // Demonstrates how to feed audio data programmatically from any source
        // (e.g., a custom audio capture device, a network stream, etc.)
        // while still getting speaker diarization.
        public static async Task TranscribeConversationWithPushStreamAsync()
        {
            // <conversationWithPushStream>
            // Creates an instance of a speech config with specified subscription key and region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create a push stream with default audio format (16kHz, 16-bit, mono PCM).
            using (var pushStream = AudioInputStream.CreatePushStream())
            using (var audioConfig = AudioConfig.FromStreamInput(pushStream))
            using (var transcriber = new ConversationTranscriber(config, audioConfig))
            {
                transcriber.Transcribing += (s, e) =>
                {
                    Console.WriteLine($"TRANSCRIBING: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                };

                transcriber.Transcribed += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"TRANSCRIBED: Text={e.Result.Text} SpeakerId={e.Result.SpeakerId}");
                    }
                    else if (e.Result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be transcribed.");
                    }
                };

                transcriber.Canceled += (s, e) =>
                {
                    Console.WriteLine($"CANCELED: Reason={e.Reason}");
                    if (e.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                    }
                    stopRecognition.TrySetResult(0);
                };

                transcriber.SessionStopped += (s, e) =>
                {
                    Console.WriteLine($"\nSession stopped.");
                    stopRecognition.TrySetResult(0);
                };

                await transcriber.StartTranscribingAsync().ConfigureAwait(false);

                // Push audio data from a WAV file.
                // In a real application, replace this with audio from a capture device, network stream, etc.
                Console.WriteLine("Pushing audio data from file into conversation transcriber...\n");

                using (BinaryAudioStreamReader reader = Helper.CreateWavReader(@"whatstheweatherlike.wav"))
                {
                    byte[] buffer = new byte[1000];
                    while (true)
                    {
                        var readSamples = reader.Read(buffer, (uint)buffer.Length);
                        if (readSamples == 0)
                        {
                            break;
                        }
                        pushStream.Write(buffer, readSamples);
                    }
                }

                // Signal that no more audio will be sent.
                pushStream.Close();

                // Wait for transcription to complete.
                Task.WaitAny(new[] { stopRecognition.Task });
                await transcriber.StopTranscribingAsync().ConfigureAwait(false);
            }
            // </conversationWithPushStream>
        }
    }
}
