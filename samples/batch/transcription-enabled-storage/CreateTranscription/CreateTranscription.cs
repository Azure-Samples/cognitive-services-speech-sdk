// <copyright file="CreateTranscription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace CreateTranscriptionFunction
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
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class CreateTranscription
    {
        [FunctionName("CreateTranscription")]
        public static void Run([ServiceBusTrigger("create_transcription_queue", Connection = "AzureServiceBus")]Message message, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            log.LogInformation($"C# ServiceBus queue trigger function processed message: {message.Label}");
            log.LogInformation($"Received message: SequenceNumber:{message.SystemProperties.SequenceNumber} Body:{Encoding.UTF8.GetString(message.Body)}");

            var location = TranscribeMessage(message, log).Result;
            log.LogInformation("Request Created, Guid: " + location);
        }

        internal static async Task<string> TranscribeMessage(Message message, ILogger log)
        {
            var transcriptionCreationTime = DateTime.UtcNow;
            var subscriptionKey = CreateTranscriptionEnvironmentVariables.AzureSpeechServicesKey;
            var subscriptionRegion = CreateTranscriptionEnvironmentVariables.AzureSpeechServicesRegion;
            var locationUri = new Uri($"https://{subscriptionRegion}.api.cognitive.microsoft.com");
            var subscription = new Subscription(subscriptionKey, locationUri);

            var messageBody = Encoding.UTF8.GetString(message.Body);
            var busMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(messageBody);

            var dataUri = busMessage.Data.Url;
            var modelIds = new List<Guid>();

            if (busMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase))
            {
                var audioFileUrl = await StorageUtilities.CreateSASAsync(CreateTranscriptionEnvironmentVariables.AzureWebJobsStorage, dataUri.AbsoluteUri, log).ConfigureAwait(false);
                var containerName = StorageUtilities.GetContainerNameFromPath(dataUri.AbsoluteUri);
                var audioFileName = StorageUtilities.GetFileNameFromPath(dataUri.AbsoluteUri);
                var audioInputContainer = CreateTranscriptionEnvironmentVariables.AudioInputContainer;

                if (StorageUtilities.IsItAudio(audioFileName) && !string.IsNullOrEmpty(containerName) && containerName.Equals(audioInputContainer, StringComparison.Ordinal))
                {
                    log.LogInformation($"Create request for audio file {audioFileName}");

                    var jobName = StorageUtilities.GetFileNameWithoutExtension(audioFileName);

                    var primaryLocale = CreateTranscriptionEnvironmentVariables.Locale;
                    primaryLocale = primaryLocale.Split('|')[0].Trim();
                    var locale = primaryLocale;

                    string locationString;
                    byte[] byteArray = null;

                    try
                    {
                        var secondaryLocale = CreateTranscriptionEnvironmentVariables.SecondaryLocale;

                        if (!string.IsNullOrEmpty(secondaryLocale) &&
                            !secondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase) &&
                            !secondaryLocale.Equals(primaryLocale, StringComparison.OrdinalIgnoreCase))
                        {
                            secondaryLocale = secondaryLocale.Split('|')[0].Trim();

                            log.LogInformation($"Primary locale: {primaryLocale}");
                            log.LogInformation($"Secondary locale: {secondaryLocale}");

                            var languageID = new LanguageIdentification(subscriptionKey, subscriptionRegion);
                            var fileExtension = Path.GetExtension(audioFileName);
                            byteArray = await StorageUtilities.DownloadFileFromSAS(audioFileUrl).ConfigureAwait(false);
                            locale = await languageID.DetectLanguage(byteArray, fileExtension, primaryLocale, secondaryLocale).ConfigureAwait(false);
                            log.LogInformation($"Identified locale: {locale}");
                        }

                        modelIds = GetModelIds(locale.Equals(primaryLocale, StringComparison.OrdinalIgnoreCase), log);
                        var properties = CreateTranscriptionPropertyBag();

                        var client = new BatchClient(subscription.SubscriptionKey, subscription.LocationUri.AbsoluteUri, log);
                        var transcriptionLocation = await client.PostTranscriptionAsync(
                            jobName,
                            "AcceleratorTranscription",
                            locale,
                            properties,
                            new[] { new Uri(audioFileUrl) }.ToList(),
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
                            ServiceBusUtilities.SendServiceBusMessageAsync(createTranscriptionSBConnectionString, message, log, 2).GetAwaiter().GetResult();
                            return string.Empty;
                        }
                        else
                        {
                            log.LogError($"Failed with Webexception. Write message for {jobName} to report file.");

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
                        log.LogError($"Failed with FormatException. Write message for {jobName} to report file.");
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
                        ServiceBusUtilities.SendServiceBusMessageAsync(createTranscriptionSBConnectionString, message, log, 2).GetAwaiter().GetResult();
                        return string.Empty;
                    }

                    var audioLengthInSeconds = 0;

                    if (CreateTranscriptionEnvironmentVariables.CreateReceiptFile)
                    {
                        if (byteArray == null)
                        {
                            byteArray = await StorageUtilities.DownloadFileFromSAS(audioFileUrl).ConfigureAwait(false);
                        }

                        var audioDetails = StorageUtilities.GetAudioDetails(
                            byteArray,
                            audioFileName,
                            locale,
                            transcriptionCreationTime,
                            modelIds.Any(),
                            CreateTranscriptionEnvironmentVariables.AddSentimentAnalysis,
                            CreateTranscriptionEnvironmentVariables.AddEntityRedaction,
                            log);
                        audioLengthInSeconds = (int)audioDetails.AudioLength.TotalSeconds;
                        await StorageUtilities.WriteTranscriptionReceiptToStorageAsync(CreateTranscriptionEnvironmentVariables.AzureWebJobsStorage, CreateTranscriptionEnvironmentVariables.ReceiptsContainer, audioDetails, log).ConfigureAwait(false);
                        log.LogInformation("Receipt created for flie: " + jobName);
                    }

                    var reenqueueingTimeInSeconds = MapDatasetSizeToReenqueueingTimeInSeconds(CreateTranscriptionEnvironmentVariables.AudioDatasetSize);

                    // Increase delay if audio file is very short:
                    var initialDelayInSeconds = audioLengthInSeconds <= 10 ? reenqueueingTimeInSeconds : audioLengthInSeconds;

                    var transcriptionMessage = new PostTranscriptionServiceBusMessage(
                        subscription,
                        locationString,
                        jobName,
                        transcriptionCreationTime.ToString(CultureInfo.InvariantCulture),
                        locale,
                        modelIds.Any(),
                        CreateTranscriptionEnvironmentVariables.AddSentimentAnalysis,
                        CreateTranscriptionEnvironmentVariables.AddEntityRedaction,
                        0,
                        reenqueueingTimeInSeconds,
                        initialDelayInSeconds);
                    var fetchTranscriptionSBConnectionString = CreateTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString;
                    log.LogInformation($"FetchTranscriptionServiceBusConnectionString from settings: {fetchTranscriptionSBConnectionString}");

                    ServiceBusUtilities.SendServiceBusMessageAsync(fetchTranscriptionSBConnectionString, transcriptionMessage.CreateMessageString(), log, initialDelayInSeconds).GetAwaiter().GetResult();
                    log.LogInformation("Fetch transcription queue informed about file: " + jobName);

                    return $"{locationString}";
                }
                else
                {
                    log.LogInformation($"File is not in supported audio format: {audioFileName}");
                }
            }

            return string.Empty;
        }

        internal static List<Guid> GetModelIds(bool isPrimaryLocale, ILogger log)
        {
            var modelIds = new List<Guid>();

            var acousticModelId = isPrimaryLocale ?
                CreateTranscriptionEnvironmentVariables.AcousticModelId :
                CreateTranscriptionEnvironmentVariables.SecondaryAcousticModelId;

            if (!string.IsNullOrEmpty(acousticModelId))
            {
                log.LogInformation($"Acoustic model id: {acousticModelId}");
                modelIds.Add(Guid.Parse(acousticModelId));
            }

            var languageModelId = isPrimaryLocale ?
                CreateTranscriptionEnvironmentVariables.LanguageModelId :
                CreateTranscriptionEnvironmentVariables.SecondaryLanguageModelId;

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
