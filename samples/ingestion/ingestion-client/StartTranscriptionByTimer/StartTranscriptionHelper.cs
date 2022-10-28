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
    using System.Linq;
    using System.Net;
    using System.Text;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;
    using Connector;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;
    using Microsoft.Extensions.Logging;
    using Microsoft.WindowsAzure.Storage;
    using Newtonsoft.Json;

    public class StartTranscriptionHelper
    {
        private static readonly StorageConnector StorageConnectorInstance = new (StartTranscriptionEnvironmentVariables.AzureWebJobsStorage);

        private static readonly ServiceBusClient StartServiceBusClient = new ServiceBusClient(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString);

        private static readonly ServiceBusSender StartSender = StartServiceBusClient.CreateSender(ServiceBusConnectionStringProperties.Parse(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString).EntityPath);

        private static readonly ServiceBusClient FetchServiceBusClient = new ServiceBusClient(StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString);

        private static readonly ServiceBusSender FetchSender = FetchServiceBusClient.CreateSender(ServiceBusConnectionStringProperties.Parse(StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString).EntityPath);

        private readonly string subscriptionKey = StartTranscriptionEnvironmentVariables.AzureSpeechServicesKey;

        private readonly string errorReportContaineName = StartTranscriptionEnvironmentVariables.ErrorReportOutputContainer;

        private readonly string audioInputContainerName = StartTranscriptionEnvironmentVariables.AudioInputContainer;

        private readonly int filesPerTranscriptionJob = StartTranscriptionEnvironmentVariables.FilesPerTranscriptionJob;

        private readonly ILogger logger;

        private readonly string locale;

        public StartTranscriptionHelper(ILogger logger)
        {
            this.logger = logger;
            this.locale = StartTranscriptionEnvironmentVariables.Locale.Split('|')[0].Trim();
        }

        public async Task StartTranscriptionsAsync(IEnumerable<ServiceBusReceivedMessage> messages, ServiceBusReceiver messageReceiver, DateTime startDateTime)
        {
            if (messageReceiver == null)
            {
                throw new ArgumentNullException(nameof(messageReceiver));
            }

            var chunkedMessages = new List<List<ServiceBusReceivedMessage>>();
            var messageCount = messages.Count();

            for (int i = 0; i < messageCount; i += this.filesPerTranscriptionJob)
            {
                var chunk = messages.Skip(i).Take(Math.Min(this.filesPerTranscriptionJob, messageCount - i)).ToList();
                chunkedMessages.Add(chunk);
            }

            var stopwatch = new Stopwatch();
            stopwatch.Start();

            for (var i = 0; i < chunkedMessages.Count; i++)
            {
                var jobName = $"{startDateTime.ToString("yyyy-MM-ddTHH:mm:ss", CultureInfo.InvariantCulture)}_{i}";
                var chunk = chunkedMessages.ElementAt(i);
                await this.StartBatchTranscriptionJobAsync(chunk, jobName).ConfigureAwait(false);

                // Complete messages in batches of 10, process each batch in parallel:
                var messagesInChunk = chunk.Count;
                for (var j = 0; j < messagesInChunk; j += 10)
                {
                    var completionBatch = chunk.Skip(j).Take(Math.Min(10, messagesInChunk - j));
                    var completionTasks = completionBatch.Select(sb => messageReceiver.CompleteMessageAsync(sb));
                    await Task.WhenAll(completionTasks).ConfigureAwait(false);
                }

                // only renew lock after 2 minutes
                if (stopwatch.Elapsed.TotalSeconds > 120)
                {
                    foreach (var remainingChunk in chunkedMessages.Skip(i + 1))
                    {
                        foreach (var message in remainingChunk)
                        {
                            await messageReceiver.RenewMessageLockAsync(message).ConfigureAwait(false);
                        }
                    }

                    stopwatch.Restart();
                }

                // Delay here to avoid throttling
                await Task.Delay(500).ConfigureAwait(false);
            }
        }

        public async Task StartTranscriptionAsync(ServiceBusReceivedMessage message)
        {
            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            var busMessage = JsonConvert.DeserializeObject<Connector.ServiceBusMessage>(message.Body.ToString());
            var audioFileName = StorageConnector.GetFileNameFromUri(busMessage.Data.Url);

            await this.StartBatchTranscriptionJobAsync(new[] { message }, audioFileName).ConfigureAwait(false);
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Catch general exception to ensure that job continues execution even if message is invalid.")]
        public bool IsValidServiceBusMessage(ServiceBusReceivedMessage message)
        {
            if (message == null || message.Body == null)
            {
                this.logger.LogError($"Message {nameof(message)} is null.");
                return false;
            }

            var messageBody = message.Body.ToString();

            try
            {
                var serviceBusMessage = JsonConvert.DeserializeObject<Connector.ServiceBusMessage>(messageBody);

                if (serviceBusMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase) &&
                    StorageConnector.GetContainerNameFromUri(serviceBusMessage.Data.Url).Equals(this.audioInputContainerName, StringComparison.Ordinal))
                {
                    return true;
                }
            }
            catch (Exception e)
            {
                this.logger.LogError($"Exception {e.Message} while parsing message {messageBody} - message will be ignored.");
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
        private async Task StartBatchTranscriptionJobAsync(IEnumerable<ServiceBusReceivedMessage> messages, string jobName)
        {
            if (messages == null || !messages.Any())
            {
                this.logger.LogError($"Invalid service bus message(s).");
                return;
            }

            var locationString = string.Empty;
            var serviceBusMessages = messages.Select(message => JsonConvert.DeserializeObject<Connector.ServiceBusMessage>(Encoding.UTF8.GetString(message.Body)));

            try
            {
                var properties = this.GetTranscriptionPropertyBag();

                // only needed to make sure we do not add the same uri twice:
                var absoluteUrls = new HashSet<string>();

                var audioUrls = new List<string>();
                var audioFileInfos = new List<AudioFileInfo>();

                foreach (var serviceBusMessage in serviceBusMessages)
                {
                    var absoluteAudioUrl = serviceBusMessage.Data.Url.AbsoluteUri;

                    if (absoluteUrls.Contains(absoluteAudioUrl))
                    {
                        this.logger.LogError($"Unexpectedly received the same audio file twice: {absoluteAudioUrl}");
                        continue;
                    }

                    absoluteUrls.Add(absoluteAudioUrl);

                    if (StartTranscriptionEnvironmentVariables.IsByosEnabledSubscription)
                    {
                        audioUrls.Add(absoluteAudioUrl);
                    }
                    else
                    {
                        audioUrls.Add(StorageConnectorInstance.CreateSas(serviceBusMessage.Data.Url));
                    }

                    audioFileInfos.Add(new AudioFileInfo(absoluteAudioUrl, serviceBusMessage.RetryCount, textAnalyticsRequests: null));
                }

                ModelIdentity modelIdentity = null;

                if (Guid.TryParse(StartTranscriptionEnvironmentVariables.CustomModelId, out var customModelId))
                {
                    modelIdentity = new ModelIdentity($"{StartTranscriptionEnvironmentVariables.AzureSpeechServicesEndpointUri}speechtotext/v3.0/models/{customModelId}");
                }

                var transcriptionDefinition = TranscriptionDefinition.Create(jobName, "StartByTimerTranscription", this.locale, audioUrls, properties, modelIdentity);

                var transcriptionLocation = await BatchClient.PostTranscriptionAsync(
                    transcriptionDefinition,
                    StartTranscriptionEnvironmentVariables.AzureSpeechServicesEndpointUri,
                    this.subscriptionKey).ConfigureAwait(false);

                this.logger.LogInformation($"Location: {transcriptionLocation}");

                var transcriptionMessage = new TranscriptionStartedMessage(
                    transcriptionLocation.AbsoluteUri,
                    jobName,
                    this.locale,
                    modelIdentity != null,
                    audioFileInfos,
                    0,
                    0);

                var fetchingDelay = TimeSpan.FromMinutes(StartTranscriptionEnvironmentVariables.InitialPollingDelayInMinutes);
                await ServiceBusUtilities.SendServiceBusMessageAsync(FetchSender, transcriptionMessage.CreateMessageString(), this.logger, fetchingDelay).ConfigureAwait(false);
            }
            catch (TransientFailureException e)
            {
                await this.RetryOrFailMessagesAsync(messages, $"Exception in job {jobName}: {e.Message}", isThrottled: false).ConfigureAwait(false);
            }
            catch (TimeoutException e)
            {
                await this.RetryOrFailMessagesAsync(messages, $"Exception in job {jobName}: {e.Message}", isThrottled: false).ConfigureAwait(false);
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
                    await this.RetryOrFailMessagesAsync(messages, $"Error in job {jobName}: {e.Message}", isThrottled: httpStatusCode.Value == HttpStatusCode.TooManyRequests).ConfigureAwait(false);
                }
                else
                {
                    await this.WriteFailedJobLogToStorageAsync(serviceBusMessages, $"Exception {e} in job {jobName}: {e.Message}", jobName).ConfigureAwait(false);
                }
            }

            this.logger.LogInformation($"Fetch transcription queue successfully informed about job at: {jobName}");
        }

        private async Task RetryOrFailMessagesAsync(IEnumerable<ServiceBusReceivedMessage> messages, string errorMessage, bool isThrottled)
        {
            this.logger.LogError(errorMessage);
            foreach (var message in messages)
            {
                var serviceBusMessage = JsonConvert.DeserializeObject<Connector.ServiceBusMessage>(Encoding.UTF8.GetString(message.Body));

                if (serviceBusMessage.RetryCount <= StartTranscriptionEnvironmentVariables.RetryLimit || isThrottled)
                {
                    serviceBusMessage.RetryCount += 1;
                    var messageDelay = GetMessageDelayTime(serviceBusMessage.RetryCount);
                    var newMessage = new Azure.Messaging.ServiceBus.ServiceBusMessage(JsonConvert.SerializeObject(serviceBusMessage));
                    await ServiceBusUtilities.SendServiceBusMessageAsync(StartSender, newMessage, this.logger, messageDelay).ConfigureAwait(false);
                }
                else
                {
                    var fileName = StorageConnector.GetFileNameFromUri(serviceBusMessage.Data.Url);
                    var errorFileName = fileName + ".txt";
                    var retryExceededErrorMessage = $"Exceeded retry count for transcription {fileName} with error message {errorMessage}.";
                    this.logger.LogError(retryExceededErrorMessage);
                    await this.ProcessFailedFileAsync(fileName, errorMessage, errorFileName).ConfigureAwait(false);
                }
            }
        }

        private async Task WriteFailedJobLogToStorageAsync(IEnumerable<Connector.ServiceBusMessage> serviceBusMessages, string errorMessage, string jobName)
        {
            this.logger.LogError(errorMessage);
            var jobErrorFileName = $"jobs/{jobName}.txt";
            await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, this.errorReportContaineName, jobErrorFileName, this.logger).ConfigureAwait(false);

            foreach (var message in serviceBusMessages)
            {
                var fileName = StorageConnector.GetFileNameFromUri(message.Data.Url);
                var errorFileName = fileName + ".txt";
                await this.ProcessFailedFileAsync(fileName, errorMessage, errorFileName).ConfigureAwait(false);
            }
        }

        private Dictionary<string, string> GetTranscriptionPropertyBag()
        {
            var properties = new Dictionary<string, string>();

            var profanityFilterMode = StartTranscriptionEnvironmentVariables.ProfanityFilterMode;
            properties.Add("ProfanityFilterMode", profanityFilterMode);
            this.logger.LogInformation($"Setting profanity filter mode to {profanityFilterMode}");

            var punctuationMode = StartTranscriptionEnvironmentVariables.PunctuationMode;
            punctuationMode = punctuationMode.Replace(" ", string.Empty, StringComparison.Ordinal);
            properties.Add("PunctuationMode", punctuationMode);
            this.logger.LogInformation($"Setting punctuation mode to {punctuationMode}");

            var addDiarization = StartTranscriptionEnvironmentVariables.AddDiarization;
            properties.Add("DiarizationEnabled", addDiarization.ToString(CultureInfo.InvariantCulture));
            this.logger.LogInformation($"Setting diarization enabled to {addDiarization}");

            var addWordLevelTimestamps = StartTranscriptionEnvironmentVariables.AddWordLevelTimestamps;
            properties.Add("WordLevelTimestampsEnabled", addWordLevelTimestamps.ToString(CultureInfo.InvariantCulture));
            this.logger.LogInformation($"Setting word level timestamps enabled to {addWordLevelTimestamps}");

            return properties;
        }

        private async Task ProcessFailedFileAsync(string fileName, string errorMessage, string logFileName)
        {
            try
            {
                await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, this.errorReportContaineName, logFileName, this.logger).ConfigureAwait(false);
                await StorageConnectorInstance.MoveFileAsync(
                    this.audioInputContainerName,
                    fileName,
                    StartTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                    fileName,
                    false,
                    this.logger).ConfigureAwait(false);
            }
            catch (StorageException e)
            {
                this.logger.LogError($"Storage Exception {e} while writing error log to file and moving result");
            }
        }
    }
}
