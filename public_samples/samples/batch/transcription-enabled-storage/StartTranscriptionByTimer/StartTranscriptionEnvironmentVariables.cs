// <copyright file="StartTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using Connector.Extensions;

    public static class StartTranscriptionEnvironmentVariables
    {
        public static readonly bool AddDiarization = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddDiarization), EnvironmentVariableTarget.Process), out AddDiarization) && AddDiarization;

        public static readonly bool AddWordLevelTimestamps = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddWordLevelTimestamps), EnvironmentVariableTarget.Process), out AddWordLevelTimestamps) && AddWordLevelTimestamps;

        public static readonly int MessagesPerFunctionExecution = int.TryParse(Environment.GetEnvironmentVariable(nameof(MessagesPerFunctionExecution), EnvironmentVariableTarget.Process), out MessagesPerFunctionExecution) ? MessagesPerFunctionExecution.ClampInt(1, MaxMessagesPerFunctionExecution) : DefaultMessagesPerFunctionExecution;

        public static readonly int FilesPerTranscriptionJob = int.TryParse(Environment.GetEnvironmentVariable(nameof(FilesPerTranscriptionJob), EnvironmentVariableTarget.Process), out FilesPerTranscriptionJob) ? FilesPerTranscriptionJob.ClampInt(1, MaxFilesPerTranscriptionJob) : DefaultFilesPerTranscriptionJob;

        public static readonly int RetryLimit = int.TryParse(Environment.GetEnvironmentVariable(nameof(RetryLimit), EnvironmentVariableTarget.Process), out RetryLimit) ? RetryLimit.ClampInt(1, MaxRetryLimit) : DefaultRetryLimit;

        public static readonly string AudioInputContainer = Environment.GetEnvironmentVariable(nameof(AudioInputContainer), EnvironmentVariableTarget.Process);

        public static readonly string AzureServiceBus = Environment.GetEnvironmentVariable(nameof(AzureServiceBus), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesRegion = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesRegion), EnvironmentVariableTarget.Process);

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly string CustomModelId = Environment.GetEnvironmentVariable(nameof(CustomModelId), EnvironmentVariableTarget.Process);

        public static readonly string ErrorFilesOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorFilesOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string FetchTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(FetchTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string Locale = Environment.GetEnvironmentVariable(nameof(Locale), EnvironmentVariableTarget.Process);

        public static readonly string ProfanityFilterMode = Environment.GetEnvironmentVariable(nameof(ProfanityFilterMode), EnvironmentVariableTarget.Process);

        public static readonly string PunctuationMode = Environment.GetEnvironmentVariable(nameof(PunctuationMode), EnvironmentVariableTarget.Process);

        public static readonly string SecondaryCustomModelId = Environment.GetEnvironmentVariable(nameof(SecondaryCustomModelId), EnvironmentVariableTarget.Process);

        public static readonly string SecondaryLocale = Environment.GetEnvironmentVariable(nameof(SecondaryLocale), EnvironmentVariableTarget.Process);

        public static readonly string StartTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(StartTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        private const int MaxMessagesPerFunctionExecution = 5000;

        private const int DefaultMessagesPerFunctionExecution = 500;

        private const int MaxFilesPerTranscriptionJob = 1000;

        private const int DefaultFilesPerTranscriptionJob = 16;

        private const int MaxRetryLimit = 16;

        private const int DefaultRetryLimit = 4;
    }
}
