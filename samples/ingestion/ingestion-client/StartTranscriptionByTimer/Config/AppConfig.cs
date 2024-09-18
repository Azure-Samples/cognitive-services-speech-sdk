// <copyright file="AppConfig.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;

    using Connector.Constants;

    public class AppConfig
    {
        private int messagesPerFunctionExecution = Constants.DefaultMessagesPerFunctionExecution;

        private int filesPerTranscriptionJob = Constants.DefaultFilesPerTranscriptionJob;

        private int initialPollingDelayInMinutes = Constants.DefaultInitialPollingDelayInMinutes;
        private int retryLimit = Constants.DefaultRetryLimit;

        public bool AddDiarization { get; set; }

        public bool AddWordLevelTimestamps { get; set; }

        public bool IsAzureGovDeployment { get; set; }

        public bool IsByosEnabledSubscription { get; set; }

        public int MessagesPerFunctionExecution
        {
            get => this.messagesPerFunctionExecution;
            set => this.messagesPerFunctionExecution = Math.Clamp(value, 1, Constants.MaxMessagesPerFunctionExecution);
        }

        public int FilesPerTranscriptionJob
        {
            get => this.filesPerTranscriptionJob;
            set => this.filesPerTranscriptionJob = Math.Clamp(value, 1, Constants.MaxFilesPerTranscriptionJob);
        }

        public int RetryLimit
        {
            get => this.retryLimit;
            set => this.retryLimit = Math.Clamp(value, 1, Constants.MaxRetryLimit);
        }

        public int InitialPollingDelayInMinutes
        {
            get => this.initialPollingDelayInMinutes;
            set => this.initialPollingDelayInMinutes = Math.Clamp(value, 2, Constants.MaxInitialPollingDelayInMinutes);
        }

        public int MaxPollingDelayInMinutes { get; set; } = Constants.DefaultMaxPollingDelayInMinutes;

        public string AudioInputContainer { get; set; }

        public string AzureServiceBus { get; set; }

        public string AzureSpeechServicesKey { get; set; }

        public string AzureSpeechServicesEndpointUri { get; set; }

        public string AzureWebJobsStorage { get; set; }

        public string CustomModelId { get; set; }

        public string ErrorFilesOutputContainer { get; set; }

        public string ErrorReportOutputContainer { get; set; }

        public string FetchTranscriptionServiceBusConnectionString { get; set; }

        public string Locale { get; set; }

        public string ProfanityFilterMode { get; set; }

        public string PunctuationMode { get; set; }

        public string StartTranscriptionServiceBusConnectionString { get; set; }

        public string StartTranscriptionFunctionTimeInterval { get; set; }

        public string Version { get; set; }
    }
}