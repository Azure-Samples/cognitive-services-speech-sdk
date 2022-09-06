// <copyright file="RealtimeTranscription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace RealtimeTranscription
{
    using System;
    using System.Text;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;
    using Connector;
    using Microsoft.Azure.WebJobs;
    using Microsoft.CognitiveServices.Speech;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class RealtimeTranscription
    {
        private static readonly StorageConnector StorageConnectorInstance = new (RealtimeTranscriptionEnvironmentVariables.AzureWebJobsStorage);

        private static readonly SpeechConfig SpeechConfig = SpeechConfig.FromEndpoint(
            new Uri($"wss://{RealtimeTranscriptionEnvironmentVariables.AzureSpeechServicesRegion}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1?setfeature=multichannel2&initialSilenceTimeoutMs=600000&endSilenceTimeoutMs=600000"),
            RealtimeTranscriptionEnvironmentVariables.AzureSpeechServicesKey);

        [FunctionName("RealtimeTranscription")]
        public static async Task Run([ServiceBusTrigger("start_transcription_queue", Connection = "AzureServiceBus")]ServiceBusReceivedMessage message, ILogger logger)
        {
            if (logger == null)
            {
                throw new ArgumentNullException(nameof(logger));
            }

            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            logger.LogInformation($"C# ServiceBus queue trigger function processed message: {message.Subject}");
            logger.LogInformation($"Received message: SequenceNumber:{message.SequenceNumber} Body:{Encoding.UTF8.GetString(message.Body)}");

            var serviceBusMessage = JsonConvert.DeserializeObject<Connector.ServiceBusMessage>(Encoding.UTF8.GetString(message.Body));

            if (!serviceBusMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase) ||
                !StorageConnector.GetContainerNameFromUri(serviceBusMessage.Data.Url).Equals(RealtimeTranscriptionEnvironmentVariables.AudioInputContainer, StringComparison.Ordinal))
            {
                logger.LogInformation("Ignoring service bus message since it is not a BlobCreate message or not coming from the audio input container.");
                return;
            }

            var audioFileName = StorageConnector.GetFileNameFromUri(serviceBusMessage.Data.Url);

            try
            {
                var audioBytes = await StorageConnectorInstance.DownloadFileFromContainer(
                    RealtimeTranscriptionEnvironmentVariables.AudioInputContainer,
                    audioFileName).ConfigureAwait(false);

                // Set speech config parameters:
                SpeechConfig.OutputFormat = OutputFormat.Detailed;

                // custom endpoint
                if (!string.IsNullOrEmpty(RealtimeTranscriptionEnvironmentVariables.CustomEndpointId))
                {
                    SpeechConfig.EndpointId = RealtimeTranscriptionEnvironmentVariables.CustomEndpointId;
                }

                // locale
                SpeechConfig.SpeechRecognitionLanguage = RealtimeTranscriptionEnvironmentVariables.Locale.Split('|')[0].Trim();

                // profanity filter mode
                var profanityFilterMode = RealtimeTranscriptionHelper.ParseProfanityModeFromString(RealtimeTranscriptionEnvironmentVariables.ProfanityFilterMode, logger);
                SpeechConfig.SetProfanity(profanityFilterMode);

                // word level timestamps
                if (RealtimeTranscriptionEnvironmentVariables.AddWordLevelTimestamps)
                {
                    SpeechConfig.RequestWordLevelTimestamps();
                }

                var jsonResults = await RealtimeTranscriptionHelper.TranscribeAsync(audioBytes, SpeechConfig, logger).ConfigureAwait(false);
                var speechTranscript = ResultConversionHelper.CreateBatchResultFromRealtimeResults(serviceBusMessage.Data.Url.AbsoluteUri, jsonResults, logger);

                var speechTranscriptString = JsonConvert.SerializeObject(speechTranscript, new JsonSerializerSettings()
                {
                    Formatting = Formatting.Indented,
                    NullValueHandling = NullValueHandling.Ignore
                });

                await StorageConnectorInstance.WriteTextFileToBlobAsync(
                    speechTranscriptString,
                    RealtimeTranscriptionEnvironmentVariables.JsonResultOutputContainer,
                    $"{audioFileName}.json",
                    logger).ConfigureAwait(false);

                await StorageConnectorInstance.MoveFileAsync(
                    RealtimeTranscriptionEnvironmentVariables.AudioInputContainer,
                    audioFileName,
                    RealtimeTranscriptionEnvironmentVariables.AudioProcessedContainer,
                    audioFileName,
                    false,
                    logger).ConfigureAwait(false);
            }
            catch (Exception exception)
            {
                if (exception is RealtimeTranscriptionException realtimeTranscriptionException && IsRetryableErrorCode(realtimeTranscriptionException.CancellationErrorCode))
                {
                    // Delaying for a minute to safe guarding against exhausting the Service Bus retries.
                    // For processing higher volume than 100 concurrent audio streams refer to set up guide
                    await Task.Delay(60000).ConfigureAwait(false);

                    throw;
                }

                logger.LogError(exception.Message);

                await StorageConnectorInstance.MoveFileAsync(
                    RealtimeTranscriptionEnvironmentVariables.AudioInputContainer,
                    audioFileName,
                    RealtimeTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                    audioFileName,
                    false,
                    logger).ConfigureAwait(false);

                await StorageConnectorInstance.WriteTextFileToBlobAsync(
                    exception.Message,
                    RealtimeTranscriptionEnvironmentVariables.ErrorReportOutputContainer,
                    $"{audioFileName}.txt",
                    logger).ConfigureAwait(false);
            }
        }

        private static bool IsRetryableErrorCode(CancellationErrorCode cancellationErrorCode)
        {
            switch (cancellationErrorCode)
            {
                case CancellationErrorCode.NoError:
                case CancellationErrorCode.TooManyRequests:
                case CancellationErrorCode.ServiceError:
                case CancellationErrorCode.ServiceTimeout:
                case CancellationErrorCode.ServiceUnavailable:
                    return true;
                default: return false;
            }
        }
    }
}
