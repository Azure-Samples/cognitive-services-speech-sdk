// <copyright file="StartTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using Connector;
    using Connector.Constants;

    public static class StartTranscriptionEnvironmentVariables
    {
        public static readonly bool AddDiarization = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddDiarization), EnvironmentVariableTarget.Process), out AddDiarization) && AddDiarization;

        public static readonly bool AddWordLevelTimestamps = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddWordLevelTimestamps), EnvironmentVariableTarget.Process), out AddWordLevelTimestamps) && AddWordLevelTimestamps;

        public static readonly bool IsAzureGovDeployment = bool.TryParse(Environment.GetEnvironmentVariable(nameof(IsAzureGovDeployment), EnvironmentVariableTarget.Process), out IsAzureGovDeployment) && IsAzureGovDeployment;

        // BYOS = Bring your own storage (https://docs.microsoft.com/azure/cognitive-services/speech-service/speech-encryption-of-data-at-rest#bring-your-own-storage-byos-for-customization-and-logging)
        public static readonly bool IsByosEnabledSubscription = bool.TryParse(Environment.GetEnvironmentVariable(nameof(IsByosEnabledSubscription), EnvironmentVariableTarget.Process), out IsByosEnabledSubscription) && IsByosEnabledSubscription;

        public static readonly int MessagesPerFunctionExecution = int.TryParse(Environment.GetEnvironmentVariable(nameof(MessagesPerFunctionExecution), EnvironmentVariableTarget.Process), out MessagesPerFunctionExecution) ? MessagesPerFunctionExecution.ClampInt(1, Constants.MaxMessagesPerFunctionExecution) : Constants.DefaultMessagesPerFunctionExecution;

        public static readonly int FilesPerTranscriptionJob = int.TryParse(Environment.GetEnvironmentVariable(nameof(FilesPerTranscriptionJob), EnvironmentVariableTarget.Process), out FilesPerTranscriptionJob) ? FilesPerTranscriptionJob.ClampInt(1, Constants.MaxFilesPerTranscriptionJob) : Constants.DefaultFilesPerTranscriptionJob;

        public static readonly int RetryLimit = int.TryParse(Environment.GetEnvironmentVariable(nameof(RetryLimit), EnvironmentVariableTarget.Process), out RetryLimit) ? RetryLimit.ClampInt(1, Constants.MaxRetryLimit) : Constants.DefaultRetryLimit;

        public static readonly int InitialPollingDelayInMinutes = int.TryParse(Environment.GetEnvironmentVariable(nameof(InitialPollingDelayInMinutes), EnvironmentVariableTarget.Process), out InitialPollingDelayInMinutes) ? InitialPollingDelayInMinutes.ClampInt(2, Constants.MaxInitialPollingDelayInMinutes) : Constants.DefaultInitialPollingDelayInMinutes;

        public static readonly int MaxPollingDelayInMinutes = int.TryParse(Environment.GetEnvironmentVariable(nameof(MaxPollingDelayInMinutes), EnvironmentVariableTarget.Process), out MaxPollingDelayInMinutes) ? MaxPollingDelayInMinutes : Constants.DefaultMaxPollingDelayInMinutes;

        public static readonly string AudioInputContainer = Environment.GetEnvironmentVariable(nameof(AudioInputContainer), EnvironmentVariableTarget.Process);

        public static readonly string AzureServiceBus = Environment.GetEnvironmentVariable(nameof(AzureServiceBus), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesEndpointUri = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesEndpointUri), EnvironmentVariableTarget.Process).TrimEnd('/') + '/';

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly string CustomModelId = Environment.GetEnvironmentVariable(nameof(CustomModelId), EnvironmentVariableTarget.Process);

        public static readonly string ErrorFilesOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorFilesOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string FetchTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(FetchTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string Locale = Environment.GetEnvironmentVariable(nameof(Locale), EnvironmentVariableTarget.Process);

        public static readonly string ProfanityFilterMode = Environment.GetEnvironmentVariable(nameof(ProfanityFilterMode), EnvironmentVariableTarget.Process);

        public static readonly string PunctuationMode = Environment.GetEnvironmentVariable(nameof(PunctuationMode), EnvironmentVariableTarget.Process);

        public static readonly string StartTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(StartTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);
    }
}
