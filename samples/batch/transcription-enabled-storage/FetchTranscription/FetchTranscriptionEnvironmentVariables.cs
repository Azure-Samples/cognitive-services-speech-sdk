// <copyright file="FetchTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;

    public static class FetchTranscriptionEnvironmentVariables
    {
        public static readonly bool AddEntityRedaction = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddEntityRedaction), EnvironmentVariableTarget.Process), out AddEntityRedaction) && AddEntityRedaction;

        public static readonly bool AddSentimentAnalysis = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddSentimentAnalysis), EnvironmentVariableTarget.Process), out AddSentimentAnalysis) && AddSentimentAnalysis;

        public static readonly bool CreateHtmlResultFile = bool.TryParse(Environment.GetEnvironmentVariable(nameof(CreateHtmlResultFile), EnvironmentVariableTarget.Process), out CreateHtmlResultFile) && CreateHtmlResultFile;

        public static readonly bool UseSqlDatabase = bool.TryParse(Environment.GetEnvironmentVariable(nameof(UseSqlDatabase), EnvironmentVariableTarget.Process), out UseSqlDatabase) && UseSqlDatabase;

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

        public static readonly string TextAnalyticsRegion = Environment.GetEnvironmentVariable(nameof(TextAnalyticsRegion), EnvironmentVariableTarget.Process);
    }
}
