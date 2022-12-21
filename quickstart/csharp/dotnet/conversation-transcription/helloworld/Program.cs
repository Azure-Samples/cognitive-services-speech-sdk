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
using Microsoft.CognitiveServices.Speech.Transcription;
using Newtonsoft.Json;

namespace helloworld
{
    class Program
    {
        /// <summary>
        /// Class which defines VoiceSignature as specified under https://aka.ms/cts/signaturegenservice.
        /// </summary>
        [DataContract]
        internal class VoiceSignature
        {
            [DataMember]
            public string Status { get; private set; }

            [DataMember]
            public VoiceSignatureData Signature { get; private set; }

            [DataMember]
            public string Transcription { get; private set; }
        }

        /// <summary>
        /// Class which defines VoiceSignatureData which is used when creating/adding participants
        /// </summary>
        [DataContract]
        internal class VoiceSignatureData
        {
            internal VoiceSignatureData()
            { }

            internal VoiceSignatureData(int version, string tag, string data)
            {
                this.Version = version;
                this.Tag = tag;
                this.Data = data;
            }

            [DataMember]
            public int Version { get; private set; }

            [DataMember]
            public string Tag { get; private set; }

            [DataMember]
            public string Data { get; private set; }
        }

        private static string voiceSignatureUser1;
        private static string voiceSignatureUser2;

        private static async Task<VoiceSignature> CreateVoiceSignatureFromVoiceSample(string voiceSample, string subscriptionKey, string region)
        {
            byte[] fileBytes = File.ReadAllBytes(voiceSample);
            var content = new ByteArrayContent(fileBytes);
            var client = new HttpClient();
            client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", subscriptionKey);
            var response = await client.PostAsync($"https://signature.{region}.cts.speech.microsoft.com/api/v1/Signature/GenerateVoiceSignatureFromByteArray", content);
            // A voice signature contains Version, Tag and Data key values from the Signature json structure from the Response body.
            // Voice signature format example: { "Version": <Numeric string or integer value>, "Tag": "string", "Data": "string" }
            var jsonData = await response.Content.ReadAsStringAsync();
            var result = JsonConvert.DeserializeObject<VoiceSignature>(jsonData);
            return result;
        }

        public static async Task TranscribeConversationsAsync(string conversationWaveFile, string subscriptionKey, string region)
        {
            var config = SpeechConfig.FromSubscription(subscriptionKey, region);
            config.SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create an audio stream from a wav file or from the default microphone if you want to stream live audio from the supported devices
            using (var audioInput = AudioStreamReader.OpenWavFile(conversationWaveFile))
            {
                var meetingID = Guid.NewGuid().ToString();
                using (var conversation = await Conversation.CreateConversationAsync(config, meetingID))
                {
                    // Create a conversation transcriber using audio stream input
                    using (var conversationTranscriber = new ConversationTranscriber(audioInput))
                    {
                        // Subscribe to events
                        conversationTranscriber.Transcribing += (s, e) =>
                        {
                            Console.WriteLine($"TRANSCRIBING: Text={e.Result.Text} SpeakerId={e.Result.UserId}");
                        };

                        conversationTranscriber.Transcribed += (s, e) =>
                        {
                            if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                Console.WriteLine($"TRANSCRIBED: Text={e.Result.Text} SpeakerId={e.Result.UserId}");
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

                        // Add participants to the conversation.
                        // Voice signature needs to be in the following format:
                        // { "Version": <Numeric value>, "Tag": "string", "Data": "string" }
                        var languageForUser1 = "en-US"; // For example "en-US"
                        var speakerA = Participant.From("User1", languageForUser1, voiceSignatureUser1);
                        var languageForUser2 = "en-US"; // For example "en-US"
                        var speakerB = Participant.From("User2", languageForUser2, voiceSignatureUser2);
                        await conversation.AddParticipantAsync(speakerA);
                        await conversation.AddParticipantAsync(speakerB);

                        // Join to the conversation.
                        await conversationTranscriber.JoinConversationAsync(conversation);

                        // Starts transcribing of the conversation. Uses StopTranscribingAsync() to stop transcribing when all participants leave.
                        await conversationTranscriber.StartTranscribingAsync().ConfigureAwait(false);

                        // Waits for completion.
                        // Use Task.WaitAny to keep the task rooted.
                        Task.WaitAny(new[] { stopRecognition.Task });

                        // Stop transcribing the conversation.
                        await conversationTranscriber.StopTranscribingAsync().ConfigureAwait(false);
                    }
                }
            }
        }

        static async Task Main()
        {
            var subscriptionKey = "YourSubscriptionKey";
            var serviceRegion = "YourServiceRegion";

            // The input audio wave format for voice signatures is 16-bit samples, 16 kHz sample rate, and a single channel (mono).
            // The recommended length for each sample is between thirty seconds and two minutes.
            var voiceSignatureWaveFileUser1 = "enrollment_audio_katie.wav";
            var voiceSignatureWaveFileUser2 = "enrollment_audio_steve.wav";

            // This sample expects a wavfile which is captured using a supported devices (8 channel, 16kHz, 16-bit PCM)
            // See https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-devices-sdk-microphone
            var conversationWaveFile = "katiesteve.wav";

            // Create voice signature for the user1 and convert it to json string
            var voiceSignature = CreateVoiceSignatureFromVoiceSample(voiceSignatureWaveFileUser1, subscriptionKey, serviceRegion);
            voiceSignatureUser1 = JsonConvert.SerializeObject(voiceSignature.Result.Signature);

            // Create voice signature for the user2 and convert it to json string
            voiceSignature = CreateVoiceSignatureFromVoiceSample(voiceSignatureWaveFileUser2, subscriptionKey, serviceRegion);
            voiceSignatureUser2 = JsonConvert.SerializeObject(voiceSignature.Result.Signature);

            await TranscribeConversationsAsync(conversationWaveFile, subscriptionKey, serviceRegion);
            Console.WriteLine("Please press <Return> to continue.");
            Console.ReadLine();
        }
    }
}
