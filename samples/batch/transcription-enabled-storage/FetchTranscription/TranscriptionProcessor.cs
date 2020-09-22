// <copyright file="TranscriptionProcessor.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Collections.Generic;
    using System.Data.SqlClient;
    using System.Linq;
    using System.Net;
    using System.Text;
    using System.Threading.Tasks;
    using System.Xml;
    using Connector;
    using Connector.Enums;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public static class TranscriptionProcessor
    {
        public static async Task<bool> GetTranscripts(PostTranscriptionServiceBusMessage serviceBusMessage, ILogger log)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            var jobName = serviceBusMessage.JobName;
            var transcriptionId = serviceBusMessage.TranscriptionLocation.Split('/').LastOrDefault();
            var transcriptionGuid = new Guid(transcriptionId);
            log.LogInformation($"Received transcription {transcriptionGuid} with name {jobName} from service bus message.");

            var serviceBusConnectionString = FetchTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString;
            var reenqueueingTimeInSeconds = serviceBusMessage.ReenqueueingTimeInSeconds;
            log.LogInformation($"Re-enqueueing time for messages: {reenqueueingTimeInSeconds} seconds.");

            log.LogInformation($"Subscription location: {serviceBusMessage.Subscription.LocationUri.AbsoluteUri}");

            var client = new BatchClient(serviceBusMessage.Subscription.SubscriptionKey, serviceBusMessage.Subscription.LocationUri.AbsoluteUri, log);

            try
            {
                var transcription = await client.GetTranscriptionAsync(transcriptionGuid).ConfigureAwait(false);
                switch (transcription.Status)
                {
                    case "Failed":
                        await ProcessFailedTranscriptionAsync(client, transcription, transcriptionGuid, jobName, log).ConfigureAwait(false);
                        break;
                    case "Succeeded":
                        await ProcessSucceededTranscriptionAsync(client, serviceBusMessage, transcriptionGuid, jobName, log).ConfigureAwait(false);
                        break;
                    case "Running":
                        var runningMessage = serviceBusMessage.RetryMessage();
                        log.LogInformation("Transcription running, retrying message - retry count: " + runningMessage.RetryCount);
                        ServiceBusUtilities.SendServiceBusMessageAsync(serviceBusConnectionString, runningMessage.CreateMessageString(), log, reenqueueingTimeInSeconds).GetAwaiter().GetResult();
                        break;
                    case "NotStarted":
                        var notStartedMessage = serviceBusMessage.RetryMessage();
                        var initialDelayInSeconds = serviceBusMessage.InitialDelayInSeconds;

                        // If the transcription is not started yet, the job will take at least length of the audio:
                        var notStartedReenqueueingTime = Math.Max(initialDelayInSeconds, reenqueueingTimeInSeconds);

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

        internal static async Task ProcessFailedTranscriptionAsync(BatchClient client, Transcription transcription, Guid transcriptionId, string jobName, ILogger log)
        {
            log.LogInformation($"Got failed transcription for job {jobName}");

            var report = "Unknown Error.";
            var errorTxtname = jobName + ".txt";

            var transcriptionFiles = await client.GetTranscriptionFilesAsync(transcriptionId).ConfigureAwait(false);

            var reportFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.TranscriptionReport).FirstOrDefault();
            var reportFileContent = string.Empty;
            if (reportFile?.Links?.ContentUrl != null)
            {
                using var webClient = new WebClient();
                reportFileContent = webClient.DownloadString(reportFile.Links.ContentUrl);
                report = reportFileContent;
            }
            else if (transcription.Properties?.Error?.Message != null)
            {
                report = transcription.Properties.Error.Message;
                report += string.IsNullOrEmpty(transcription.Properties.Error.Code) ? string.Empty : $" (Error: {transcription.Properties.Error.Code}).";
            }

            var errorOutputContainer = FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer;
            await StorageUtilities.WriteTextFileToBlobAsync(FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage, report, errorOutputContainer, errorTxtname, log).ConfigureAwait(false);

            if (!string.IsNullOrEmpty(reportFileContent))
            {
                await ProcessReportFileAsync(reportFileContent, log).ConfigureAwait(false);
            }

            await client.DeleteTranscriptionAsync(transcriptionId).ConfigureAwait(false);
        }

        internal static async Task ProcessSucceededTranscriptionAsync(BatchClient client, PostTranscriptionServiceBusMessage serviceBusMessage, Guid transcriptionId, string jobName, ILogger log)
        {
            log.LogInformation($"Got succeeded transcription for job {jobName}");

            var jsonContainer = FetchTranscriptionEnvironmentVariables.JsonResultOutputContainer;

            var transcriptionFiles = await client.GetTranscriptionFilesAsync(transcriptionId).ConfigureAwait(false);
            var resultFiles = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.Transcription);
            var containsMultipleTranscriptions = resultFiles.Skip(1).Any();

            var textAnalyticsKey = FetchTranscriptionEnvironmentVariables.TextAnalyticsKey;
            var textAnalyticsRegion = FetchTranscriptionEnvironmentVariables.TextAnalyticsRegion;

            var textAnalyticsInfoProvided = !string.IsNullOrEmpty(textAnalyticsKey)
                && !string.IsNullOrEmpty(textAnalyticsRegion)
                && !textAnalyticsRegion.Equals("none", StringComparison.OrdinalIgnoreCase);

            var textAnalytics = textAnalyticsInfoProvided ? new TextAnalytics(serviceBusMessage.Locale, textAnalyticsKey, textAnalyticsRegion, log) : null;

            var generalErrorsStringBuilder = new StringBuilder();

            foreach (var resultFile in resultFiles)
            {
                var fileName = string.Empty;

                try
                {
                    var transcriptionResultJson = string.Empty;
                    using (var webClient = new WebClient())
                    {
                        transcriptionResultJson = webClient.DownloadString(resultFile.Links.ContentUrl);
                    }

                    var transcriptionResult = JsonConvert.DeserializeObject<SpeechTranscript>(transcriptionResultJson);
                    fileName = StorageUtilities.GetFileNameFromUri(new Uri(transcriptionResult.Source));
                    log.LogInformation($"Filename is {fileName}");

                    if (transcriptionResult.RecognizedPhrases == null || transcriptionResult.RecognizedPhrases.All(phrase => !phrase.RecognitionStatus.Equals("Success", StringComparison.Ordinal)))
                    {
                        continue;
                    }

                    var textAnalyticsErrors = new List<string>();

                    if (serviceBusMessage.AddSentimentAnalysis)
                    {
                        var sentimentErrors = await textAnalytics.AddSentimentToTranscriptAsync(transcriptionResult).ConfigureAwait(false);
                        textAnalyticsErrors.AddRange(sentimentErrors);
                    }

                    if (serviceBusMessage.AddEntityRedaction)
                    {
                        var entityRedactionErrors = await textAnalytics.RedactEntitiesAsync(transcriptionResult).ConfigureAwait(false);
                        textAnalyticsErrors.AddRange(entityRedactionErrors);
                    }

                    var editedTranscriptionResultJson = JsonConvert.SerializeObject(transcriptionResult, Newtonsoft.Json.Formatting.Indented);

                    // Store transcript json:
                    var jsonFileName = $"{fileName}.json";
                    await StorageUtilities.WriteTextFileToBlobAsync(FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage, editedTranscriptionResultJson, jsonContainer, jsonFileName, log).ConfigureAwait(false);

                    if (FetchTranscriptionEnvironmentVariables.CreateHtmlResultFile)
                    {
                        var htmlContainer = FetchTranscriptionEnvironmentVariables.HtmlResultOutputContainer;
                        var htmlFileName = $"{fileName}.html";
                        var displayResults = TranscriptionToHtml.ToHtml(transcriptionResult, jobName);
                        await StorageUtilities.WriteTextFileToBlobAsync(FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage, displayResults, htmlContainer, htmlFileName, log).ConfigureAwait(false);
                    }

                    if (textAnalyticsErrors.Any())
                    {
                        var distinctErrors = textAnalyticsErrors.Distinct();
                        var errorMessage = $"File {(string.IsNullOrEmpty(fileName) ? "unknown" : fileName)}:\n{string.Join('\n', distinctErrors)}";

                        generalErrorsStringBuilder.AppendLine(errorMessage);
                    }

                    if (FetchTranscriptionEnvironmentVariables.UseSqlDatabase)
                    {
                        var duration = XmlConvert.ToTimeSpan(transcriptionResult.Duration);
                        var approximatedCost = CostEstimation.GetCostEstimation(
                            duration,
                            transcriptionResult.CombinedRecognizedPhrases.Count(),
                            serviceBusMessage.UsesCustomModel,
                            serviceBusMessage.AddSentimentAnalysis,
                            serviceBusMessage.AddEntityRedaction);

                        var dbConnectionString = FetchTranscriptionEnvironmentVariables.DatabaseConnectionString;
                        using var dbConnector = new DatabaseConnector(log, dbConnectionString);
                        await dbConnector.StoreTranscriptionAsync(
                            containsMultipleTranscriptions ? Guid.NewGuid() : transcriptionId,
                            serviceBusMessage.Locale,
                            string.IsNullOrEmpty(fileName) ? jobName : fileName,
                            (float)approximatedCost,
                            transcriptionResult).ConfigureAwait(false);
                    }
                }
                catch (Exception e)
                {
                    if (string.IsNullOrEmpty(fileName) && e is ArgumentNullException)
                    {
                        var errorMessage = $"Transcription file name is unknown, failed with message: {e.Message}";
                        log.LogError(errorMessage);

                        generalErrorsStringBuilder.AppendLine(errorMessage);

                        continue;
                    }
                    else if (e is JsonException || e is SqlException)
                    {
                        var errorTxtname = fileName + ".txt";
                        var errorMessage = $"Transcription result processing failed with exception: {e.Message}";
                        log.LogError(errorMessage);

                        await StorageUtilities.WriteTextFileToBlobAsync(
                            FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage,
                            errorMessage,
                            FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer,
                            errorTxtname,
                            log).ConfigureAwait(false);

                        continue;
                    }

                    throw;
                }
            }

            var errors = generalErrorsStringBuilder.ToString();

            if (!string.IsNullOrEmpty(errors))
            {
                var errorTxtname = jobName + ".txt";

                await StorageUtilities.WriteTextFileToBlobAsync(
                    FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage,
                    errors,
                    FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer,
                    errorTxtname,
                    log).ConfigureAwait(false);
            }

            // Delete trace from service
            client.DeleteTranscriptionAsync(transcriptionId).ConfigureAwait(false).GetAwaiter().GetResult();
        }

        internal static async Task ProcessReportFileAsync(string reportFileContent, ILogger log)
        {
            var transcriptionReportFile = JsonConvert.DeserializeObject<TranscriptionReportFile>(reportFileContent);

            var failedTranscriptions = transcriptionReportFile.Details.
                Where(detail => !string.IsNullOrEmpty(detail.Status) &&
                    detail.Status.Equals("Failed", StringComparison.OrdinalIgnoreCase) &&
                     !string.IsNullOrEmpty(detail.SourceUrl));

            foreach (var failedTranscription in failedTranscriptions)
            {
                var fileName = StorageUtilities.GetFileNameFromUri(new Uri(failedTranscription.SourceUrl));

                var message = $"Transcription \"{fileName}\" failed with error \"{failedTranscription.ErrorKind}\" and message \"{failedTranscription.ErrorMessage}\"";
                log.LogError(message);

                var errorTxtname = fileName + ".txt";
                await StorageUtilities.WriteTextFileToBlobAsync(
                    FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage,
                    message,
                    FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer,
                    errorTxtname,
                    log).ConfigureAwait(false);
            }
        }
    }
}
