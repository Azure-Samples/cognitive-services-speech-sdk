// <copyright file="StartTranscriptionHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Text;
    using System.Threading.Tasks;
    using Connector;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Azure.ServiceBus.Core;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public class StartTranscriptionHelper
    {
        private static StorageConnector StorageConnectorInstance = new StorageConnector(StartTranscriptionEnvironmentVariables.AzureWebJobsStorage);

        private static QueueClient StartQueueClientInstance = new QueueClient(new ServiceBusConnectionStringBuilder(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString));

        private static QueueClient FetchQueueClientInstance = new QueueClient(new ServiceBusConnectionStringBuilder(StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString));

        private readonly string SubscriptionKey = StartTranscriptionEnvironmentVariables.AzureSpeechServicesKey;

        private readonly string SubscriptionRegion = StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion;

        private readonly string ErrorReportContaineName = StartTranscriptionEnvironmentVariables.ErrorReportOutputContainer;

        private readonly string AudioInputContainerName = StartTranscriptionEnvironmentVariables.AudioInputContainer;

        private readonly int FilesPerTranscriptionJob = StartTranscriptionEnvironmentVariables.FilesPerTranscriptionJob;

        private readonly string HostName = $"https://{StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion}.api.cognitive.microsoft.com/";

        private ILogger Logger;

        private string Locale;

        public StartTranscriptionHelper(ILogger logger)
        {
            Logger = logger;
            Locale = StartTranscriptionEnvironmentVariables.Locale.Split('|')[0].Trim();
        }

        public static StorageConnector GetStorageConnector()
        {
            return StorageConnectorInstance;
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

            for (var i = 0; i < chunkedMessages.Count; i++)
            {
                var jobName = $"{startDateTime.ToString("yyyy-MM-ddTHH:mm:ss", CultureInfo.InvariantCulture)}_{i}";
                var chunk = chunkedMessages.ElementAt(i);
                await StartBatchTranscriptionJobAsync(chunk, jobName).ConfigureAwait(false);
                await messageReceiver.CompleteAsync(chunk.Select(m => m.SystemProperties.LockToken)).ConfigureAwait(false);

                // Renew lock on remaining messages
                foreach (var remainingChunk in chunkedMessages.Skip(i + 1))
                {
                    foreach (var message in remainingChunk)
                    {
                        await messageReceiver.RenewLockAsync(message).ConfigureAwait(false);
                    }
                }

                // Delay here to avoid throttling
                await Task.Delay(2000).ConfigureAwait(false);
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

            // Check if language identification is required:
            var secondaryLocale = StartTranscriptionEnvironmentVariables.SecondaryLocale;
            if (!string.IsNullOrEmpty(secondaryLocale) &&
                !secondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase) &&
                !secondaryLocale.Equals(Locale, StringComparison.OrdinalIgnoreCase))
            {
                secondaryLocale = secondaryLocale.Split('|')[0].Trim();

                Logger.LogInformation($"Primary locale: {Locale}");
                Logger.LogInformation($"Secondary locale: {secondaryLocale}");

                var languageID = new LanguageIdentification(SubscriptionKey, SubscriptionRegion);
                var fileExtension = Path.GetExtension(audioFileName);
                var sasUrl = StorageConnectorInstance.CreateSas(busMessage.Data.Url);
                var byteArray = await StorageConnector.DownloadFileFromSAS(sasUrl).ConfigureAwait(false);
                var identifiedLocale = await languageID.DetectLanguage(byteArray, fileExtension, Locale, secondaryLocale).ConfigureAwait(false);
                Logger.LogInformation($"Identified locale: {identifiedLocale}");
                Locale = identifiedLocale;
            }

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

        private static TimeSpan GetInitialFetchingDelay(int transcriptionCount)
        {
            var minutes = Math.Max(2, transcriptionCount / 100);
            return TimeSpan.FromMinutes(minutes);
        }

        private static TimeSpan GetMessageDelayTime(int pollingCounter)
        {
            var delayInMinutes = Math.Pow(2, Math.Min(pollingCounter, 7));
            return TimeSpan.FromMinutes(delayInMinutes);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Allow general exception catching to retry transcriptions in that case.")]
        private async Task StartBatchTranscriptionJobAsync(IEnumerable<Message> messages, string jobName)
        {
            if (messages == null || !messages.Any())
            {
                Logger.LogError($"Invalid service bus message(s).");
                return;
            }

            var fetchingDelay = GetInitialFetchingDelay(messages.Count());
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

                await ServiceBusUtilities.SendServiceBusMessageAsync(FetchQueueClientInstance, transcriptionMessage.CreateMessageString(), Logger, fetchingDelay).ConfigureAwait(false);
            }
            catch (WebException e)
            {
                if (BatchClient.IsThrottledOrTimeoutStatusCode(((HttpWebResponse)e.Response).StatusCode))
                {
                    var errorMessage = $"Throttled or timeout while creating post. Error Message: {e.Message}";
                    Logger.LogError(errorMessage);
                    await RetryOrFailMessagesAsync(messages, errorMessage).ConfigureAwait(false);
                }
                else
                {
                    var errorMessage = $"Start Transcription in job with name {jobName} failed with WebException {e} and message {e.Message}";
                    Logger.LogError(errorMessage);

                    using (var reader = new StreamReader(e.Response.GetResponseStream()))
                    {
                        var responseMessage = await reader.ReadToEndAsync().ConfigureAwait(false);
                        errorMessage += "\nResponse message:" + responseMessage;
                    }

                    await WriteFailedJobLogToStorageAsync(serviceBusMessages, errorMessage, jobName).ConfigureAwait(false);
                }

                throw;
            }
            catch (TimeoutException e)
            {
                var errorMessage = $"Timeout while creating post, re-enqueueing transcription start. Message: {e.Message}";
                Logger.LogError(errorMessage);
                await RetryOrFailMessagesAsync(messages, errorMessage).ConfigureAwait(false);
                throw;
            }
            catch (Exception e)
            {
                var errorMessage = $"Start Transcription in job with name {jobName} failed with exception {e} and message {e.Message}";
                Logger.LogError(errorMessage);
                await WriteFailedJobLogToStorageAsync(serviceBusMessages, errorMessage, jobName).ConfigureAwait(false);
                throw;
            }

            Logger.LogInformation($"Fetch transcription queue successfully informed about job at: {jobName}");
        }

        private async Task RetryOrFailMessagesAsync(IEnumerable<Message> messages, string errorMessage)
        {
            foreach (var message in messages)
            {
                var sbMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body));

                if (sbMessage.RetryCount >= StartTranscriptionEnvironmentVariables.RetryLimit)
                {
                    var fileName = StorageConnector.GetFileNameFromUri(sbMessage.Data.Url);
                    var errorFileName = fileName + ".txt";
                    var retryExceededErrorMessage = $"Exceeded retry count for transcription {fileName} with error message {errorMessage}.";
                    Logger.LogInformation(retryExceededErrorMessage);
                    await StorageConnectorInstance.WriteTextFileToBlobAsync(retryExceededErrorMessage, ErrorReportContaineName, errorFileName, Logger).ConfigureAwait(false);

                    await StorageConnectorInstance.MoveFileAsync(
                        AudioInputContainerName,
                        fileName,
                        StartTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                        fileName,
                        Logger).ConfigureAwait(false);
                }
                else
                {
                    sbMessage.RetryCount += 1;
                    var messageDelay = GetMessageDelayTime(sbMessage.RetryCount);
                    var newMessage = new Message(Encoding.UTF8.GetBytes(JsonConvert.SerializeObject(sbMessage)));
                    await ServiceBusUtilities.SendServiceBusMessageAsync(StartQueueClientInstance, newMessage, Logger, messageDelay).ConfigureAwait(false);
                }
            }
        }

        private async Task WriteFailedJobLogToStorageAsync(IEnumerable<ServiceBusMessage> serviceBusMessages, string errorMessage, string jobName)
        {
            var jobErrorFileName = $"jobs/{jobName}.txt";
            await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, ErrorReportContaineName, jobErrorFileName, Logger).ConfigureAwait(false);

            foreach (var message in serviceBusMessages)
            {
                var fileName = StorageConnector.GetFileNameFromUri(message.Data.Url);
                var errorFileName = fileName + ".txt";
                await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, ErrorReportContaineName, errorFileName, Logger).ConfigureAwait(false);
                await StorageConnectorInstance.MoveFileAsync(
                    AudioInputContainerName,
                    fileName,
                    StartTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                    fileName,
                    Logger).ConfigureAwait(false);
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
    }
}
