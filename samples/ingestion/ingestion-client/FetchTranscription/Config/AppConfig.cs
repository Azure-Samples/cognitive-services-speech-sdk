// <copyright file="AppConfig.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using Connector.Constants;
    using Connector.Enums;
    using Connector.Serializable.Language.Conversations;

    public class AppConfig
    {
        private ConversationSummarizationOptions conversationSummarizationOptions;

        private int initialPollingDelayInMinutes = Constants.DefaultInitialPollingDelayInMinutes;

        private int retryLimit = Constants.DefaultRetryLimit;

        public SentimentAnalysisSetting SentimentAnalysisSetting { get; set; } = SentimentAnalysisSetting.None;

        public PiiRedactionSetting PiiRedactionSetting { get; set; } = PiiRedactionSetting.None;

        public bool CreateHtmlResultFile { get; set; }

        public ConversationPiiSetting ConversationPiiSetting { get; set; } = ConversationPiiSetting.None;

        public string ConversationPiiCategories { get; set; }

        public string ConversationPiiInferenceSource { get; set; }

        public int ConversationPiiMaxChunkSize { get; set; } = Constants.DefaultConversationAnalysisMaxChunkSize;

        public ConversationSummarizationOptions ConversationSummarizationOptions
        {
            get
            {
                if (this.conversationSummarizationOptions == null)
                {
                    var envVarValue = Environment.GetEnvironmentVariable(nameof(this.ConversationSummarizationOptions), EnvironmentVariableTarget.Process);
                    this.conversationSummarizationOptions = string.IsNullOrEmpty(envVarValue)
                        ? new ConversationSummarizationOptions()
                        : System.Text.Json.JsonSerializer.Deserialize<ConversationSummarizationOptions>(envVarValue);
                }

                return this.conversationSummarizationOptions;
            }

            set
            {
                this.conversationSummarizationOptions = value;
            }
        }

        public bool UseSqlDatabase { get; set; }

        public int InitialPollingDelayInMinutes
        {
            get => this.initialPollingDelayInMinutes;
            set => this.initialPollingDelayInMinutes = Math.Clamp(value, 2, Constants.MaxInitialPollingDelayInMinutes);
        }

        public int MaxPollingDelayInMinutes { get; set; } = Constants.DefaultMaxPollingDelayInMinutes;

        public int RetryLimit
        {
            get => this.retryLimit;
            set => this.retryLimit = Math.Clamp(value, 1, Constants.MaxRetryLimit);
        }

        public string AudioInputContainer { get; set; }

        public string AzureSpeechServicesKey { get; set; }

        public string AzureWebJobsStorage { get; set; }

        public string DatabaseConnectionString { get; set; }

        public string ErrorFilesOutputContainer { get; set; }

        public string ErrorReportOutputContainer { get; set; }

        public string FetchTranscriptionServiceBusConnectionString { get; set; }

        public string CompletedServiceBusConnectionString { get; set; }

        public string HtmlResultOutputContainer { get; set; }

        public string JsonResultOutputContainer { get; set; }

        public string StartTranscriptionServiceBusConnectionString { get; set; }

        public string TextAnalyticsKey { get; set; }

        public string TextAnalyticsEndpoint { get; set; }

        public string PiiCategories { get; set; }

        public bool CreateConsolidatedOutputFiles { get; set; }

        public string ConsolidatedFilesOutputContainer { get; set; }

        public bool CreateAudioProcessedContainer { get; set; }

        public string AudioProcessedContainer { get; set; }

        public string Version { get; set; }
    }
}