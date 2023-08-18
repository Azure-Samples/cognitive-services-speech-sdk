//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <toplevel>
using Azure;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.RemoteMeeting;
using Microsoft.CognitiveServices.Speech.Transcription;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
// </toplevel>

namespace MicrosoftSpeechSDKSamples
{
    public class RemoteMeetingSamples
    {
        // This sample shows how to use real-time plus asynchronous
        // Meeting Transcriptions. This sample does not use voice
        // signatures. Talkers are differentiated as Guest 0 and Guest 1.
        // For more information, including how to use voice signatures, see
        // https://learn.microsoft.com/azure/cognitive-services/speech-service/how-to-async-meeting-transcription?pivots=programming-language-csharp
        public static async Task RemoteMeetingWithFileAsync()
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
            RemoteMeetingTranscriptionClient client = new RemoteMeetingTranscriptionClient(config);
            RemoteMeetingTranscriptionOperation operation = new RemoteMeetingTranscriptionOperation(meetingId, client);
            WaitForCompletion(operation).Wait();
        }

        private static async Task WaitForCompletion(RemoteMeetingTranscriptionOperation operation)
        {
            while (!operation.HasCompleted)
            {
                Thread.Sleep(TimeSpan.FromSeconds(10));
                Response response = await operation.UpdateStatusAsync();
                Console.WriteLine($"HTTP response status code: {response.Status}");
            }

            await operation.WaitForCompletionAsync(TimeSpan.FromSeconds(10), CancellationToken.None);

            Console.WriteLine($"Id = {operation.Value.Id}");
            var val = operation.Value.MeetingTranscriptionResults;
            foreach (var item in val)
            {
                Console.WriteLine($"{item.Text}, {item.ResultId}, {item.Reason}, {item.UserId}, {item.OffsetInTicks}, {item.Duration}");
                // To see raw JSON result:
                // Console.WriteLine($"{item.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult)}");
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

        private static MeetingTranscriber TrackSessionId(MeetingTranscriber recognizer)
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

            // Set this to differentiate talkers (Guest 0, Guest 1, ...)
            config.SetProperty("DifferentiateGuestSpeakers", "true");
    
            config.SetServiceProperty("transcriptionMode", "RealTimeAndAsync", ServicePropertyChannel.UriQueryParameter);
            var waveFilePullStream = OpenWavFile(@"katiesteve.wav", out audioStreamFormat);
            var audioInput = AudioConfig.FromStreamInput(AudioInputStream.CreatePullStream(waveFilePullStream, audioStreamFormat));

            var meetingId = Guid.NewGuid().ToString();
            using (var meeting = await Meeting.CreateMeetingAsync(config, meetingId))
            {
                using (var meetingTranscriber = TrackSessionId(new MeetingTranscriber(audioInput)))
                {
                    await meetingTranscriber.JoinMeetingAsync(meeting);

                    await meeting.AddParticipantAsync("OneUserByUserId");

                    var user = User.FromUserId("CreateUserFromId and then add it");
                    await meeting.AddParticipantAsync(user);

                    var result = await GetRecognizerResult(meetingTranscriber, meetingId);
                }
            }
            return meetingId;
        }

        private static async Task<List<string>> GetRecognizerResult(MeetingTranscriber recognizer, string meetingId)
        {
            List<string> recognizedText = new List<string>();
            recognizer.Transcribed += (s, e) =>
            {
                if (e.Result.Text.Length > 0)
                {
                    recognizedText.Add(e.Result.Text);
                    Console.WriteLine($"TRANSCRIBED: {e.Result.Text}, {e.Result.ResultId}, {e.Result.Reason}, {e.Result.UserId}, {e.Result.OffsetInTicks}, {e.Result.Duration}");
                }
            };

            await CompleteContinuousRecognition(recognizer, meetingId);

            recognizer.Dispose();
            return recognizedText;
        }

        private static async Task CompleteContinuousRecognition(MeetingTranscriber recognizer, string meetingId)
        {
            TaskCompletionSource<int> taskCompletionSource = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            recognizer.SessionStopped += (s, e) =>
            {
                taskCompletionSource.TrySetResult(0);
            };

            recognizer.Canceled += (s, e) =>
            {
                Console.WriteLine($"CANCELED: Reason={e.Reason}");
                if (e.Reason == CancellationReason.Error)
                {
                    Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                    Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                    Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    throw new System.ApplicationException("${e.ErrorDetails}");
                }
                taskCompletionSource.TrySetResult(0);
            };

            await recognizer.StartTranscribingAsync().ConfigureAwait(false);
            await Task.WhenAny(taskCompletionSource.Task, Task.Delay(TimeSpan.FromSeconds(10)));
            await recognizer.StopTranscribingAsync().ConfigureAwait(false);
        }
    }
}
