// <copyright file="FetchTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using Connector;
    using Connector.Constants;
    using Connector.Enums;
    using Connector.Serializable.Language.Conversations;

    public static class FetchTranscriptionEnvironmentVariables
    {
        public static readonly SentimentAnalysisSetting SentimentAnalysisSetting = Enum.TryParse(Environment.GetEnvironmentVariable(nameof(SentimentAnalysisSetting), EnvironmentVariableTarget.Process), out SentimentAnalysisSetting) ? SentimentAnalysisSetting : SentimentAnalysisSetting.None;

        public static readonly PiiRedactionSetting PiiRedactionSetting = Enum.TryParse(Environment.GetEnvironmentVariable(nameof(PiiRedactionSetting), EnvironmentVariableTarget.Process), out PiiRedactionSetting) ? PiiRedactionSetting : PiiRedactionSetting.None;

        public static readonly bool CreateHtmlResultFile = bool.TryParse(Environment.GetEnvironmentVariable(nameof(CreateHtmlResultFile), EnvironmentVariableTarget.Process), out CreateHtmlResultFile) && CreateHtmlResultFile;

        public static readonly ConversationPiiSetting ConversationPiiSetting = Enum.TryParse(Environment.GetEnvironmentVariable(nameof(ConversationPiiSetting), EnvironmentVariableTarget.Process), out ConversationPiiSetting) ? ConversationPiiSetting : ConversationPiiSetting.None;

        public static readonly string ConversationPiiCategories = Environment.GetEnvironmentVariable(nameof(ConversationPiiCategories), EnvironmentVariableTarget.Process);

        public static readonly string ConversationPiiInferenceSource = Environment.GetEnvironmentVariable(nameof(ConversationPiiInferenceSource), EnvironmentVariableTarget.Process);

        public static readonly int ConversationPiiMaxChunkSize = int.TryParse(Environment.GetEnvironmentVariable(nameof(ConversationPiiMaxChunkSize), EnvironmentVariableTarget.Process), out ConversationPiiMaxChunkSize) ? ConversationPiiMaxChunkSize : Constants.DefaultConversationAnalysisMaxChunkSize;

        public static readonly ConversationSummarizationOptions ConversationSummarizationOptions = string.IsNullOrEmpty(Environment.GetEnvironmentVariable(nameof(ConversationSummarizationOptions), EnvironmentVariableTarget.Process)) ? new ConversationSummarizationOptions() : System.Text.Json.JsonSerializer.Deserialize<ConversationSummarizationOptions>(Environment.GetEnvironmentVariable(nameof(ConversationSummarizationOptions), EnvironmentVariableTarget.Process));

        public static readonly bool UseSqlDatabase = bool.TryParse(Environment.GetEnvironmentVariable(nameof(UseSqlDatabase), EnvironmentVariableTarget.Process), out UseSqlDatabase) && UseSqlDatabase;

        public static readonly int InitialPollingDelayInMinutes = int.TryParse(Environment.GetEnvironmentVariable(nameof(InitialPollingDelayInMinutes), EnvironmentVariableTarget.Process), out InitialPollingDelayInMinutes) ? InitialPollingDelayInMinutes.ClampInt(2, Constants.MaxInitialPollingDelayInMinutes) : Constants.DefaultInitialPollingDelayInMinutes;

        public static readonly int MaxPollingDelayInMinutes = int.TryParse(Environment.GetEnvironmentVariable(nameof(MaxPollingDelayInMinutes), EnvironmentVariableTarget.Process), out MaxPollingDelayInMinutes) ? MaxPollingDelayInMinutes : Constants.DefaultMaxPollingDelayInMinutes;

        public static readonly int RetryLimit = int.TryParse(Environment.GetEnvironmentVariable(nameof(RetryLimit), EnvironmentVariableTarget.Process), out RetryLimit) ? RetryLimit.ClampInt(1, Constants.MaxRetryLimit) : Constants.DefaultRetryLimit;

        public static readonly string AudioInputContainer = Environment.GetEnvironmentVariable(nameof(AudioInputContainer), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly string DatabaseConnectionString = Environment.GetEnvironmentVariable(nameof(DatabaseConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string ErrorFilesOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorFilesOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string FetchTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(FetchTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string HtmlResultOutputContainer = Environment.GetEnvironmentVariable(nameof(HtmlResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string JsonResultOutputContainer = Environment.GetEnvironmentVariable(nameof(JsonResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string StartTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(StartTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string TextAnalyticsKey = Environment.GetEnvironmentVariable(nameof(TextAnalyticsKey), EnvironmentVariableTarget.Process);

        public static readonly string TextAnalyticsEndpoint = Environment.GetEnvironmentVariable(nameof(TextAnalyticsEndpoint), EnvironmentVariableTarget.Process);

        public static readonly string PiiCategories = Environment.GetEnvironmentVariable(nameof(PiiCategories), EnvironmentVariableTarget.Process);

        public static readonly bool CreateConsolidatedOutputFiles = bool.TryParse(Environment.GetEnvironmentVariable(nameof(CreateConsolidatedOutputFiles), EnvironmentVariableTarget.Process), out CreateConsolidatedOutputFiles) && CreateConsolidatedOutputFiles;

        public static readonly string ConsolidatedFilesOutputContainer = Environment.GetEnvironmentVariable(nameof(ConsolidatedFilesOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly bool CreateAudioProcessedContainer = bool.TryParse(Environment.GetEnvironmentVariable(nameof(CreateAudioProcessedContainer), EnvironmentVariableTarget.Process), out CreateAudioProcessedContainer) && CreateAudioProcessedContainer;

        public static readonly string AudioProcessedContainer = Environment.GetEnvironmentVariable(nameof(AudioProcessedContainer), EnvironmentVariableTarget.Process);
    }
}
