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
            var subscriptionKey = CreateEnvironmentVariables.AzureSpeechServicesKey;
            var subscriptionRegion = CreateEnvironmentVariables.AzureSpeechServicesRegion;
            var subscription = new Subscription(subscriptionKey, subscriptionRegion);

            var messageBody = Encoding.UTF8.GetString(message.Body);
            var busMessage = JsonConvert.DeserializeObject<ServiceBusMessage>(messageBody);

            var dataUri = busMessage.Data.Url;
            var name = dataUri.AbsoluteUri.Split('/').LastOrDefault();

            if (busMessage.EventType.Contains("BlobCreate", StringComparison.OrdinalIgnoreCase))
            {
                (var audioFileUrl, var containerName, var blobName) = StorageUtilities.CreateSAS(CreateEnvironmentVariables.AzureWebJobsStorage, dataUri.AbsoluteUri, log);
                var audioInputContainer = CreateEnvironmentVariables.AudioInputContainer;

                if (StorageUtilities.IsItAudio(blobName) && !string.IsNullOrEmpty(containerName) && containerName.Equals(audioInputContainer, StringComparison.Ordinal))
                {
                    log.LogInformation($"Create request for audio file {blobName}");

                    var fileExtension = Path.GetExtension(blobName);
                    log.LogInformation("Determined file name extension:" + fileExtension);

                    var primaryLocale = CreateEnvironmentVariables.Locale;
                    primaryLocale = primaryLocale.Split('|')[0].Trim();
                    var locale = primaryLocale;

                    string locationString;
                    byte[] byteArray;
                    try
                    {
                        byteArray = await StorageUtilities.DownloadFileFromSAS(audioFileUrl).ConfigureAwait(false);

                        var secondaryLocale = CreateEnvironmentVariables.SecondaryLocale;

                        if (!string.IsNullOrEmpty(secondaryLocale) &&
                            !secondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase) &&
                            !secondaryLocale.Equals(primaryLocale, StringComparison.OrdinalIgnoreCase))
                        {
                            secondaryLocale = secondaryLocale.Split('|')[0].Trim();

                            log.LogInformation($"Primary locale: {primaryLocale}");
                            log.LogInformation($"Secondary locale: {secondaryLocale}");

                            var languageID = new LanguageIdentification(subscriptionKey, subscriptionRegion);
                            locale = await languageID.DetectLanguage(byteArray, fileExtension, primaryLocale, secondaryLocale).ConfigureAwait(false);
                            log.LogInformation($"Identified locale: {locale}");
                        }

                        var modelIds = GetModelIds(locale.Equals(primaryLocale, StringComparison.OrdinalIgnoreCase), log);

                        var properties = CreateTranscriptionPropertyBag();
                        var location = await TranscriptionAPIRequest.PostRequest(subscription, blobName, audioFileUrl, locale, properties, modelIds, log).ConfigureAwait(false);
                        log.LogInformation("Location:" + location);
                        locationString = location.ToString();
                    }
                    catch (WebException e)
                    {
                        if (BatchClient.IsThrottledOrTimeoutStatusCode(((HttpWebResponse)e.Response).StatusCode))
                        {
                            log.LogError($"Throttled or timeout while creating post, re-enqueueing transcription create. Message: {e.Message}");

                            var createTranscriptionSBConnectionString = CreateEnvironmentVariables.CreateTranscriptionServiceBusConnectionString;
                            log.LogInformation($"CreateTranscriptionSBConnectionString from settings: {createTranscriptionSBConnectionString}");
                            ServiceBusUtilities.SendServiceBusMessageAsync(createTranscriptionSBConnectionString, message, log, 2).GetAwaiter().GetResult();
                            return string.Empty;
                        }
                        else
                        {
                            log.LogError($"Failed with Webexception. Write message for {blobName} to report file.");

                            var blobNameWithoutExtension = blobName.Replace(fileExtension, string.Empty, StringComparison.OrdinalIgnoreCase);

                            var errorTxtName = blobNameWithoutExtension + "_error.txt";
                            var exceptionMessage = e.Message;

                            using (var reader = new StreamReader(e.Response.GetResponseStream()))
                            {
                                var responseMessage = await reader.ReadToEndAsync().ConfigureAwait(false);
                                exceptionMessage += "\n" + responseMessage;
                            }

                            var errorOutputContainer = CreateEnvironmentVariables.ErrorReportOutputContainer;
                            StorageUtilities.WriteTextFileToBlob(CreateEnvironmentVariables.AzureWebJobsStorage, exceptionMessage, errorOutputContainer, errorTxtName, log);
                        }

                        throw;
                    }
                    catch (FormatException e)
                    {
                        log.LogError($"Failed with FormatException. Write message for {blobName} to report file.");

                        var blobNameWithoutExtension = blobName.Replace(fileExtension, string.Empty, StringComparison.OrdinalIgnoreCase);

                        var errorTxtName = blobNameWithoutExtension + "_error.txt";
                        var responseMessage = $"Transcription failed with FormatException:\n{e.Message}";

                        var errorOutputContainer = CreateEnvironmentVariables.ErrorReportOutputContainer;
                        StorageUtilities.WriteTextFileToBlob(CreateEnvironmentVariables.AzureWebJobsStorage, responseMessage, errorOutputContainer, errorTxtName, log);

                        return string.Empty;
                    }
                    catch (TimeoutException e)
                    {
                        log.LogError($"Timeout while creating post, re-enqueueing transcription create. Message: {e.Message}");
                        var createTranscriptionSBConnectionString = CreateEnvironmentVariables.CreateTranscriptionServiceBusConnectionString;
                        ServiceBusUtilities.SendServiceBusMessageAsync(createTranscriptionSBConnectionString, message, log, 2).GetAwaiter().GetResult();
                        return string.Empty;
                    }

                    var receiptsContainerName = CreateEnvironmentVariables.ReceiptsContainer;
                    var audioDetails = await StorageUtilities.WriteTranscriptionAcknowledgementToBlob(CreateEnvironmentVariables.AzureWebJobsStorage, byteArray, locale, receiptsContainerName, blobName, log).ConfigureAwait(false);
                    log.LogInformation("Receipt created for flie: " + blobName);

                    var reenqueueingTimeInSeconds = MapDatasetSizeToReenqueueingTimeInSeconds(CreateEnvironmentVariables.AudioDatasetSize);

                    var transcriptionMessage = new TranscriptionServiceBusMessage(
                        subscription,
                        locationString,
                        blobName,
                        audioFileUrl,
                        audioDetails.CreatedTime.ToString(CultureInfo.InvariantCulture),
                        audioDetails.AudioLength.ToString(),
                        audioDetails.Channels,
                        audioDetails.EstimatedCost,
                        audioDetails.Locale,
                        0,
                        reenqueueingTimeInSeconds);
                    var fetchTranscriptionSBConnectionString = CreateEnvironmentVariables.FetchTranscriptionServiceBusConnectionString;
                    log.LogInformation($"FetchTranscriptionServiceBusConnectionString from settings: {fetchTranscriptionSBConnectionString}");

                    // Delay re-enqueueing for first start to the total length of the audio file
                    var delay = audioDetails.AudioLength.TotalSeconds <= 10 ? new Random().Next(0, reenqueueingTimeInSeconds) : audioDetails.AudioLength.TotalSeconds;

                    ServiceBusUtilities.SendServiceBusMessageAsync(fetchTranscriptionSBConnectionString, transcriptionMessage.CreateMessageString(), log, delay).GetAwaiter().GetResult();
                    log.LogInformation("Fetch transcription queue informed about file: " + blobName);

                    return $"{locationString}*{name}";
                }
                else
                {
                    log.LogInformation($"File is not in supported audio format: {blobName}");
                }
            }

            return string.Empty;
        }

        internal static List<Guid> GetModelIds(bool isPrimaryLocale, ILogger log)
        {
            var modelIds = new List<Guid>();

            var acousticModelId = isPrimaryLocale ?
                CreateEnvironmentVariables.AcousticModelId :
                CreateEnvironmentVariables.SecondaryAcousticModelId;

            if (!string.IsNullOrEmpty(acousticModelId))
            {
                log.LogInformation($"Acoustic model id: {acousticModelId}");
                modelIds.Add(Guid.Parse(acousticModelId));
            }

            var languageModelId = isPrimaryLocale ?
                CreateEnvironmentVariables.LanguageModelId :
                CreateEnvironmentVariables.SecondaryLanguageModelId;

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

            var profanityFilterMode = CreateEnvironmentVariables.ProfanityFilterMode;
            properties.Add("ProfanityFilterMode", profanityFilterMode);

            var punctuationMode = CreateEnvironmentVariables.PunctuationMode;
            punctuationMode = punctuationMode.Replace(" ", string.Empty, StringComparison.Ordinal);
            properties.Add("PunctuationMode", punctuationMode);

            var addDiarization = CreateEnvironmentVariables.AddDiarization;
            properties.Add("DiarizationEnabled", addDiarization);

            var addWordLevelTimestamps = CreateEnvironmentVariables.AddWordLevelTimestamps;
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
