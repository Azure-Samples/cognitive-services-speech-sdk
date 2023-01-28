// <copyright file="RealtimeTranscriptionHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace RealtimeTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.CognitiveServices.Speech;
    using Microsoft.CognitiveServices.Speech.Audio;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class RealtimeTranscriptionHelper
    {
        public static async Task<List<JsonResult>> TranscribeAsync(byte[] audio, SpeechConfig speechConfig, ILogger logger)
        {
            audio = audio ?? throw new ArgumentNullException(nameof(audio));
            speechConfig = speechConfig ?? throw new ArgumentNullException(nameof(speechConfig));

            var jsonResults = new List<JsonResult>();

            var chunkId = Encoding.ASCII.GetString(audio.Take(4).ToArray());

            // Verify that first 4 bytes are RIFF in ascii:
            // (see https://docs.fileformat.com/audio/wav/ for details)
            if (chunkId != "RIFF")
            {
                throw new InvalidOperationException($"Expected file header containing RIFF, received {chunkId} instead.");
            }

            // Get bytes for sampleRate, bitsPerSample, and channels:
            var sampleRate = BitConverter.ToUInt32(audio.Skip(24).Take(4).ToArray());
            var bitsPerSample = audio[34];
            var channels = audio[22];

            var audioStreamFormat = AudioStreamFormat.GetWaveFormatPCM(sampleRate, bitsPerSample, channels);

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            using var pushStream = AudioInputStream.CreatePushStream(audioStreamFormat);
            using (var audioInput = AudioConfig.FromStreamInput(pushStream))
            {
                using var recognizer = new SpeechRecognizer(
                    speechConfig,
                    audioInput);

                using var connection = Connection.FromRecognizer(recognizer);

                pushStream.Write(audio);
                pushStream.Close();

                logger.LogInformation("Starting speech recognition.");
                recognizer.Recognized += (s, e) =>
                {
                    if (e.Result.Reason == ResultReason.RecognizedSpeech)
                    {
                        var stringResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                        var deserializedResult = JsonConvert.DeserializeObject<JsonResult>(stringResult);
                        jsonResults.Add(deserializedResult);
                    }
                };

                recognizer.SessionStarted += (s, e) =>
                {
                    logger.LogInformation($"Starting session. Session id: {e.SessionId}.");
                };

                recognizer.SessionStopped += (s, e) =>
                {
                    logger.LogInformation("Session stopped");
                    stopRecognition.TrySetResult(0);
                };

                var cancellationError = CancellationErrorCode.NoError;
                var errorDetails = string.Empty;

                recognizer.Canceled += (s, e) =>
                {
                    var cancellation = CancellationDetails.FromResult(e.Result);
                    logger.LogError($"Recognition canceled: Reason={cancellation.Reason}");

                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        logger.LogError($"ErrorCode={cancellation.ErrorCode}");
                        logger.LogError($"ErrorDetails={cancellation.ErrorDetails}");

                        cancellationError = cancellation.ErrorCode;
                        errorDetails = cancellation.ErrorDetails;
                    }

                    stopRecognition.TrySetResult(0);
                };

                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                await Task.WhenAll(stopRecognition.Task).ConfigureAwait(false);

                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                if (cancellationError != CancellationErrorCode.NoError)
                {
                    throw new RealtimeTranscriptionException(cancellationError, errorDetails);
                }

                logger.LogInformation("Recognition stopped.");
            }

            return jsonResults;
        }

        public static ProfanityOption ParseProfanityModeFromString(string profanityMode, ILogger logger)
        {
            switch (profanityMode)
            {
                case "None":
                    return ProfanityOption.Raw;
                case "Removed":
                    return ProfanityOption.Removed;
                case "Masked":
                    return ProfanityOption.Masked;
                default:
                    logger.LogError($"Profanity filter mode \'{profanityMode}\' is not available for realtime transcription.");
                    return ProfanityOption.Raw;
            }
        }
    }
}
