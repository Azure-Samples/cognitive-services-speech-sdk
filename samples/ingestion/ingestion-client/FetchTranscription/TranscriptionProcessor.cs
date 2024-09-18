// <copyright file="TranscriptionProcessor.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Net.Http;
    using System.Text;
    using System.Threading.Tasks;
    using System.Xml;
    using Azure;
    using Azure.Messaging.ServiceBus;
    using Connector;
    using Connector.Database;
    using Connector.Enums;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    using Microsoft.EntityFrameworkCore;
    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.Logging;
    using Microsoft.Extensions.Options;

    using Newtonsoft.Json;

    public class TranscriptionProcessor
    {
        private readonly ServiceBusSender startTranscriptionServiceBusSender;

        private readonly ServiceBusSender fetchTranscriptionServiceBusSender;

        private readonly ServiceBusSender completedTranscriptionServiceBusSender;

        private readonly IngestionClientDbContext databaseContext;

        private readonly IStorageConnector storageConnector;

        private readonly BatchClient batchClient;

        private readonly AppConfig appConfig;

        public TranscriptionProcessor(
            IStorageConnector storageConnector,
            IAzureClientFactory<ServiceBusClient> serviceBusClientFactory,
            IngestionClientDbContext databaseContext,
            BatchClient batchClient,
            IOptions<AppConfig> appConfig)
        {
            this.storageConnector = storageConnector;
            this.databaseContext = databaseContext;
            this.batchClient = batchClient;
            this.appConfig = appConfig?.Value;

            ArgumentNullException.ThrowIfNull(serviceBusClientFactory, nameof(serviceBusClientFactory));
            var startTranscriptionServiceBusClient = serviceBusClientFactory.CreateClient(ServiceBusClientName.StartTranscriptionServiceBusClient.ToString());
            var startTranscriptionQueueName = ServiceBusConnectionStringProperties.Parse(this.appConfig.StartTranscriptionServiceBusConnectionString).EntityPath;
            this.startTranscriptionServiceBusSender = startTranscriptionServiceBusClient.CreateSender(startTranscriptionQueueName);

            var fetchTranscriptionServiceBusClient = serviceBusClientFactory.CreateClient(ServiceBusClientName.FetchTranscriptionServiceBusClient.ToString());
            var fetchTranscriptionQueueName = ServiceBusConnectionStringProperties.Parse(this.appConfig.FetchTranscriptionServiceBusConnectionString).EntityPath;
            this.fetchTranscriptionServiceBusSender = fetchTranscriptionServiceBusClient.CreateSender(fetchTranscriptionQueueName);

            if (!string.IsNullOrWhiteSpace(this.appConfig.CompletedServiceBusConnectionString))
            {
                var completedTranscriptionServiceBusClient = serviceBusClientFactory.CreateClient(ServiceBusClientName.CompletedTranscriptionServiceBusClient.ToString());
                var completedTranscriptionQueueName = ServiceBusConnectionStringProperties.Parse(this.appConfig.CompletedServiceBusConnectionString).EntityPath;
                this.completedTranscriptionServiceBusSender = completedTranscriptionServiceBusClient.CreateSender(completedTranscriptionQueueName);
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Catch general exception to allow manual retrying.")]
        public async Task ProcessTranscriptionJobAsync(TranscriptionStartedMessage serviceBusMessage, IServiceProvider serviceProvider, ILogger log)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            var subscriptionKey = this.appConfig.AzureSpeechServicesKey;
            var jobName = serviceBusMessage.JobName;
            var transcriptionLocation = serviceBusMessage.TranscriptionLocation;
            log.LogInformation($"Received transcription at {transcriptionLocation} with name {jobName} from service bus message.");

            var messageDelayTime = GetMessageDelayTime(serviceBusMessage.PollingCounter, this.appConfig);
            serviceBusMessage.PollingCounter += 1;

            try
            {
                var transcription = await this.batchClient.GetTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
                log.LogInformation($"Polled {serviceBusMessage.PollingCounter} time(s) for results in total, delay job for {messageDelayTime.TotalMinutes} minutes if not completed.");
                switch (transcription.Status)
                {
                    case "Failed":
                        await this.ProcessFailedTranscriptionAsync(transcriptionLocation, subscriptionKey, serviceBusMessage, transcription, jobName, log).ConfigureAwait(false);
                        break;
                    case "Succeeded":
                        await this.ProcessSucceededTranscriptionAsync(transcriptionLocation, subscriptionKey, serviceBusMessage, jobName, log).ConfigureAwait(false);
                        break;
                    case "Running":
                        log.LogInformation($"Transcription running, polling again after {messageDelayTime.TotalMinutes} minutes.");
                        await ServiceBusUtilities.SendServiceBusMessageAsync(this.fetchTranscriptionServiceBusSender, serviceBusMessage.CreateMessageString(), log, messageDelayTime).ConfigureAwait(false);
                        break;
                    case "NotStarted":
                        log.LogInformation($"Transcription not started, polling again after {messageDelayTime.TotalMinutes} minutes.");
                        await ServiceBusUtilities.SendServiceBusMessageAsync(this.fetchTranscriptionServiceBusSender, serviceBusMessage.CreateMessageString(), log, messageDelayTime).ConfigureAwait(false);
                        break;
                }
            }
            catch (TransientFailureException e)
            {
                await this.RetryOrFailJobAsync(
                    serviceBusMessage,
                    $"Exception {e} in job {jobName} at {transcriptionLocation}: {e.Message}",
                    jobName,
                    transcriptionLocation,
                    subscriptionKey,
                    log,
                    isThrottled: false).ConfigureAwait(false);
            }
            catch (TimeoutException e)
            {
                await this.RetryOrFailJobAsync(
                    serviceBusMessage,
                    $"TimeoutException {e} in job {jobName} at {transcriptionLocation}: {e.Message}",
                    jobName,
                    transcriptionLocation,
                    subscriptionKey,
                    log,
                    isThrottled: false).ConfigureAwait(false);
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
                else if (e is RequestFailedException requestFailedException && requestFailedException.Status != 0)
                {
                    httpStatusCode = (HttpStatusCode)requestFailedException.Status;
                }

                if (httpStatusCode.HasValue && httpStatusCode.Value.IsRetryableStatus())
                {
                    await this.RetryOrFailJobAsync(serviceBusMessage, $"Exception {e} in job {jobName} at {transcriptionLocation}: {e.Message}", jobName, transcriptionLocation, subscriptionKey, log, isThrottled: httpStatusCode.Value == HttpStatusCode.TooManyRequests).ConfigureAwait(false);
                }
                else
                {
                    await this.WriteFailedJobLogToStorageAsync(serviceBusMessage, $"Exception {e} in job {jobName} at {transcriptionLocation}: {e.Message}", jobName, log).ConfigureAwait(false);
                }
            }
        }

        private static TimeSpan GetMessageDelayTime(int pollingCounter, AppConfig appConfig)
        {
            if (pollingCounter == 0)
            {
                return TimeSpan.FromMinutes(appConfig.InitialPollingDelayInMinutes);
            }

            var updatedDelay = Math.Pow(2, Math.Min(pollingCounter, 8)) * appConfig.InitialPollingDelayInMinutes;

            if ((int)updatedDelay > appConfig.MaxPollingDelayInMinutes)
            {
                return TimeSpan.FromMinutes(appConfig.MaxPollingDelayInMinutes);
            }

            return TimeSpan.FromMinutes(updatedDelay);
        }

        private static bool IsRetryableError(string errorCode)
        {
            return errorCode switch
            {
                "InvalidUri" or "Internal" or "Timeout" or "Transient" => true,
                _ => false,
            };
        }

        private async Task ProcessFailedTranscriptionAsync(string transcriptionLocation, string subscriptionKey, TranscriptionStartedMessage transcriptionStartedMessage, Transcription transcription, string jobName, ILogger log)
        {
            var safeErrorCode = transcription?.Properties?.Error?.Code ?? "unknown";
            var safeErrorMessage = transcription?.Properties?.Error?.Message ?? "unknown";
            var logMessage = $"Got failed transcription for job {jobName} with error {safeErrorMessage} (Error code: {safeErrorCode}).";

            log.LogInformation(logMessage);

            var transcriptionFiles = await this.batchClient.GetTranscriptionFilesAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);

            var errorReportOutput = logMessage;
            var reportFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.TranscriptionReport).FirstOrDefault();
            if (reportFile?.Links?.ContentUrl != null)
            {
                var reportFileContent = await this.batchClient.GetTranscriptionReportFileFromSasAsync(reportFile.Links.ContentUrl).ConfigureAwait(false);
                errorReportOutput += $"\nReport file: \n {JsonConvert.SerializeObject(reportFileContent)}";
            }

            var errorOutputContainer = this.appConfig.ErrorReportOutputContainer;
            await this.storageConnector.WriteTextFileToBlobAsync(errorReportOutput, errorOutputContainer, $"jobs/{jobName}.txt").ConfigureAwait(false);

            var retryAudioFile = IsRetryableError(safeErrorCode);

            foreach (var audio in transcriptionStartedMessage.AudioFileInfos)
            {
                var fileName = this.storageConnector.GetFileNameFromUri(new Uri(audio.FileUrl));

                if (retryAudioFile && audio.RetryCount < this.appConfig.RetryLimit)
                {
                    log.LogInformation($"Retrying transcription with name {fileName} - retry count: {audio.RetryCount}");
                    var serviceBusMessage = new Connector.ServiceBusMessage
                    {
                        Data = new Data
                        {
                            Url = new Uri(audio.FileUrl)
                        },
                        EventType = "BlobCreated",
                        RetryCount = audio.RetryCount + 1
                    };

                    var audioFileMessage = new Azure.Messaging.ServiceBus.ServiceBusMessage(JsonConvert.SerializeObject(serviceBusMessage));
                    await ServiceBusUtilities.SendServiceBusMessageAsync(this.startTranscriptionServiceBusSender, audioFileMessage, log, TimeSpan.FromMinutes(1)).ConfigureAwait(false);
                }
                else
                {
                    var message = $"Failed transcription with name {fileName} in job {jobName} after {audio.RetryCount} retries with error: {safeErrorMessage} (Error: {safeErrorCode}).";
                    await this.storageConnector.WriteTextFileToBlobAsync(message, errorOutputContainer, $"{fileName}.txt").ConfigureAwait(false);
                    await this.storageConnector.MoveFileAsync(
                        this.appConfig.AudioInputContainer,
                        fileName,
                        this.appConfig.ErrorFilesOutputContainer,
                        fileName,
                        false).ConfigureAwait(false);
                }
            }

            await this.batchClient.DeleteTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
        }

        private async Task ProcessReportFileAsync(TranscriptionReportFile transcriptionReportFile, ILogger log)
        {
            var failedTranscriptions = transcriptionReportFile.Details.
                Where(detail => !string.IsNullOrEmpty(detail.Status) &&
                    detail.Status.Equals("Failed", StringComparison.OrdinalIgnoreCase) &&
                    !string.IsNullOrEmpty(detail.Source));

            foreach (var failedTranscription in failedTranscriptions)
            {
                if (string.IsNullOrEmpty(failedTranscription.Source))
                {
                    continue;
                }

                var safeErrorCode = failedTranscription.ErrorKind ?? "unknown";
                var safeErrorMessage = failedTranscription.ErrorMessage ?? "unknown";

                var fileName = this.storageConnector.GetFileNameFromUri(new Uri(failedTranscription.Source));

                var message = $"Transcription \"{fileName}\" failed with error \"{safeErrorCode}\" and message \"{safeErrorMessage}\"";
                log.LogError(message);

                var errorTxtname = fileName + ".txt";
                await this.storageConnector.WriteTextFileToBlobAsync(
                    message,
                    this.appConfig.ErrorReportOutputContainer,
                    errorTxtname).ConfigureAwait(false);
                await this.storageConnector.MoveFileAsync(
                    this.appConfig.AudioInputContainer,
                    fileName,
                    this.appConfig.ErrorFilesOutputContainer,
                    fileName,
                    false).ConfigureAwait(false);
            }
        }

        private async Task RetryOrFailJobAsync(TranscriptionStartedMessage message, string errorMessage, string jobName, string transcriptionLocation, string subscriptionKey, ILogger log, bool isThrottled)
        {
            log.LogError(errorMessage);
            message.FailedExecutionCounter += 1;
            var messageDelayTime = GetMessageDelayTime(message.PollingCounter, this.appConfig);

            if (message.FailedExecutionCounter <= this.appConfig.RetryLimit || isThrottled)
            {
                log.LogInformation("Retrying..");
                await ServiceBusUtilities.SendServiceBusMessageAsync(this.fetchTranscriptionServiceBusSender, message.CreateMessageString(), log, messageDelayTime).ConfigureAwait(false);
            }
            else
            {
                await this.WriteFailedJobLogToStorageAsync(message, errorMessage, jobName, log).ConfigureAwait(false);
                await this.batchClient.DeleteTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
            }
        }

        private async Task WriteFailedJobLogToStorageAsync(TranscriptionStartedMessage transcriptionStartedMessage, string errorMessage, string jobName, ILogger log)
        {
            log.LogError(errorMessage);
            var errorOutputContainer = this.appConfig.ErrorReportOutputContainer;

            var jobErrorFileName = $"jobs/{jobName}.txt";
            await this.storageConnector.WriteTextFileToBlobAsync(errorMessage, errorOutputContainer, jobErrorFileName).ConfigureAwait(false);

            foreach (var audioFileInfo in transcriptionStartedMessage.AudioFileInfos)
            {
                var fileName = this.storageConnector.GetFileNameFromUri(new Uri(audioFileInfo.FileUrl));
                var errorFileName = fileName + ".txt";
                try
                {
                    await this.storageConnector.WriteTextFileToBlobAsync(errorMessage, errorOutputContainer, errorFileName).ConfigureAwait(false);
                    await this.storageConnector.MoveFileAsync(
                        this.appConfig.AudioInputContainer,
                        fileName,
                        this.appConfig.ErrorFilesOutputContainer,
                        fileName,
                        false).ConfigureAwait(false);
                }
                catch (RequestFailedException e)
                {
                    log.LogError($"Storage Exception {e} while writing error log to file and moving result");
                }
            }
        }

        private async Task WriteErrorReportAsync(string errorString, string jobName)
        {
            var errorTxtname = $"jobs/{jobName}.txt";

            await this.storageConnector.WriteTextFileToBlobAsync(
                errorString,
                this.appConfig.ErrorReportOutputContainer,
                errorTxtname).ConfigureAwait(false);
        }

        private async Task ProcessSucceededTranscriptionAsync(string transcriptionLocation, string subscriptionKey, TranscriptionStartedMessage serviceBusMessage, string jobName, ILogger log)
        {
            log.LogInformation($"Got succeeded transcription for job {jobName}");

            var transcriptionAnalyticsOrchestrator = new TranscriptionAnalyticsOrchestrator(serviceBusMessage.Locale, log, Options.Create(this.appConfig));
            var transcriptionAnalyticsJobStatus = await transcriptionAnalyticsOrchestrator.GetTranscriptionAnalyticsJobsStatusAsync(serviceBusMessage).ConfigureAwait(false);
            if (transcriptionAnalyticsJobStatus == TranscriptionAnalyticsJobStatus.Running)
            {
                // If transcription analytics request is still running, re-queue message and get status again after X minutes:
                log.LogInformation($"Transcription analytics requests still running for job {jobName} - re-queueing message.");
                await ServiceBusUtilities.SendServiceBusMessageAsync(this.fetchTranscriptionServiceBusSender, serviceBusMessage.CreateMessageString(), log, GetMessageDelayTime(serviceBusMessage.PollingCounter, this.appConfig)).ConfigureAwait(false);
                return;
            }

            var transcriptionFiles = await this.batchClient.GetTranscriptionFilesAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
            log.LogInformation($"Received transcription files.");
            var resultFiles = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.Transcription);

            var generalErrorsStringBuilder = new StringBuilder();
            var speechTranscriptMappings = new Dictionary<AudioFileInfo, SpeechTranscript>();

            foreach (var resultFile in resultFiles)
            {
                log.LogInformation($"Getting result for file {resultFile.Name}");

                try
                {
                    var transcriptionResult = await this.batchClient.GetSpeechTranscriptFromSasAsync(resultFile.Links.ContentUrl).ConfigureAwait(false);

                    if (string.IsNullOrEmpty(transcriptionResult.Source))
                    {
                        var errorMessage = "Transcription source is unknown, skipping evaluation.";
                        log.LogError(errorMessage);

                        generalErrorsStringBuilder.AppendLine(errorMessage);
                        continue;
                    }

                    var audioFileName = this.storageConnector.GetFileNameFromUri(new Uri(transcriptionResult.Source));
                    var audioFileInfo = serviceBusMessage.AudioFileInfos.Where(a => a.FileName == audioFileName).First();

                    if (speechTranscriptMappings.ContainsKey(audioFileInfo))
                    {
                        var errorMessage = $"Duplicate audio file in job, skipping: {audioFileInfo.FileName}.";
                        log.LogError(errorMessage);
                        generalErrorsStringBuilder.AppendLine(errorMessage);
                        continue;
                    }

                    speechTranscriptMappings.Add(audioFileInfo, transcriptionResult);
                }
                catch (Exception e) when (e is HttpStatusCodeException || e is HttpRequestException)
                {
                    var errorMessage = $"Failed getting speech transcript from content url: {e.Message}";
                    log.LogError(errorMessage);

                    generalErrorsStringBuilder.AppendLine(errorMessage);
                }
            }

            if (transcriptionAnalyticsJobStatus == TranscriptionAnalyticsJobStatus.Completed)
            {
                var errors = await transcriptionAnalyticsOrchestrator.AddTranscriptionAnalyticsResultsToTranscripts(speechTranscriptMappings).ConfigureAwait(false);

                foreach (var error in errors)
                {
                    generalErrorsStringBuilder.AppendLine(error);
                }
            }
            else if (transcriptionAnalyticsJobStatus == TranscriptionAnalyticsJobStatus.NotSubmitted)
            {
                var errors = await transcriptionAnalyticsOrchestrator.SubmitTranscriptionAnalyticsJobsAndAddToAudioFileInfos(speechTranscriptMappings).ConfigureAwait(false);
                foreach (var error in errors)
                {
                    generalErrorsStringBuilder.AppendLine(error);
                }

                var textAnalyticsSubmitErrors = generalErrorsStringBuilder.ToString();
                if (!string.IsNullOrEmpty(textAnalyticsSubmitErrors))
                {
                    await this.WriteErrorReportAsync(textAnalyticsSubmitErrors, jobName).ConfigureAwait(false);
                }

                log.LogInformation($"Added text analytics requests to service bus message - re-queueing message.");

                // Poll for first time with TA request after 1 minute
                await ServiceBusUtilities.SendServiceBusMessageAsync(this.fetchTranscriptionServiceBusSender, serviceBusMessage.CreateMessageString(), log, TimeSpan.FromMinutes(1)).ConfigureAwait(false);
                return;
            }

            var completedMessages = new List<CompletedMessage>();
            foreach (var speechTranscriptMapping in speechTranscriptMappings)
            {
                var speechTranscript = speechTranscriptMapping.Value;

                var audioFileInfo = speechTranscriptMapping.Key;
                var fileName = audioFileInfo.FileName;

                var editedTranscriptionResultJson = JsonConvert.SerializeObject(
                    speechTranscript,
                    Newtonsoft.Json.Formatting.Indented,
                    new JsonSerializerSettings
                    {
                        NullValueHandling = NullValueHandling.Ignore
                    });

                var jsonFileName = $"{fileName}.json";
                var archiveFileLocation = System.IO.Path.GetFileNameWithoutExtension(fileName);

                var jsonFileUrl = await this.storageConnector.WriteTextFileToBlobAsync(editedTranscriptionResultJson, this.appConfig.JsonResultOutputContainer, jsonFileName).ConfigureAwait(false);

                if (!string.IsNullOrEmpty(this.appConfig.CompletedServiceBusConnectionString))
                {
                    var completedMessage = new CompletedMessage(audioFileInfo.FileUrl, jsonFileUrl);
                    completedMessages.Add(completedMessage);
                }

                var consolidatedContainer = this.appConfig.ConsolidatedFilesOutputContainer;
                if (this.appConfig.CreateConsolidatedOutputFiles)
                {
                    var audioArchiveFileName = $"{archiveFileLocation}/{fileName}";
                    var jsonArchiveFileName = $"{archiveFileLocation}/{jsonFileName}";

                    await this.storageConnector.MoveFileAsync(this.appConfig.AudioInputContainer, fileName, consolidatedContainer, audioArchiveFileName, true).ConfigureAwait(false);
                    await this.storageConnector.WriteTextFileToBlobAsync(editedTranscriptionResultJson, consolidatedContainer, jsonArchiveFileName).ConfigureAwait(false);
                }

                if (this.appConfig.CreateHtmlResultFile)
                {
                    var htmlContainer = this.appConfig.HtmlResultOutputContainer;
                    var htmlFileName = $"{fileName}.html";
                    var displayResults = TranscriptionToHtml.ToHtml(speechTranscript, jobName);
                    await this.storageConnector.WriteTextFileToBlobAsync(displayResults, htmlContainer, htmlFileName).ConfigureAwait(false);

                    if (this.appConfig.CreateConsolidatedOutputFiles)
                    {
                        var htmlArchiveFileName = $"{archiveFileLocation}/{htmlFileName}";
                        await this.storageConnector.WriteTextFileToBlobAsync(displayResults, consolidatedContainer, htmlArchiveFileName).ConfigureAwait(false);
                    }
                }

                if (this.appConfig.UseSqlDatabase)
                {
                    var duration = string.IsNullOrEmpty(speechTranscript.Duration) ? TimeSpan.Zero : XmlConvert.ToTimeSpan(speechTranscript.Duration);
                    var approximatedCost = CostEstimation.GetCostEstimation(
                        duration,
                        speechTranscript.CombinedRecognizedPhrases.Count(),
                        serviceBusMessage.UsesCustomModel,
                        this.appConfig.SentimentAnalysisSetting,
                        this.appConfig.PiiRedactionSetting);

                    var containsMultipleTranscriptions = resultFiles.Skip(1).Any();
                    var jobId = containsMultipleTranscriptions ? Guid.NewGuid() : new Guid(transcriptionLocation.Split('/').LastOrDefault());

                    try
                    {
                        await this.databaseContext.StoreTranscriptionAsync(
                            jobId,
                            serviceBusMessage.Locale,
                            string.IsNullOrEmpty(fileName) ? jobName : fileName,
                            (float)approximatedCost,
                            speechTranscript).ConfigureAwait(false);
                    }
                    catch (Exception exception) when (exception is DbUpdateException || exception is DbUpdateConcurrencyException)
                    {
                        var errorMessage = $"Exception while processing database update: {exception}";
                        log.LogError(errorMessage);
                        generalErrorsStringBuilder.AppendLine(errorMessage);
                    }
                }

                if (this.appConfig.CreateAudioProcessedContainer)
                {
                    await this.storageConnector.MoveFileAsync(this.appConfig.AudioInputContainer, fileName, this.appConfig.AudioProcessedContainer, fileName, false).ConfigureAwait(false);
                }
            }

            if (this.completedTranscriptionServiceBusSender != null)
            {
                await ServiceBusUtilities.SendServiceBusMessageAsync(this.completedTranscriptionServiceBusSender, JsonConvert.SerializeObject(completedMessages), log, GetMessageDelayTime(serviceBusMessage.PollingCounter, this.appConfig)).ConfigureAwait(false);
            }

            var generalErrors = generalErrorsStringBuilder.ToString();
            if (!string.IsNullOrEmpty(generalErrors))
            {
                await this.WriteErrorReportAsync(generalErrors, jobName).ConfigureAwait(false);
            }

            var reportFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.TranscriptionReport).FirstOrDefault();
            var reportFileContent = await this.batchClient.GetTranscriptionReportFileFromSasAsync(reportFile.Links.ContentUrl).ConfigureAwait(false);
            await this.ProcessReportFileAsync(reportFileContent, log).ConfigureAwait(false);

            this.batchClient.DeleteTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false).GetAwaiter().GetResult();
        }
    }
}
