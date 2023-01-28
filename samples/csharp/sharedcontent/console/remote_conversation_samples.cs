//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using Azure;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.RemoteConversation;
using Microsoft.CognitiveServices.Speech.Transcription;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class RemoteConversationSamples
    {
        public static async Task RemoteConversationWithFileAsync()
        {
            string key = "YourSubscriptionKey";
            string region = "YourServiceRegion";

            // Upload the audio to the service
            string meetingId = await UploadAudioAndStartRemoteTranscription(key, region);

            // Poll the service 
            TestRemoteTranscription(key, region, meetingId);
        }

        private static void TestRemoteTranscription(string key, string region, string meetingId)
        {
            SpeechConfig config = SpeechConfig.FromSubscription(key, region);
            RemoteConversationTranscriptionClient client = new RemoteConversationTranscriptionClient(config);
            RemoteConversationTranscriptionOperation operation = new RemoteConversationTranscriptionOperation(meetingId, client);
            WaitForCompletion(operation).Wait();
        }

        private static async Task WaitForCompletion(RemoteConversationTranscriptionOperation operation)
        {
            while (!operation.HasCompleted)
            {
                Thread.Sleep(TimeSpan.FromSeconds(10));
                Response response = await operation.UpdateStatusAsync();
                Console.WriteLine(response.Status);
            }

            await operation.WaitForCompletionAsync(TimeSpan.FromSeconds(10), CancellationToken.None);

            Console.WriteLine($"Id = {operation.Value.Id}");
            var val = operation.Value.ConversationTranscriptionResults;
            foreach (var item in val)
            {
                Console.WriteLine($"{item.Text}, {item.ResultId}, {item.Reason}, {item.UserId}, {item.OffsetInTicks}, {item.Duration}");
                Console.WriteLine($"{item.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult)}");
                Console.WriteLine();
            }
        }


        private static PullAudioInputStreamCallback OpenWavFile(string filename, out AudioStreamFormat format)
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(filename));

            // Tag "RIFF"
            char[] data = new char[4];
            reader.Read(data, 0, 4);

            // Chunk size
            long fileSize = reader.ReadInt32();

            // Subchunk, Wave Header
            // Subchunk, Format
            // Tag: "WAVE"
            reader.Read(data, 0, 4);

            // Tag: "fmt"
            reader.Read(data, 0, 4);
            // chunk format size
            var formatSize = reader.ReadInt32();
            var unusedFormatTag = reader.ReadUInt16();
            var channels = reader.ReadUInt16();
            var samplesPerSecond = reader.ReadUInt32();
            var unusedAvgBytesPerSec = reader.ReadUInt32();
            var unusedBlockAlign = reader.ReadUInt16();
            var bitsPerSample = reader.ReadUInt16();

            // Until now we have read 16 bytes into format, the rest is cbSize and is ignored for now.
            if (formatSize > 16)
                reader.ReadBytes((int)(formatSize - 16));

            bool foundDataChunk = false;
            while (!foundDataChunk)
            {
                reader.Read(data, 0, 4);
                var chunkSize = reader.ReadInt32();
                if ((data[0] == 'd') && (data[1] == 'a') && (data[2] == 't') && (data[3] == 'a'))
                {
                    foundDataChunk = true;
                    break;
                }
                reader.ReadBytes(chunkSize);
            }
            if (!foundDataChunk)
            {
                throw new System.ApplicationException("${filename} does not contain a data chunk!");
            }

            // now, we have the format in the format parameter and the
            // reader set to the start of the body, i.e., the raw sample data
            format = AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, (byte)bitsPerSample, (byte)channels);
            return new BinaryAudioStreamReader(reader);
        }

        private static ConversationTranscriber TrackSessionId(ConversationTranscriber recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }

        private static async Task<string> UploadAudioAndStartRemoteTranscription(string key, string region)
        {
            AudioStreamFormat audioStreamFormat;

            var config = SpeechConfig.FromSubscription(key, region);
            config.SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
            config.SetServiceProperty("transcriptionMode", "RealTimeAndAsync", ServicePropertyChannel.UriQueryParameter);
            var waveFilePullStream = OpenWavFile(@"katiesteve.wav", out audioStreamFormat);
            var audioInput = AudioConfig.FromStreamInput(AudioInputStream.CreatePullStream(waveFilePullStream, audioStreamFormat));

            var meetingId = Guid.NewGuid().ToString();
            using (var conversation = await Conversation.CreateConversationAsync(config, meetingId))
            {
                using (var conversationTranscriber = TrackSessionId(new ConversationTranscriber(audioInput)))
                {
                    await conversationTranscriber.JoinConversationAsync(conversation);

                    await conversation.AddParticipantAsync("OneUserByUserId");

                    var user = User.FromUserId("CreateUserFromId and then add it");
                    await conversation.AddParticipantAsync(user);

                    var result = await GetRecognizerResult(conversationTranscriber, meetingId);
                }
            }
            return meetingId;
        }

        private static async Task<List<string>> GetRecognizerResult(ConversationTranscriber recognizer, string conversationId)
        {
            List<string> recognizedText = new List<string>();
            recognizer.Transcribed += (s, e) =>
            {
                if (e.Result.Text.Length > 0)
                {
                    recognizedText.Add(e.Result.Text);
                }
            };


            await CompleteContinuousRecognition(recognizer, conversationId);

            recognizer.Dispose();
            return recognizedText;
        }

        private static async Task CompleteContinuousRecognition(ConversationTranscriber recognizer, string conversationId)
        {
            TaskCompletionSource<int> m_taskCompletionSource = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            recognizer.SessionStopped += (s, e) =>
            {
                m_taskCompletionSource.TrySetResult(0);
            };
            string canceled = string.Empty;

            recognizer.Canceled += (s, e) =>
            {
                canceled = e.ErrorDetails;
                if (e.Reason == CancellationReason.Error)
                {
                    m_taskCompletionSource.TrySetResult(0);
                }
            };

            await recognizer.StartTranscribingAsync().ConfigureAwait(false);
            await Task.WhenAny(m_taskCompletionSource.Task, Task.Delay(TimeSpan.FromSeconds(10)));
            await recognizer.StopTranscribingAsync().ConfigureAwait(false);

        }
    }
}
