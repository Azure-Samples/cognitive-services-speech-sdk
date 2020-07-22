// <copyright file="TranscriptionProcessor.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Net;
    using System.Threading.Tasks;
    using Connector;
    using Connector.Enums;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class TranscriptionProcessor
    {
        public static async Task<bool> GetTranscripts(TranscriptionServiceBusMessage serviceBusMessage, ILogger log)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            var fileName = serviceBusMessage.BlobName;
            var transcriptionId = serviceBusMessage.TranscriptionLocation.Split('/').LastOrDefault();
            var transcriptionGuid = new Guid(transcriptionId);
            log.LogInformation($"Received guid {transcriptionGuid} from service bus message.");

            var serviceBusConnectionString = FetchEnvironmentVariables.FetchTranscriptionServiceBusConnectionString;
            log.LogInformation($"ServiceBusQueueConnectionString from settings: {serviceBusConnectionString}");

            var reenqueueingTimeInSeconds = serviceBusMessage.ReenqueueingTimeInSeconds;
            log.LogInformation($"Re-enqueueing time for messages: {reenqueueingTimeInSeconds} seconds.");

            log.LogInformation($"Subscription location: {serviceBusMessage.Subscription.LocationUri.AbsoluteUri}");
            log.LogInformation("Local file name: " + fileName);

            var client = new BatchClient(serviceBusMessage.Subscription.SubscriptionKey, serviceBusMessage.Subscription.LocationUri.AbsoluteUri, log);

            try
            {
                var transcription = await client.GetTranscriptionAsync(transcriptionGuid).ConfigureAwait(false);
                switch (transcription.Status)
                {
                    case "Failed":
                        await ProcessFailedTranscriptionAsync(client, transcription, transcriptionGuid, fileName, log).ConfigureAwait(false);
                        break;
                    case "Succeeded":
                        await ProcessSucceededTranscriptionAsync(client, serviceBusMessage, transcriptionGuid, fileName, log).ConfigureAwait(false);
                        break;
                    case "Running":
                        var runningMessage = serviceBusMessage.RetryMessage();
                        log.LogInformation("Transcription running, retrying message - retry count: " + runningMessage.RetryCount);
                        ServiceBusUtilities.SendServiceBusMessageAsync(serviceBusConnectionString, runningMessage.CreateMessageString(), log, reenqueueingTimeInSeconds).GetAwaiter().GetResult();
                        break;
                    case "NotStarted":
                        var notStartedMessage = serviceBusMessage.RetryMessage();

                        var audioLengthInSeconds = Convert.ToInt32(TimeSpan.Parse(serviceBusMessage.AudioLength, CultureInfo.InvariantCulture).TotalSeconds);

                        // If the transcription is not started yet, the job will take at least length of the audio:
                        var notStartedReenqueueingTime = Math.Max(audioLengthInSeconds, reenqueueingTimeInSeconds);

                        log.LogInformation("Transcription not started, retrying message - retry count: " + notStartedMessage.RetryCount);
                        ServiceBusUtilities.SendServiceBusMessageAsync(serviceBusConnectionString, notStartedMessage.CreateMessageString(), log, notStartedReenqueueingTime).GetAwaiter().GetResult();
                        break;
                }
            }
            catch (WebException e)
            {
                if (BatchClient.IsThrottledOrTimeoutStatusCode(((HttpWebResponse)e.Response).StatusCode))
                {
                    var timeoutMessage = serviceBusMessage.RetryMessage();
                    log.LogInformation("Timeout or throttled, retrying message - retry count: " + timeoutMessage.RetryCount);
                    ServiceBusUtilities.SendServiceBusMessageAsync(serviceBusConnectionString, timeoutMessage.CreateMessageString(), log, reenqueueingTimeInSeconds).GetAwaiter().GetResult();
                    return false;
                }

                throw;
            }
            catch (TimeoutException e)
            {
                var timeoutMessage = serviceBusMessage.RetryMessage();
                log.LogInformation($"Timeout - re-enqueueing fetch transcription message. Exception message: {e.Message}");
                ServiceBusUtilities.SendServiceBusMessageAsync(serviceBusConnectionString, timeoutMessage.CreateMessageString(), log, reenqueueingTimeInSeconds).GetAwaiter().GetResult();
                return false;
            }

            return true;
        }

        internal static async Task ProcessFailedTranscriptionAsync(BatchClient client, Transcription transcription, Guid transcriptionId, string fileName, ILogger log)
        {
            log.LogInformation($"Got failed transcription for file {fileName}");
            var ext = Path.GetExtension(fileName);
            log.LogInformation("Determined file name extension:" + ext);
            var fileNameWithoutExtension = fileName.Replace(ext, string.Empty, StringComparison.OrdinalIgnoreCase);

            var report = "Unknown Error.";
            var errorTxtname = fileNameWithoutExtension + "_error.txt";

            var transcriptionFiles = await client.GetTranscriptionFilesAsync(transcriptionId).ConfigureAwait(false);

            var reportFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.TranscriptionReport).FirstOrDefault();
            if (reportFile?.Links?.ContentUrl != null)
            {
                using var webClient = new WebClient();
                report = webClient.DownloadString(reportFile.Links.ContentUrl);
            }
            else if (transcription.Properties?.Error?.Message != null)
            {
                report = transcription.Properties.Error.Message;
                report += string.IsNullOrEmpty(transcription.Properties.Error.Code) ? string.Empty : $" (Error: {transcription.Properties.Error.Code}).";
            }

            var errorOutputContainer = FetchEnvironmentVariables.ErrorReportOutputContainer;
            StorageUtilities.WriteTextFileToBlob(FetchEnvironmentVariables.AzureWebJobsStorage, report, errorOutputContainer, errorTxtname, log);

            await client.DeleteTranscriptionWithRetryAsync(transcriptionId).ConfigureAwait(false);
        }

        internal static async Task ProcessSucceededTranscriptionAsync(BatchClient client, TranscriptionServiceBusMessage serviceBusMessage, Guid transcriptionId, string fileName, ILogger log)
        {
            log.LogInformation($"Got succeeded transcription for file {fileName}");
            var ext = Path.GetExtension(fileName);
            log.LogInformation("Determined file name extension:" + ext);
            var fileNameWithoutExtension = fileName.Replace(ext, string.Empty, StringComparison.OrdinalIgnoreCase);
            var jsonContainer = FetchEnvironmentVariables.JsonResultOutputContainer;
            var htmlContainer = FetchEnvironmentVariables.HtmlResultOutputContainer;

            var transcriptionFiles = await client.GetTranscriptionFilesAsync(transcriptionId).ConfigureAwait(false);
            var resultFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.Transcription).FirstOrDefault();

            var transcriptionResultJson = string.Empty;
            using (var webClient = new WebClient())
            {
                transcriptionResultJson = webClient.DownloadString(resultFile.Links.ContentUrl);
            }

            var transcriptionResult = JsonConvert.DeserializeObject<SpeechTranscript>(transcriptionResultJson);

            var textAnalyticsKey = FetchEnvironmentVariables.TextAnalyticsKey;
            var textAnalyticsRegion = FetchEnvironmentVariables.TextAnalyticsRegion;

            var textAnalyticsErrors = new List<string>();

            // Perform text analytics
            if (!string.IsNullOrEmpty(textAnalyticsKey) && !string.IsNullOrEmpty(textAnalyticsRegion) && !textAnalyticsRegion.Equals("none", StringComparison.OrdinalIgnoreCase))
            {
                var textAnalytics = new TextAnalytics(serviceBusMessage.Locale, textAnalyticsKey, textAnalyticsRegion, log);

                var addSentimentAnalysis = bool.Parse(FetchEnvironmentVariables.AddSentimentAnalysis);
                if (addSentimentAnalysis)
                {
                    var sentimentErrors = await textAnalytics.AddSentimentToTranscriptAsync(transcriptionResult).ConfigureAwait(false);
                    textAnalyticsErrors.AddRange(sentimentErrors);
                }

                var addEntityRedaction = bool.Parse(FetchEnvironmentVariables.AddEntityRedaction);
                if (addEntityRedaction)
                {
                    var entityRedactionErrors = await textAnalytics.RedactEntitiesAsync(transcriptionResult).ConfigureAwait(false);
                    textAnalyticsErrors.AddRange(entityRedactionErrors);
                }
            }

            var editedTranscriptionResultJson = JsonConvert.SerializeObject(transcriptionResult, Formatting.Indented);

            // Store transcript as is
            var jsonFileName = fileNameWithoutExtension + ".json";
            StorageUtilities.WriteTextFileToBlob(FetchEnvironmentVariables.AzureWebJobsStorage, editedTranscriptionResultJson, jsonContainer, jsonFileName, log);

            // Store HTML version of the transcript
            var htmlFileName = fileNameWithoutExtension + ".html";
            var displayResults = TranscriptionToHtml.ToHTML(transcriptionResult, fileName);
            StorageUtilities.WriteTextFileToBlob(FetchEnvironmentVariables.AzureWebJobsStorage, displayResults, htmlContainer, htmlFileName, log);

            if (textAnalyticsErrors.Any())
            {
                var distinctErrors = textAnalyticsErrors.Distinct();

                var errorMessage = "Text Analytics failed with error(s):\n";
                errorMessage += string.Join('\n', distinctErrors);

                var errorTxtname = fileNameWithoutExtension + "_error.txt";

                var errorOutputContainer = FetchEnvironmentVariables.ErrorReportOutputContainer;
                StorageUtilities.WriteTextFileToBlob(FetchEnvironmentVariables.AzureWebJobsStorage, errorMessage, errorOutputContainer, errorTxtname, log);
            }

            var useSqlDatabaseEnvVar = FetchEnvironmentVariables.UseSqlDatabase;

            if (bool.TryParse(useSqlDatabaseEnvVar, out var useSqlDatabaseEnv) && useSqlDatabaseEnv)
            {
                var dbConnectionString = FetchEnvironmentVariables.DatabaseConnectionString;
                using var dbConnector = new DatabaseConnector(log, dbConnectionString);
                await dbConnector.StoreTranscriptionAsync(
                    transcriptionId,
                    serviceBusMessage.Locale,
                    fileName,
                    serviceBusMessage.Channels,
                    (float)serviceBusMessage.EstimatedCost,
                    transcriptionResult).ConfigureAwait(false);
            }

            // Delete trace from service
            client.DeleteTranscriptionWithRetryAsync(transcriptionId).ConfigureAwait(false).GetAwaiter().GetResult();
        }
    }
}
