// <copyright file="CreateTranscriptionByTimer.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace CreateByTimerFunction
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
    using Microsoft.Azure.ServiceBus.Core;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class CreateTranscriptionByTimer
    {
        private const int MaxMessagesPerRequest = 500;

        private const int MaxFilesPerTranscriptionJob = 500;

        private static DateTime StartDateTime;

        private static ILogger Logger;

        [FunctionName("CreateTranscriptionByTimer")]
        public static async Task Run([TimerTrigger("0 */2 * * * *")] TimerInfo myTimer, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            if (myTimer == null)
            {
                throw new ArgumentNullException(nameof(myTimer));
            }

            Logger = log;
            StartDateTime = DateTime.UtcNow;
            log.LogInformation($"C# Timer trigger function v3 executed at: {StartDateTime}. Next occurrence on {myTimer.Schedule.GetNextOccurrence(StartDateTime)}.");

            if (!string.IsNullOrEmpty(CreateTranscriptionEnvironmentVariables.SecondaryLocale) && !CreateTranscriptionEnvironmentVariables.SecondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase))
            {
                log.LogWarning($"Language identification is not supported for timer-based transcription creation.");
            }

            var receiver = new MessageReceiver(CreateTranscriptionEnvironmentVariables.AzureServiceBus, "create_transcription_queue");

            var totalServiceBusMessages = new List<Message>();

            var serviceBusMessagesRemaining = true;
            log.LogInformation("Pulling messages from queue...");
            do
            {
                var messages = await receiver.ReceiveAsync(MaxMessagesPerRequest, TimeSpan.FromSeconds(20)).ConfigureAwait(false);

                if (messages == null)
                {
                    log.LogInformation("No remaining messages found in queue.");
                    break;
                }

                foreach (var message in messages)
                {
                    await receiver.CompleteAsync(message.SystemProperties.LockToken).ConfigureAwait(false);
                    totalServiceBusMessages.Add(message);
                }

                if (messages.Count < MaxMessagesPerRequest)
                {
                    serviceBusMessagesRemaining = false;
                }
            }
            while (serviceBusMessagesRemaining);

            log.LogInformation($"Received {totalServiceBusMessages.Count} messages in total from queue.");

            var validMessages = totalServiceBusMessages.Where(message => IsValidServiceBusMessage(message));

            if (!validMessages.Any())
            {
                log.LogInformation("No valid messages were found in this function execution.");
                return;
            }

            var validMessageCount = validMessages.Count();

            log.LogInformation($"{validMessageCount} valid messages were found in this function execution.");
            var chunkedMessages = new List<List<Message>>();

            for (int i = 0; i < validMessageCount; i += MaxFilesPerTranscriptionJob)
            {
                var chunk = validMessages.Skip(i).Take(Math.Min(MaxFilesPerTranscriptionJob, validMessageCount - i)).ToList();
                chunkedMessages.Add(chunk);
            }

            for (var i = 0; i < chunkedMessages.Count; i++)
            {
                await StartTranscriptionJob(chunkedMessages.ElementAt(i), log, i).ConfigureAwait(false);
            }
        }

        internal static async Task<string> StartTranscriptionJob(IEnumerable<Message> messages, ILogger log, int index)
        {
            var locationString = string.Empty;
            var subscriptionKey = CreateTranscriptionEnvironmentVariables.AzureSpeechServicesKey;
            var subscriptionRegion = CreateTranscriptionEnvironmentVariables.AzureSpeechServicesRegion;
            var locationUri = new Uri($"https://{subscriptionRegion}.api.cognitive.microsoft.com");
            var subscription = new Subscription(subscriptionKey, locationUri);
            var jobName = $"{StartDateTime.ToString("yyyy-MM-ddTHH:mm:ss", CultureInfo.InvariantCulture)}_{index}";
            var locale = CreateTranscriptionEnvironmentVariables.Locale.Split('|')[0].Trim();

            var serviceBusMessages = messages.Select(message => JsonConvert.DeserializeObject<ServiceBusMessage>(Encoding.UTF8.GetString(message.Body)));
            var modelIds = new List<Guid>();

            try
            {
                var properties = CreateTranscriptionPropertyBag();
                modelIds = GetModelIds(log);

                var audioFileUrls = new List<Uri>();

                foreach (var serviceBusMessage in serviceBusMessages)
                {
                    var audioFileUrl = await StorageUtilities.CreateSASAsync(CreateTranscriptionEnvironmentVariables.AzureWebJobsStorage, serviceBusMessage.Data.Url.AbsoluteUri, log).ConfigureAwait(false);
                    audioFileUrls.Add(new Uri(audioFileUrl));
                }

                if (CreateTranscriptionEnvironmentVariables.CreateReceiptFile)
                {
                    foreach (var audioFileUrl in audioFileUrls)
                    {
                        var audioFileName = StorageUtilities.GetFileNameFromPath(audioFileUrl.AbsoluteUri);

                        var byteArray = await StorageUtilities.DownloadFileFromSAS(audioFileUrl.AbsoluteUri).ConfigureAwait(false);
                        var audioDetails = StorageUtilities.GetAudioDetails(
                            byteArray,
                            audioFileName,
                            locale,
                            StartDateTime,
                            modelIds.Any(),
                            CreateTranscriptionEnvironmentVariables.AddSentimentAnalysis,
                            CreateTranscriptionEnvironmentVariables.AddEntityRedaction,
                            log);

                        await StorageUtilities.WriteTranscriptionReceiptToStorageAsync(CreateTranscriptionEnvironmentVariables.AzureWebJobsStorage, CreateTranscriptionEnvironmentVariables.ReceiptsContainer, audioDetails, log).ConfigureAwait(false);
                    }
                }

                var client = new BatchClient(subscription.SubscriptionKey, subscription.LocationUri.AbsoluteUri, log);
                var transcriptionLocation = await client.PostTranscriptionAsync(
                    jobName,
                    "CreateByTimerTranscription",
                    locale,
                    properties,
                    audioFileUrls,
                    modelIds).ConfigureAwait(false);

                log.LogInformation($"Location: {transcriptionLocation}");
                locationString = transcriptionLocation.ToString();
            }
            catch (WebException e)
            {
                if (BatchClient.IsThrottledOrTimeoutStatusCode(((HttpWebResponse)e.Response).StatusCode))
                {
                    log.LogError($"Throttled or timeout while creating post, re-enqueueing transcription create. Message: {e.Message}");

                    var createTranscriptionSBConnectionString = CreateTranscriptionEnvironmentVariables.CreateTranscriptionServiceBusConnectionString;
                    log.LogInformation($"CreateTranscriptionSBConnectionString from settings: {createTranscriptionSBConnectionString}");

                    foreach (var message in messages)
                    {
                        ServiceBusUtilities.SendServiceBusMessageAsync(createTranscriptionSBConnectionString, message, log, 2).GetAwaiter().GetResult();
                    }

                    return string.Empty;
                }
                else
                {
                    log.LogError($"Failed with Webexception. Write message for job with name {jobName} to report file.");

                    var errorTxtName = jobName + "_error.txt";
                    var exceptionMessage = e.Message;

                    using (var reader = new StreamReader(e.Response.GetResponseStream()))
                    {
                        var responseMessage = await reader.ReadToEndAsync().ConfigureAwait(false);
                        exceptionMessage += "\n" + responseMessage;
                    }

                    var errorOutputContainer = CreateTranscriptionEnvironmentVariables.ErrorReportOutputContainer;
                    await StorageUtilities.WriteTextFileToBlobAsync(CreateTranscriptionEnvironmentVariables.AzureWebJobsStorage, exceptionMessage, errorOutputContainer, errorTxtName, log).ConfigureAwait(false);
                }

                throw;
            }
            catch (FormatException e)
            {
                log.LogError($"Failed with FormatException. Write message for job with name {jobName} to report file.");

                var errorTxtName = jobName + "_error.txt";
                var responseMessage = $"Transcription failed with FormatException:\n{e.Message}";

                var errorOutputContainer = CreateTranscriptionEnvironmentVariables.ErrorReportOutputContainer;
                await StorageUtilities.WriteTextFileToBlobAsync(CreateTranscriptionEnvironmentVariables.AzureWebJobsStorage, responseMessage, errorOutputContainer, errorTxtName, log).ConfigureAwait(false);

                return string.Empty;
            }
            catch (TimeoutException e)
            {
                log.LogError($"Timeout while creating post, re-enqueueing transcription create. Message: {e.Message}");
                var createTranscriptionSBConnectionString = CreateTranscriptionEnvironmentVariables.CreateTranscriptionServiceBusConnectionString;

                foreach (var message in messages)
                {
                    ServiceBusUtilities.SendServiceBusMessageAsync(createTranscriptionSBConnectionString, message, log, 2).GetAwaiter().GetResult();
                }

                return string.Empty;
            }

            var reenqueueingTimeInSeconds = MapDatasetSizeToReenqueueingTimeInSeconds(CreateTranscriptionEnvironmentVariables.AudioDatasetSize);

            var transcriptionMessage = new PostTranscriptionServiceBusMessage(
                subscription,
                locationString,
                jobName,
                StartDateTime.ToString(CultureInfo.InvariantCulture),
                locale,
                modelIds.Any(),
                CreateTranscriptionEnvironmentVariables.AddSentimentAnalysis,
                CreateTranscriptionEnvironmentVariables.AddEntityRedaction,
                0,
                reenqueueingTimeInSeconds,
                reenqueueingTimeInSeconds);
            var fetchTranscriptionSBConnectionString = CreateTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString;
            log.LogInformation($"FetchTranscriptionServiceBusConnectionString from settings: {fetchTranscriptionSBConnectionString}");

            ServiceBusUtilities.SendServiceBusMessageAsync(fetchTranscriptionSBConnectionString, transcriptionMessage.CreateMessageString(), log, reenqueueingTimeInSeconds).GetAwaiter().GetResult();
            log.LogInformation($"Fetch transcription queue informed about job at: {jobName}");

            return locationString;
        }

        internal static bool IsValidServiceBusMessage(Message message)
        {
            var messageBody = Encoding.UTF8.GetString(message.Body);

            try
            {
                var serviceBusMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(messageBody);

                if (serviceBusMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase) &&
                    StorageUtilities.IsItAudio(serviceBusMessage.Data.Url.AbsoluteUri) &&
                    StorageUtilities.GetContainerNameFromPath(serviceBusMessage.Data.Url.AbsoluteUri).Equals(CreateTranscriptionEnvironmentVariables.AudioInputContainer, StringComparison.Ordinal))
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

        internal static List<Guid> GetModelIds(ILogger log)
        {
            var modelIds = new List<Guid>();

            var acousticModelId = CreateTranscriptionEnvironmentVariables.AcousticModelId;
            if (!string.IsNullOrEmpty(acousticModelId))
            {
                log.LogInformation($"Acoustic model id: {acousticModelId}");
                modelIds.Add(Guid.Parse(acousticModelId));
            }

            var languageModelId = CreateTranscriptionEnvironmentVariables.LanguageModelId;

            if (!string.IsNullOrEmpty(languageModelId) && languageModelId != acousticModelId)
            {
                log.LogInformation($"Language model id: {languageModelId}");
                modelIds.Add(Guid.Parse(languageModelId));
            }

            return modelIds;
        }

        internal static Dictionary<string, string> CreateTranscriptionPropertyBag()
        {
            var properties = new Dictionary<string, string>();

            var profanityFilterMode = CreateTranscriptionEnvironmentVariables.ProfanityFilterMode;
            properties.Add("ProfanityFilterMode", profanityFilterMode);

            var punctuationMode = CreateTranscriptionEnvironmentVariables.PunctuationMode;
            punctuationMode = punctuationMode.Replace(" ", string.Empty, StringComparison.Ordinal);
            properties.Add("PunctuationMode", punctuationMode);

            var addDiarization = CreateTranscriptionEnvironmentVariables.AddDiarization;
            properties.Add("DiarizationEnabled", addDiarization);

            var addWordLevelTimestamps = CreateTranscriptionEnvironmentVariables.AddWordLevelTimestamps;
            properties.Add("WordLevelTimestampsEnabled", addWordLevelTimestamps);

            return properties;
        }

        internal static int MapDatasetSizeToReenqueueingTimeInSeconds(string audioDatasetSize)
        {
            if (audioDatasetSize.StartsWith("Small", StringComparison.OrdinalIgnoreCase))
            {
                // 2 minutes
                return 120;
            }

            if (audioDatasetSize.StartsWith("Medium", StringComparison.OrdinalIgnoreCase))
            {
                // 5 minutes
                return 300;
            }

            if (audioDatasetSize.StartsWith("Large", StringComparison.OrdinalIgnoreCase))
            {
                // 10 minutes
                return 600;
            }

            if (audioDatasetSize.StartsWith("Very large ", StringComparison.OrdinalIgnoreCase))
            {
                // 15 minutes
                return 900;
            }

            return 1;
        }
    }
}
