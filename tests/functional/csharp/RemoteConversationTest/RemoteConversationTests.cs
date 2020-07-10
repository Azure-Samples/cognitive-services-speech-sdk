//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Transcription;
using Microsoft.CognitiveServices.Speech.RemoteConversation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;

namespace RemoteConversationTest
{
    public struct SubscriptionRegion
    {
        public string Key { get; set; }
        public string Region { get; set; }
    }

    internal class BinaryAudioStreamReader : PullAudioInputStreamCallback
    {
        private System.IO.BinaryReader _reader;

        /// <summary>
        /// Creates and initializes an instance of BinaryAudioStreamReader.
        /// </summary>
        /// <param name="reader">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public BinaryAudioStreamReader(System.IO.BinaryReader reader)
        {
            _reader = reader;
        }

        /// <summary>
        /// Creates and initializes an instance of BinaryAudioStreamReader.
        /// </summary>
        /// <param name="stream">The underlying stream to read the audio data from. Note: The stream contains the bare sample data, not the container (like wave header data, etc).</param>
        public BinaryAudioStreamReader(System.IO.Stream stream)
            : this(new System.IO.BinaryReader(stream))
        {
        }

        /// <summary>
        /// Reads binary data from the stream.
        /// </summary>
        /// <param name="dataBuffer">The buffer to fill</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
        /// If there is no data immediate available, Read() blocks until the next data becomes available.</returns>
        public override int Read(byte[] dataBuffer, uint size)
        {
            return _reader.Read(dataBuffer, 0, (int)size);
        }

        /// <summary>
        /// Get property associated to data buffer, such as a timestamp or userId. if the property is not available, an empty string must be returned. 
        /// </summary>
        /// <param name="id">A property id.</param>
        /// <returns>The value of the property </returns>
        public override string GetProperty(PropertyId id)
        {
            if (PropertyId.ConversationTranscribingService_DataBufferUserId == id)
            {
                return "speaker123";
            }
            else if (PropertyId.ConversationTranscribingService_DataBufferTimeStamp == id)
            {
                return "somefaketimestamp";
            }
            return "";
        }

        /// <summary>
        /// This method performs cleanup of resources.
        /// The Boolean parameter <paramref name="disposing"/> indicates whether the method is called from <see cref="IDisposable.Dispose"/> (if <paramref name="disposing"/> is true) or from the finalizer (if <paramref name="disposing"/> is false).
        /// Derived classes should override this method to dispose resource if needed.
        /// </summary>
        /// <param name="disposing">Flag to request disposal.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                _reader.Dispose();
            }

            disposed = true;
            base.Dispose(disposing);
        }

        private bool disposed = false;
    }


    [TestClass]
    public class RemoteConversationTests
    {

        [TestMethod]
        public void BasicRemoteConversationTest()
        {
            // Load the keys from json
            LoadKeys();

            var key = m_subscriptionsRegionsMap["ConversationTranscriptionPRODSubscription"].Key;
            var region = m_subscriptionsRegionsMap["ConversationTranscriptionPRODSubscription"].Region;

            // Upload the audio and poll the service
            BasicRemoteConversationUseCase(key, region);

            // Delete the transcriptions
            TestDeleteCase(key, region).Wait();
        }

        private async Task TestDeleteCase(string key, string region)
        {
            string meetingid = m_meetingId;

            SpeechConfig config = SpeechConfig.FromSubscription(key, region);
            RemoteConversationTranscriptionClient client = new RemoteConversationTranscriptionClient(config);
            var deleteRes = await client.DeleteRemoteTranscriptionAsync(meetingid);

            Assert.AreEqual(200, deleteRes.Status, $"Delete operation failed with status {deleteRes.Status}");
        }

        private void BasicRemoteConversationUseCase(string key, string region)
        {
            // Upload the audio to the service
            UploadAudioAndStartRemoteTranscription(key, region).Wait();

            // Poll the service 
            TestRemoteTranscription(key, region);
        }

        private void TestRemoteTranscription(string key, string region)
        {
            Console.WriteLine($"MeetingId : {m_meetingId}");
            SpeechConfig config = SpeechConfig.FromSubscription(key, region);
            RemoteConversationTranscriptionClient client = new RemoteConversationTranscriptionClient(config);
            RemoteConversationTranscriptionOperation operation = new RemoteConversationTranscriptionOperation(m_meetingId, client);
            WaitForCompletion(operation).Wait();
        }

        private async Task WaitForCompletion(RemoteConversationTranscriptionOperation operation)
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
            Assert.AreEqual(7, val.Count, $"expected is 7, but actual is {val.Count}");
        }
        private PullAudioInputStreamCallback OpenWavFile(string filename, out AudioStreamFormat format)
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

        private ConversationTranscriber TrackSessionId(ConversationTranscriber recognizer)
        {
            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("SessionId: " + e.SessionId);
            };
            return recognizer;
        }

        private async Task UploadAudioAndStartRemoteTranscription(string key, string region)
        {
            AudioStreamFormat audioStreamFormat;

            var config = SpeechConfig.FromSubscription(key, region);
            config.SetProperty("ConversationTranscriptionInRoomAndOnline", "true");
            config.SetServiceProperty("transcriptionMode", "RealTimeAndAsync", ServicePropertyChannel.UriQueryParameter);
            var waveFilePullStream = OpenWavFile(m_wavfilename, out audioStreamFormat);
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

                    string voice = m_voiceSignatureKatie;
                    var participant = Participant.From("userIdForParticipant", "en-US", voice);
                    await conversation.AddParticipantAsync(participant);

                    var result = await GetRecognizerResult(conversationTranscriber, meetingId);
                }
            }
            m_meetingId = meetingId;
        }

        private async Task<List<string>> GetRecognizerResult(ConversationTranscriber recognizer, string conversationId)
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

        private async Task CompleteContinuousRecognition(ConversationTranscriber recognizer, string conversationId)
        {
            recognizer.SessionStopped += (s, e) =>
            {
                m_taskCompletionSource.TrySetResult(0);
            };
            string canceled = string.Empty;

            recognizer.Canceled += (s, e) => {
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

        private void Deserialize<T>(string filePath, out T data)
        {
            if (File.Exists(filePath))
            {
                using (FileStream fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
                {
                    using (StreamReader streamReader = new StreamReader(fileStream))
                    {
                        using (JsonTextReader jsonTextReader = new JsonTextReader(streamReader))
                        {
                            JsonSerializer jsonSerializer = new JsonSerializer();
                            try
                            {
                                data = jsonSerializer.Deserialize<T>(jsonTextReader);
                            }
                            catch (JsonException jsonException)
                            {
                                Console.WriteLine($"Deserializing test.defaults.json threw an exception {jsonException.Message}");
                                throw;
                            }
                        }
                    }
                }
            }
            else
            {
                throw new InvalidOperationException($"json file {filePath} not found!");
            }
        }

        private void LoadKeys()
        {
            Deserialize("test.subscriptions.regions.json", out m_subscriptionsRegionsMap);
        }

        private const string m_voiceSignatureKatie = "{ \"Version\": 0, \"Tag\": \"VtZQ7sJp8np3AxQC+87WYyBHWsZohWFBN0zgWzzOnpw=\", \"Data\": \"BhRRgDCrg6ij5fylg5Jpf5ZW/o/uDWi199DYBbfL1Qdspj77qiuvsVKzG2g5Z9jxKtfdwtkKtaDxog9O6pGD7Ot/8mM1jUtt6LKNz4H9EFvznV/dlFk2Oisg8ZKx/RBlNFMYJkQJnxT/zLfhNWiIF5Y97jH4sgRh2orDg6/567FGktAgcESAbiDx1e7tf0TTLdwijw4p1vJ3qJ2cSCdNbXE9KeUd8sClQLDheCPo+et3rMs5W+Rju3W1SJE6ru9gAoH88CyAfI80+ysAecH3GPJYM+j1uhvmWoKIrSfS40BYOe6AUgLNb3a4Pue4oGAmuAyWfwpP1uezleSanpJc73HT91n2UsaHrQj5eK6uuBCjwmu+JI3FT+Vo6JhAARHecdb70U1wyW9vv5t0Q3tV1WNiN/30qSVydDtyrSVpgBiIwlj41Ua22JJCOMrPl7NKLnFmeZ4Hi4aIKoHAxDvrApteL60sxLX/ADAtYCB3Y6iagDyR1IOsIlbaPhL0rQDnC/0z65k7BDekietFNzvvPVoIwJ26GHrXFYYwZe3alVvCsXTpZGBknvSiaCalrixnyGqYo0nG/cd/LodEEIht/PWoFkNlbABqHMbToeI/6j+ICKuVJgTDtcsDQiWKSvrQp9kTSv+cF3LyPVkbks0JvbQhj4AkAv7Rvymgnsu6o8gGyxTX0kxujqCMPCzgFrVd\"}";
        private TaskCompletionSource<int> m_taskCompletionSource = new TaskCompletionSource<int>();
        private string m_meetingId = "";
        private string m_wavfilename = @"katiesteve.wav";
        private IDictionary<string, SubscriptionRegion> m_subscriptionsRegionsMap;

    }
}
