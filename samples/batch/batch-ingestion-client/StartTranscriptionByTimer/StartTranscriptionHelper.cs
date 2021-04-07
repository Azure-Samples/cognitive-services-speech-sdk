// <copyright file="StartTranscriptionHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Text;
    using System.Threading.Tasks;
    using Connector;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Azure.ServiceBus.Core;
    using Microsoft.Extensions.Logging;
    using Microsoft.WindowsAzure.Storage;
    using Newtonsoft.Json;

    public class StartTranscriptionHelper
    {
        private static readonly StorageConnector StorageConnectorInstance = new (StartTranscriptionEnvironmentVariables.AzureWebJobsStorage);

        private static readonly QueueClient StartQueueClientInstance = new (new ServiceBusConnectionStringBuilder(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString));

        private static readonly QueueClient FetchQueueClientInstance = new (new ServiceBusConnectionStringBuilder(StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString));

        private readonly string SubscriptionKey = StartTranscriptionEnvironmentVariables.AzureSpeechServicesKey;

        private readonly string ErrorReportContaineName = StartTranscriptionEnvironmentVariables.ErrorReportOutputContainer;

        private readonly string AudioInputContainerName = StartTranscriptionEnvironmentVariables.AudioInputContainer;

        private readonly int FilesPerTranscriptionJob = StartTranscriptionEnvironmentVariables.FilesPerTranscriptionJob;

        private readonly string HostName = StartTranscriptionEnvironmentVariables.IsAzureGovDeployment ?
            $"https://{StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion}.api.cognitive.microsoft.us/" :
            $"https://{StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion}.api.cognitive.microsoft.com/";

        private readonly ILogger Logger;

        private readonly string Locale;

        public StartTranscriptionHelper(ILogger logger)
        {
            Logger = logger;
            Locale = StartTranscriptionEnvironmentVariables.Locale.Split('|')[0].Trim();
        }

        public async Task StartTranscriptionsAsync(IEnumerable<Message> messages, MessageReceiver messageReceiver, DateTime startDateTime)
        {
            if (messageReceiver == null)
            {
                throw new ArgumentNullException(nameof(messageReceiver));
            }

            var chunkedMessages = new List<List<Message>>();
            var messageCount = messages.Count();

            for (int i = 0; i < messageCount; i += FilesPerTranscriptionJob)
            {
                var chunk = messages.Skip(i).Take(Math.Min(FilesPerTranscriptionJob, messageCount - i)).ToList();
                chunkedMessages.Add(chunk);
            }

            var stopwatch = new Stopwatch();
            stopwatch.Start();

            for (var i = 0; i < chunkedMessages.Count; i++)
            {
                var jobName = $"{startDateTime.ToString("yyyy-MM-ddTHH:mm:ss", CultureInfo.InvariantCulture)}_{i}";
                var chunk = chunkedMessages.ElementAt(i);
                await StartBatchTranscriptionJobAsync(chunk, jobName).ConfigureAwait(false);
                await messageReceiver.CompleteAsync(chunk.Select(m => m.SystemProperties.LockToken)).ConfigureAwait(false);

                // only renew lock after 2 minutes
                if (stopwatch.Elapsed.TotalSeconds > 120)
                {
                    foreach (var remainingChunk in chunkedMessages.Skip(i + 1))
                    {
                        foreach (var message in remainingChunk)
                        {
                            await messageReceiver.RenewLockAsync(message).ConfigureAwait(false);
                        }
                    }

                    stopwatch.Restart();
                }

                // Delay here to avoid throttling
                await Task.Delay(500).ConfigureAwait(false);
            }
        }

        public async Task StartTranscriptionAsync(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            var busMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body));
            var audioFileName = StorageConnector.GetFileNameFromUri(busMessage.Data.Url);

            await StartBatchTranscriptionJobAsync(new[] { message }, audioFileName).ConfigureAwait(false);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Catch general exception to ensure that job continues execution even if message is invalid.")]
        public bool IsValidServiceBusMessage(Message message)
        {
            if (message == null || message.Body == null || !message.Body.Any())
            {
                Logger.LogError($"Message {nameof(message)} is null.");
                return false;
            }

            var messageBody = Encoding.UTF8.GetString(message.Body);

            try
            {
                var serviceBusMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(messageBody);

                if (serviceBusMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase) &&
                    StorageConnector.GetContainerNameFromUri(serviceBusMessage.Data.Url).Equals(AudioInputContainerName, StringComparison.Ordinal))
                {
                    return true;
                }
            }
            catch (Exception e)
            {
                Logger.LogError($"Exception {e.Message} while parsing message {messageBody} - message will be ignored.");
                return false;
            }

            return false;
        }

        private static TimeSpan GetMessageDelayTime(int pollingCounter)
        {
            if (pollingCounter == 0)
            {
                return TimeSpan.FromMinutes(StartTranscriptionEnvironmentVariables.InitialPollingDelayInMinutes);
            }

            var updatedDelay = Math.Pow(2, Math.Min(pollingCounter, 8)) * StartTranscriptionEnvironmentVariables.InitialPollingDelayInMinutes;

            if ((int)updatedDelay > StartTranscriptionEnvironmentVariables.MaxPollingDelayInMinutes)
            {
                return TimeSpan.FromMinutes(StartTranscriptionEnvironmentVariables.MaxPollingDelayInMinutes);
            }

            return TimeSpan.FromMinutes(updatedDelay);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Allow general exception catching to retry transcriptions in that case.")]
        private async Task StartBatchTranscriptionJobAsync(IEnumerable<Message> messages, string jobName)
        {
            if (messages == null || !messages.Any())
            {
                Logger.LogError($"Invalid service bus message(s).");
                return;
            }

            var locationString = string.Empty;
            var serviceBusMessages = messages.Select(message => JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body)));

            try
            {
                var properties = GetTranscriptionPropertyBag();

                var sasUrls = new List<string>();
                var audioFileInfos = new List<AudioFileInfo>();

                foreach (var serviceBusMessage in serviceBusMessages)
                {
                    var sasUrl = StorageConnectorInstance.CreateSas(serviceBusMessage.Data.Url);
                    sasUrls.Add(sasUrl);
                    audioFileInfos.Add(new AudioFileInfo(serviceBusMessage.Data.Url.AbsoluteUri, serviceBusMessage.RetryCount));
                }

                ModelIdentity modelIdentity = null;

                if (Guid.TryParse(StartTranscriptionEnvironmentVariables.CustomModelId, out var customModelId))
                {
                    modelIdentity = ModelIdentity.Create(StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion, customModelId);
                }

                var transcriptionDefinition = TranscriptionDefinition.Create(jobName, "StartByTimerTranscription", Locale, sasUrls, properties, modelIdentity);

                var transcriptionLocation = await BatchClient.PostTranscriptionAsync(
                    transcriptionDefinition,
                    HostName,
                    SubscriptionKey,
                    Logger).ConfigureAwait(false);

                Logger.LogInformation($"Location: {transcriptionLocation}");

                var transcriptionMessage = new TranscriptionStartedMessage(
                    transcriptionLocation.AbsoluteUri,
                    jobName,
                    Locale,
                    modelIdentity != null,
                    audioFileInfos,
                    0,
                    0);

                var fetchingDelay = TimeSpan.FromMinutes(StartTranscriptionEnvironmentVariables.InitialPollingDelayInMinutes);
                await ServiceBusUtilities.SendServiceBusMessageAsync(FetchQueueClientInstance, transcriptionMessage.CreateMessageString(), Logger, fetchingDelay).ConfigureAwait(false);
            }
            catch (TimeoutException e)
            {
                await RetryOrFailMessagesAsync(messages, $"Timeout in job {jobName}: {e.Message}", HttpStatusCode.RequestTimeout).ConfigureAwait(false);
            }
            catch (Exception e)
            {
                HttpStatusCode? httpStatusCode = null;
                if (e is HttpStatusCodeException statusCodeException && statusCodeException.HttpStatusCode.HasValue)
                {
                    httpStatusCode = statusCodeException.HttpStatusCode.Value;
                }
                else if (e is WebException webException && webException.Response != null)
                {
                    httpStatusCode = ((HttpWebResponse)webException.Response).StatusCode;
                }

                if (httpStatusCode.HasValue && httpStatusCode.Value.IsRetryableStatus())
                {
                    await RetryOrFailMessagesAsync(messages, $"Error in job {jobName}: {e.Message}", httpStatusCode.Value).ConfigureAwait(false);
                }
                else
                {
                    await WriteFailedJobLogToStorageAsync(serviceBusMessages, $"Exception {e} in job {jobName}: {e.Message}", jobName).ConfigureAwait(false);
                }
            }

            Logger.LogInformation($"Fetch transcription queue successfully informed about job at: {jobName}");
        }

        private async Task RetryOrFailMessagesAsync(IEnumerable<Message> messages, string errorMessage, HttpStatusCode statusCode)
        {
            Logger.LogError(errorMessage);
            foreach (var message in messages)
            {
                var sbMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body));

                if (sbMessage.RetryCount <= StartTranscriptionEnvironmentVariables.RetryLimit || statusCode == HttpStatusCode.TooManyRequests)
                {
                    sbMessage.RetryCount += 1;
                    var messageDelay = GetMessageDelayTime(sbMessage.RetryCount);
                    var newMessage = new Message(Encoding.UTF8.GetBytes(JsonConvert.SerializeObject(sbMessage)));
                    await ServiceBusUtilities.SendServiceBusMessageAsync(StartQueueClientInstance, newMessage, Logger, messageDelay).ConfigureAwait(false);
                }
                else
                {
                    var fileName = StorageConnector.GetFileNameFromUri(sbMessage.Data.Url);
                    var errorFileName = fileName + ".txt";
                    var retryExceededErrorMessage = $"Exceeded retry count for transcription {fileName} with error message {errorMessage}.";
                    Logger.LogError(retryExceededErrorMessage);
                    await ProcessFailedFileAsync(fileName, errorMessage, errorFileName).ConfigureAwait(false);
                }
            }
        }

        private async Task WriteFailedJobLogToStorageAsync(IEnumerable<ServiceBusMessage> serviceBusMessages, string errorMessage, string jobName)
        {
            Logger.LogError(errorMessage);
            var jobErrorFileName = $"jobs/{jobName}.txt";
            await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, ErrorReportContaineName, jobErrorFileName, Logger).ConfigureAwait(false);

            foreach (var message in serviceBusMessages)
            {
                var fileName = StorageConnector.GetFileNameFromUri(message.Data.Url);
                var errorFileName = fileName + ".txt";
                await ProcessFailedFileAsync(fileName, errorMessage, errorFileName).ConfigureAwait(false);
            }
        }

        private Dictionary<string, string> GetTranscriptionPropertyBag()
        {
            var properties = new Dictionary<string, string>();

            var profanityFilterMode = StartTranscriptionEnvironmentVariables.ProfanityFilterMode;
            properties.Add("ProfanityFilterMode", profanityFilterMode);
            Logger.LogInformation($"Setting profanity filter mode to {profanityFilterMode}");

            var punctuationMode = StartTranscriptionEnvironmentVariables.PunctuationMode;
            punctuationMode = punctuationMode.Replace(" ", string.Empty, StringComparison.Ordinal);
            properties.Add("PunctuationMode", punctuationMode);
            Logger.LogInformation($"Setting punctuation mode to {punctuationMode}");

            var addDiarization = StartTranscriptionEnvironmentVariables.AddDiarization;
            properties.Add("DiarizationEnabled", addDiarization.ToString(CultureInfo.InvariantCulture));
            Logger.LogInformation($"Setting diarization enabled to {addDiarization}");

            var addWordLevelTimestamps = StartTranscriptionEnvironmentVariables.AddWordLevelTimestamps;
            properties.Add("WordLevelTimestampsEnabled", addWordLevelTimestamps.ToString(CultureInfo.InvariantCulture));
            Logger.LogInformation($"Setting word level timestamps enabled to {addWordLevelTimestamps}");

            return properties;
        }

        private async Task ProcessFailedFileAsync(string fileName, string errorMessage, string logFileName)
        {
            try
            {
                await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, ErrorReportContaineName, logFileName, Logger).ConfigureAwait(false);
                await StorageConnectorInstance.MoveFileAsync(
                    AudioInputContainerName,
                    fileName,
                    StartTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                    fileName,
                    false,
                    Logger).ConfigureAwait(false);
            }
            catch (StorageException e)
            {
                Logger.LogError($"Storage Exception {e} while writing error log to file and moving result");
            }
        }
    }
}
