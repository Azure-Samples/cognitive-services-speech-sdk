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
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public class StartTranscriptionHelper
    {
        private const int MaxFilesPerTranscriptionJob = 1000;

        private ILogger Logger;

        private Subscription Subscription;

        private string Locale;

        public StartTranscriptionHelper(ILogger logger)
        {
            Logger = logger;
            var subscriptionRegion = new Uri($"https://{StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion}.api.cognitive.microsoft.com");
            Subscription = new Subscription(StartTranscriptionEnvironmentVariables.AzureSpeechServicesKey, subscriptionRegion);
            Locale = StartTranscriptionEnvironmentVariables.Locale.Split('|')[0].Trim();
        }

        public async Task StartTranscriptionsAsync(IEnumerable<Message> messages)
        {
            var startDateTime = DateTime.UtcNow;

            var chunkedMessages = new List<List<Message>>();
            var messageCount = messages.Count();

            for (int i = 0; i < messageCount; i += MaxFilesPerTranscriptionJob)
            {
                var chunk = messages.Skip(i).Take(Math.Min(MaxFilesPerTranscriptionJob, messageCount - i)).ToList();
                chunkedMessages.Add(chunk);
            }

            for (var i = 0; i < chunkedMessages.Count; i++)
            {
                var jobName = $"{startDateTime.ToString("yyyy-MM-ddTHH:mm:ss", CultureInfo.InvariantCulture)}_{i}";
                await StartBatchTranscriptionJobAsync(chunkedMessages.ElementAt(i), jobName, startDateTime).ConfigureAwait(false);
            }
        }

        public async Task StartTranscriptionAsync(Message message)
        {
            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            var busMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body));
            var audioFileName = StorageUtilities.GetFileNameFromUri(busMessage.Data.Url);
            var startDateTime = DateTime.UtcNow;

            // Check if language identification is required:
            var secondaryLocale = StartTranscriptionEnvironmentVariables.SecondaryLocale;
            if (!string.IsNullOrEmpty(secondaryLocale) &&
                !secondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase) &&
                !secondaryLocale.Equals(Locale, StringComparison.OrdinalIgnoreCase))
            {
                secondaryLocale = secondaryLocale.Split('|')[0].Trim();

                Logger.LogInformation($"Primary locale: {Locale}");
                Logger.LogInformation($"Secondary locale: {secondaryLocale}");

                var languageID = new LanguageIdentification(StartTranscriptionEnvironmentVariables.AzureSpeechServicesKey, StartTranscriptionEnvironmentVariables.AzureSpeechServicesRegion);
                var fileExtension = Path.GetExtension(audioFileName);
                var sasUrl = await StorageUtilities.CreateSASAsync(StartTranscriptionEnvironmentVariables.AzureWebJobsStorage, busMessage.Data.Url, Logger).ConfigureAwait(false);
                var byteArray = await StorageUtilities.DownloadFileFromSAS(sasUrl).ConfigureAwait(false);
                var identifiedLocale = await languageID.DetectLanguage(byteArray, fileExtension, Locale, secondaryLocale).ConfigureAwait(false);
                Logger.LogInformation($"Identified locale: {identifiedLocale}");
                Locale = identifiedLocale;
            }

            await StartBatchTranscriptionJobAsync(new[] { message }, audioFileName, startDateTime).ConfigureAwait(false);
        }

        public bool IsValidServiceBusMessage(Message message)
        {
            if (message == null)
            {
                Logger.LogError($"Message {nameof(message)} is null.");
                return false;
            }

            var messageBody = Encoding.UTF8.GetString(message.Body);

            try
            {
                var serviceBusMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(messageBody);

                if (serviceBusMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase) &&
                    StorageUtilities.GetContainerNameFromUri(serviceBusMessage.Data.Url).Equals(StartTranscriptionEnvironmentVariables.AudioInputContainer, StringComparison.Ordinal))
                {
                    return true;
                }
            }
            catch (JsonSerializationException e)
            {
                Logger.LogError($"Exception {e.Message} while parsing message {messageBody} - message will be ignored.");
                return false;
            }

            return false;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Allow general exception catching to retry transcriptions in that case.")]
        private async Task StartBatchTranscriptionJobAsync(IEnumerable<Message> messages, string jobName, DateTime startDateTime)
        {
            if (messages == null || !messages.Any())
            {
                Logger.LogError("Invalid service bus message(s).");
                return;
            }

            var locationString = string.Empty;
            var serviceBusMessages = messages.Select(message => JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body)));
            var modelIds = new List<Guid>();

            try
            {
                var properties = GetTranscriptionPropertyBag();
                modelIds = GetModelIds();

                var audioFileUrls = new List<string>();

                foreach (var serviceBusMessage in serviceBusMessages)
                {
                    var audioFileUrl = await StorageUtilities.CreateSASAsync(StartTranscriptionEnvironmentVariables.AzureWebJobsStorage, serviceBusMessage.Data.Url, Logger).ConfigureAwait(false);
                    audioFileUrls.Add(audioFileUrl);
                }

                var client = new BatchClient(Subscription.SubscriptionKey, Subscription.LocationUri.AbsoluteUri, Logger);
                var transcriptionLocation = await client.PostTranscriptionAsync(
                    jobName,
                    "StartByTimerTranscription",
                    Locale,
                    properties,
                    audioFileUrls,
                    modelIds).ConfigureAwait(false);

                Logger.LogInformation($"Location: {transcriptionLocation}");
                locationString = transcriptionLocation.ToString();
            }
            catch (WebException e)
            {
                if (BatchClient.IsThrottledOrTimeoutStatusCode(((HttpWebResponse)e.Response).StatusCode))
                {
                    Logger.LogError($"Throttled or timeout while creating post, re-enqueueing transcription start. Message: {e.Message}");

                    var startTranscriptionSBConnectionString = StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString;

                    foreach (var message in messages)
                    {
                        ServiceBusUtilities.SendServiceBusMessageAsync(startTranscriptionSBConnectionString, message, Logger, 2).GetAwaiter().GetResult();
                    }

                    return;
                }
                else
                {
                    Logger.LogError($"Failed with Webexception. Write message for job with name {jobName} to report file.");

                    var errorTxtName = jobName + ".txt";
                    var exceptionMessage = e.Message;

                    using (var reader = new StreamReader(e.Response.GetResponseStream()))
                    {
                        var responseMessage = await reader.ReadToEndAsync().ConfigureAwait(false);
                        exceptionMessage += "\n" + responseMessage;
                    }

                    await WriteFailedJobLogToStorageAsync(serviceBusMessages, exceptionMessage, jobName).ConfigureAwait(false);
                    return;
                }
            }
            catch (TimeoutException e)
            {
                Logger.LogError($"Timeout while creating post, re-enqueueing transcription start. Message: {e.Message}");
                var startTranscriptionSBConnectionString = StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString;

                foreach (var message in messages)
                {
                    ServiceBusUtilities.SendServiceBusMessageAsync(startTranscriptionSBConnectionString, message, Logger, 2).GetAwaiter().GetResult();
                }

                return;
            }
            catch (Exception e)
            {
                Logger.LogError($"Failed with Exception {e} and message {e.Message}. Write message for job with name {jobName} to report file.");
                await WriteFailedJobLogToStorageAsync(serviceBusMessages, e.Message, jobName).ConfigureAwait(false);
                return;
            }

            var reenqueueingTimeInSeconds = MapDatasetSizeToReenqueueingTimeInSeconds(StartTranscriptionEnvironmentVariables.AudioDatasetSize);

            var transcriptionMessage = new PostTranscriptionServiceBusMessage(
                Subscription,
                locationString,
                jobName,
                startDateTime.ToString(CultureInfo.InvariantCulture),
                Locale,
                modelIds.Any(),
                StartTranscriptionEnvironmentVariables.AddSentimentAnalysis,
                StartTranscriptionEnvironmentVariables.AddEntityRedaction,
                0,
                reenqueueingTimeInSeconds,
                reenqueueingTimeInSeconds);
            var fetchTranscriptionSBConnectionString = StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString;
            Logger.LogInformation($"FetchTranscriptionServiceBusConnectionString from settings: {fetchTranscriptionSBConnectionString}");

            try
            {
                ServiceBusUtilities.SendServiceBusMessageAsync(fetchTranscriptionSBConnectionString, transcriptionMessage.CreateMessageString(), Logger, reenqueueingTimeInSeconds).GetAwaiter().GetResult();
            }
            catch (Exception e)
            {
                Logger.LogError($"Failed with Exception {e} and message {e.Message}. Write message for job with name {jobName} to report file.");
                await WriteFailedJobLogToStorageAsync(serviceBusMessages, e.Message, jobName).ConfigureAwait(false);
            }

            Logger.LogInformation($"Fetch transcription queue informed about job at: {jobName}");
        }

        private async Task WriteFailedJobLogToStorageAsync(IEnumerable<ServiceBusMessage> serviceBusMessages, string errorMessage, string jobName)
        {
            var errorOutputContainer = StartTranscriptionEnvironmentVariables.ErrorReportOutputContainer;

            var errorFileName = jobName + ".txt";
            await StorageUtilities.WriteTextFileToBlobAsync(StartTranscriptionEnvironmentVariables.AzureWebJobsStorage, errorMessage, errorOutputContainer, errorFileName, Logger).ConfigureAwait(false);

            foreach (var message in serviceBusMessages)
            {
                var fileName = StorageUtilities.GetFileNameFromUri(message.Data.Url);
                errorFileName = fileName + ".txt";
                await StorageUtilities.WriteTextFileToBlobAsync(StartTranscriptionEnvironmentVariables.AzureWebJobsStorage, errorMessage, errorOutputContainer, errorFileName, Logger).ConfigureAwait(false);
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
            properties.Add("DiarizationEnabled", addDiarization);
            Logger.LogInformation($"Setting diarization enabled to {addDiarization}");

            var addWordLevelTimestamps = StartTranscriptionEnvironmentVariables.AddWordLevelTimestamps;
            properties.Add("WordLevelTimestampsEnabled", addWordLevelTimestamps);
            Logger.LogInformation($"Setting word level timestamps enabled to {addWordLevelTimestamps}");

            return properties;
        }

        private List<Guid> GetModelIds()
        {
            var modelIds = new List<Guid>();

            var acousticModelId = StartTranscriptionEnvironmentVariables.AcousticModelId;
            if (!string.IsNullOrEmpty(acousticModelId))
            {
                Logger.LogInformation($"Acoustic model id: {acousticModelId}");
                modelIds.Add(Guid.Parse(acousticModelId));
            }

            var languageModelId = StartTranscriptionEnvironmentVariables.LanguageModelId;

            if (!string.IsNullOrEmpty(languageModelId) && languageModelId != acousticModelId)
            {
                Logger.LogInformation($"Language model id: {languageModelId}");
                modelIds.Add(Guid.Parse(languageModelId));
            }

            return modelIds;
        }

        private int MapDatasetSizeToReenqueueingTimeInSeconds(string audioDatasetSize)
        {
            var reenqueueingTime = 60;

            if (audioDatasetSize.StartsWith("Small", StringComparison.OrdinalIgnoreCase))
            {
                // 2 minutes
                reenqueueingTime = 120;
            }

            if (audioDatasetSize.StartsWith("Medium", StringComparison.OrdinalIgnoreCase))
            {
                // 5 minutes
                reenqueueingTime = 300;
            }

            if (audioDatasetSize.StartsWith("Large", StringComparison.OrdinalIgnoreCase))
            {
                // 10 minutes
                reenqueueingTime = 600;
            }

            if (audioDatasetSize.StartsWith("Very large ", StringComparison.OrdinalIgnoreCase))
            {
                // 15 minutes
                reenqueueingTime = 900;
            }

            Logger.LogInformation($"Re-enqueueing time for messages was set to {reenqueueingTime} seconds.");

            return reenqueueingTime;
        }
    }
}
