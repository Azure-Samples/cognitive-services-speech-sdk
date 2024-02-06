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
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Logging;
    using Newtonsoft.Json;

    public class TranscriptionProcessor
    {
        private static readonly StorageConnector StorageConnectorInstance = new StorageConnector(FetchTranscriptionEnvironmentVariables.AzureWebJobsStorage);

        private static readonly ServiceBusClient StartServiceBusClient = new ServiceBusClient(FetchTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString);

        private static readonly ServiceBusSender StartServiceBusSender = StartServiceBusClient.CreateSender(ServiceBusConnectionStringProperties.Parse(FetchTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString).EntityPath);

        private static readonly ServiceBusClient FetchServiceBusClient = new ServiceBusClient(FetchTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString);

        private static readonly ServiceBusSender FetchServiceBusSender = FetchServiceBusClient.CreateSender(ServiceBusConnectionStringProperties.Parse(FetchTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString).EntityPath);

        private readonly IServiceProvider serviceProvider;

        private readonly IngestionClientDbContext databaseContext;

        public TranscriptionProcessor(IServiceProvider serviceProvider)
        {
            this.serviceProvider = serviceProvider;

            if (FetchTranscriptionEnvironmentVariables.UseSqlDatabase)
            {
                this.databaseContext = this.serviceProvider.GetRequiredService<IngestionClientDbContext>();
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Design", "CA1031:Do not catch general exception types", Justification = "Catch general exception to allow manual retrying.")]
        public async Task ProcessTranscriptionJobAsync(TranscriptionStartedMessage serviceBusMessage, IServiceProvider serviceProvider, ILogger log)
        {
            if (serviceBusMessage == null)
            {
                throw new ArgumentNullException(nameof(serviceBusMessage));
            }

            var subscriptionKey = FetchTranscriptionEnvironmentVariables.AzureSpeechServicesKey;
            var jobName = serviceBusMessage.JobName;
            var transcriptionLocation = serviceBusMessage.TranscriptionLocation;
            log.LogInformation($"Received transcription at {transcriptionLocation} with name {jobName} from service bus message.");

            var messageDelayTime = GetMessageDelayTime(serviceBusMessage.PollingCounter);
            serviceBusMessage.PollingCounter += 1;

            try
            {
                var transcription = await BatchClient.GetTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
                log.LogInformation($"Polled {serviceBusMessage.PollingCounter} time(s) for results in total, delay job for {messageDelayTime.TotalMinutes} minutes if not completed.");
                switch (transcription.Status)
                {
                    case "Failed":
                        await ProcessFailedTranscriptionAsync(transcriptionLocation, subscriptionKey, serviceBusMessage, transcription, jobName, log).ConfigureAwait(false);
                        break;
                    case "Succeeded":
                        await this.ProcessSucceededTranscriptionAsync(transcriptionLocation, subscriptionKey, serviceBusMessage, jobName, log).ConfigureAwait(false);
                        break;
                    case "Running":
                        log.LogInformation($"Transcription running, polling again after {messageDelayTime.TotalMinutes} minutes.");
                        await ServiceBusUtilities.SendServiceBusMessageAsync(FetchServiceBusSender, serviceBusMessage.CreateMessageString(), log, messageDelayTime).ConfigureAwait(false);
                        break;
                    case "NotStarted":
                        log.LogInformation($"Transcription not started, polling again after {messageDelayTime.TotalMinutes} minutes.");
                        await ServiceBusUtilities.SendServiceBusMessageAsync(FetchServiceBusSender, serviceBusMessage.CreateMessageString(), log, messageDelayTime).ConfigureAwait(false);
                        break;
                }
            }
            catch (TransientFailureException e)
            {
                await RetryOrFailJobAsync(
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
                await RetryOrFailJobAsync(
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
                    await RetryOrFailJobAsync(serviceBusMessage, $"Exception {e} in job {jobName} at {transcriptionLocation}: {e.Message}", jobName, transcriptionLocation, subscriptionKey, log, isThrottled: httpStatusCode.Value == HttpStatusCode.TooManyRequests).ConfigureAwait(false);
                }
                else
                {
                    await WriteFailedJobLogToStorageAsync(serviceBusMessage, $"Exception {e} in job {jobName} at {transcriptionLocation}: {e.Message}", jobName, log).ConfigureAwait(false);
                }
            }
        }

        private static TimeSpan GetMessageDelayTime(int pollingCounter)
        {
            if (pollingCounter == 0)
            {
                return TimeSpan.FromMinutes(FetchTranscriptionEnvironmentVariables.InitialPollingDelayInMinutes);
            }

            var updatedDelay = Math.Pow(2, Math.Min(pollingCounter, 8)) * FetchTranscriptionEnvironmentVariables.InitialPollingDelayInMinutes;

            if ((int)updatedDelay > FetchTranscriptionEnvironmentVariables.MaxPollingDelayInMinutes)
            {
                return TimeSpan.FromMinutes(FetchTranscriptionEnvironmentVariables.MaxPollingDelayInMinutes);
            }

            return TimeSpan.FromMinutes(updatedDelay);
        }

        private static async Task ProcessFailedTranscriptionAsync(string transcriptionLocation, string subscriptionKey, TranscriptionStartedMessage transcriptionStartedMessage, Transcription transcription, string jobName, ILogger log)
        {
            var safeErrorCode = transcription?.Properties?.Error?.Code ?? "unknown";
            var safeErrorMessage = transcription?.Properties?.Error?.Message ?? "unknown";
            var logMessage = $"Got failed transcription for job {jobName} with error {safeErrorMessage} (Error code: {safeErrorCode}).";

            log.LogInformation(logMessage);

            var transcriptionFiles = await BatchClient.GetTranscriptionFilesAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);

            var errorReportOutput = logMessage;
            var reportFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.TranscriptionReport).FirstOrDefault();
            if (reportFile?.Links?.ContentUrl != null)
            {
                var reportFileContent = await BatchClient.GetTranscriptionReportFileFromSasAsync(reportFile.Links.ContentUrl).ConfigureAwait(false);
                errorReportOutput += $"\nReport file: \n {JsonConvert.SerializeObject(reportFileContent)}";
            }

            var errorOutputContainer = FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer;
            await StorageConnectorInstance.WriteTextFileToBlobAsync(errorReportOutput, errorOutputContainer, $"jobs/{jobName}.txt", log).ConfigureAwait(false);

            var retryAudioFile = IsRetryableError(safeErrorCode);

            foreach (var audio in transcriptionStartedMessage.AudioFileInfos)
            {
                var fileName = StorageConnector.GetFileNameFromUri(new Uri(audio.FileUrl));

                if (retryAudioFile && audio.RetryCount < FetchTranscriptionEnvironmentVariables.RetryLimit)
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
                    await ServiceBusUtilities.SendServiceBusMessageAsync(StartServiceBusSender, audioFileMessage, log, TimeSpan.FromMinutes(1)).ConfigureAwait(false);
                }
                else
                {
                    var message = $"Failed transcription with name {fileName} in job {jobName} after {audio.RetryCount} retries with error: {safeErrorMessage} (Error: {safeErrorCode}).";
                    await StorageConnectorInstance.WriteTextFileToBlobAsync(message, errorOutputContainer, $"{fileName}.txt", log).ConfigureAwait(false);
                    await StorageConnectorInstance.MoveFileAsync(
                        FetchTranscriptionEnvironmentVariables.AudioInputContainer,
                        fileName,
                        FetchTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                        fileName,
                        false,
                        log).ConfigureAwait(false);
                }
            }

            await BatchClient.DeleteTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
        }

        private static bool IsRetryableError(string errorCode)
        {
            return errorCode switch
            {
                "InvalidUri" or "Internal" or "Timeout" or "Transient" => true,
                _ => false,
            };
        }

        private static async Task ProcessReportFileAsync(TranscriptionReportFile transcriptionReportFile, ILogger log)
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

                var fileName = StorageConnector.GetFileNameFromUri(new Uri(failedTranscription.Source));

                var message = $"Transcription \"{fileName}\" failed with error \"{safeErrorCode}\" and message \"{safeErrorMessage}\"";
                log.LogError(message);

                var errorTxtname = fileName + ".txt";
                await StorageConnectorInstance.WriteTextFileToBlobAsync(
                    message,
                    FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer,
                    errorTxtname,
                    log).ConfigureAwait(false);
                await StorageConnectorInstance.MoveFileAsync(
                    FetchTranscriptionEnvironmentVariables.AudioInputContainer,
                    fileName,
                    FetchTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                    fileName,
                    false,
                    log).ConfigureAwait(false);
            }
        }

        private static async Task RetryOrFailJobAsync(TranscriptionStartedMessage message, string errorMessage, string jobName, string transcriptionLocation, string subscriptionKey, ILogger log, bool isThrottled)
        {
            log.LogError(errorMessage);
            message.FailedExecutionCounter += 1;
            var messageDelayTime = GetMessageDelayTime(message.PollingCounter);

            if (message.FailedExecutionCounter <= FetchTranscriptionEnvironmentVariables.RetryLimit || isThrottled)
            {
                log.LogInformation("Retrying..");
                await ServiceBusUtilities.SendServiceBusMessageAsync(FetchServiceBusSender, message.CreateMessageString(), log, messageDelayTime).ConfigureAwait(false);
            }
            else
            {
                await WriteFailedJobLogToStorageAsync(message, errorMessage, jobName, log).ConfigureAwait(false);
                await BatchClient.DeleteTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
            }
        }

        private static async Task WriteFailedJobLogToStorageAsync(TranscriptionStartedMessage transcriptionStartedMessage, string errorMessage, string jobName, ILogger log)
        {
            log.LogError(errorMessage);
            var errorOutputContainer = FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer;

            var jobErrorFileName = $"jobs/{jobName}.txt";
            await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, errorOutputContainer, jobErrorFileName, log).ConfigureAwait(false);

            foreach (var audioFileInfo in transcriptionStartedMessage.AudioFileInfos)
            {
                var fileName = StorageConnector.GetFileNameFromUri(new Uri(audioFileInfo.FileUrl));
                var errorFileName = fileName + ".txt";
                try
                {
                    await StorageConnectorInstance.WriteTextFileToBlobAsync(errorMessage, errorOutputContainer, errorFileName, log).ConfigureAwait(false);
                    await StorageConnectorInstance.MoveFileAsync(
                        FetchTranscriptionEnvironmentVariables.AudioInputContainer,
                        fileName,
                        FetchTranscriptionEnvironmentVariables.ErrorFilesOutputContainer,
                        fileName,
                        false,
                        log).ConfigureAwait(false);
                }
                catch (RequestFailedException e)
                {
                    log.LogError($"Storage Exception {e} while writing error log to file and moving result");
                }
            }
        }

        private static async Task WriteErrorReportAsync(string errorString, string jobName, ILogger logger)
        {
            var errorTxtname = $"jobs/{jobName}.txt";

            await StorageConnectorInstance.WriteTextFileToBlobAsync(
                errorString,
                FetchTranscriptionEnvironmentVariables.ErrorReportOutputContainer,
                errorTxtname,
                logger).ConfigureAwait(false);
        }

        private async Task ProcessSucceededTranscriptionAsync(string transcriptionLocation, string subscriptionKey, TranscriptionStartedMessage serviceBusMessage, string jobName, ILogger log)
        {
            log.LogInformation($"Got succeeded transcription for job {jobName}");

            var transcriptionAnalyticsOrchestrator = new TranscriptionAnalyticsOrchestrator(serviceBusMessage.Locale, log);
            var transcriptionAnalyticsJobStatus = await transcriptionAnalyticsOrchestrator.GetTranscriptionAnalyticsJobsStatusAsync(serviceBusMessage).ConfigureAwait(false);
            if (transcriptionAnalyticsJobStatus == TranscriptionAnalyticsJobStatus.Running)
            {
                // If transcription analytics request is still running, re-queue message and get status again after X minutes:
                log.LogInformation($"Transcription analytics requests still running for job {jobName} - re-queueing message.");
                await ServiceBusUtilities.SendServiceBusMessageAsync(FetchServiceBusSender, serviceBusMessage.CreateMessageString(), log, GetMessageDelayTime(serviceBusMessage.PollingCounter)).ConfigureAwait(false);
                return;
            }

            var transcriptionFiles = await BatchClient.GetTranscriptionFilesAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false);
            log.LogInformation($"Received transcription files.");
            var resultFiles = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.Transcription);

            var generalErrorsStringBuilder = new StringBuilder();
            var speechTranscriptMappings = new Dictionary<AudioFileInfo, SpeechTranscript>();

            foreach (var resultFile in resultFiles)
            {
                log.LogInformation($"Getting result for file {resultFile.Name}");

                try
                {
                    var transcriptionResult = await BatchClient.GetSpeechTranscriptFromSasAsync(resultFile.Links.ContentUrl).ConfigureAwait(false);

                    if (string.IsNullOrEmpty(transcriptionResult.Source))
                    {
                        var errorMessage = "Transcription source is unknown, skipping evaluation.";
                        log.LogError(errorMessage);

                        generalErrorsStringBuilder.AppendLine(errorMessage);
                        continue;
                    }

                    var audioFileName = StorageConnector.GetFileNameFromUri(new Uri(transcriptionResult.Source));
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
                    await WriteErrorReportAsync(textAnalyticsSubmitErrors, jobName, log).ConfigureAwait(false);
                }

                log.LogInformation($"Added text analytics requests to service bus message - re-queueing message.");

                // Poll for first time with TA request after 1 minute
                await ServiceBusUtilities.SendServiceBusMessageAsync(FetchServiceBusSender, serviceBusMessage.CreateMessageString(), log, TimeSpan.FromMinutes(1)).ConfigureAwait(false);
                return;
            }

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

                await StorageConnectorInstance.WriteTextFileToBlobAsync(editedTranscriptionResultJson, FetchTranscriptionEnvironmentVariables.JsonResultOutputContainer, jsonFileName, log).ConfigureAwait(false);

                var consolidatedContainer = FetchTranscriptionEnvironmentVariables.ConsolidatedFilesOutputContainer;
                if (FetchTranscriptionEnvironmentVariables.CreateConsolidatedOutputFiles)
                {
                    var audioArchiveFileName = $"{archiveFileLocation}/{fileName}";
                    var jsonArchiveFileName = $"{archiveFileLocation}/{jsonFileName}";

                    await StorageConnectorInstance.MoveFileAsync(FetchTranscriptionEnvironmentVariables.AudioInputContainer, fileName, consolidatedContainer, audioArchiveFileName, true, log).ConfigureAwait(false);
                    await StorageConnectorInstance.WriteTextFileToBlobAsync(editedTranscriptionResultJson, consolidatedContainer, jsonArchiveFileName, log).ConfigureAwait(false);
                }

                if (FetchTranscriptionEnvironmentVariables.CreateHtmlResultFile)
                {
                    var htmlContainer = FetchTranscriptionEnvironmentVariables.HtmlResultOutputContainer;
                    var htmlFileName = $"{fileName}.html";
                    var displayResults = TranscriptionToHtml.ToHtml(speechTranscript, jobName);
                    await StorageConnectorInstance.WriteTextFileToBlobAsync(displayResults, htmlContainer, htmlFileName, log).ConfigureAwait(false);

                    if (FetchTranscriptionEnvironmentVariables.CreateConsolidatedOutputFiles)
                    {
                        var htmlArchiveFileName = $"{archiveFileLocation}/{htmlFileName}";
                        await StorageConnectorInstance.WriteTextFileToBlobAsync(displayResults, consolidatedContainer, htmlArchiveFileName, log).ConfigureAwait(false);
                    }
                }

                if (FetchTranscriptionEnvironmentVariables.UseSqlDatabase)
                {
                    var duration = string.IsNullOrEmpty(speechTranscript.Duration) ? TimeSpan.Zero : XmlConvert.ToTimeSpan(speechTranscript.Duration);
                    var approximatedCost = CostEstimation.GetCostEstimation(
                        duration,
                        speechTranscript.CombinedRecognizedPhrases.Count(),
                        serviceBusMessage.UsesCustomModel,
                        FetchTranscriptionEnvironmentVariables.SentimentAnalysisSetting,
                        FetchTranscriptionEnvironmentVariables.PiiRedactionSetting);

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

                if (FetchTranscriptionEnvironmentVariables.CreateAudioProcessedContainer)
                {
                    await StorageConnectorInstance.MoveFileAsync(FetchTranscriptionEnvironmentVariables.AudioInputContainer, fileName, FetchTranscriptionEnvironmentVariables.AudioProcessedContainer, fileName, false, log).ConfigureAwait(false);
                }
            }

            var generalErrors = generalErrorsStringBuilder.ToString();
            if (!string.IsNullOrEmpty(generalErrors))
            {
                await WriteErrorReportAsync(generalErrors, jobName, log).ConfigureAwait(false);
            }

            var reportFile = transcriptionFiles.Values.Where(t => t.Kind == TranscriptionFileKind.TranscriptionReport).FirstOrDefault();
            var reportFileContent = await BatchClient.GetTranscriptionReportFileFromSasAsync(reportFile.Links.ContentUrl).ConfigureAwait(false);
            await ProcessReportFileAsync(reportFileContent, log).ConfigureAwait(false);

            BatchClient.DeleteTranscriptionAsync(transcriptionLocation, subscriptionKey).ConfigureAwait(false).GetAwaiter().GetResult();
        }
    }
}
