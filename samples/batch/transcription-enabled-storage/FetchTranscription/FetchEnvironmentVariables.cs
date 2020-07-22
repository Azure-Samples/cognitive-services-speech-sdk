// <copyright file="FetchEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;

    public static class FetchEnvironmentVariables
    {
        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly string FetchTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(FetchTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string TextAnalyticsKey = Environment.GetEnvironmentVariable(nameof(TextAnalyticsKey), EnvironmentVariableTarget.Process);

        public static readonly string TextAnalyticsRegion = Environment.GetEnvironmentVariable(nameof(TextAnalyticsRegion), EnvironmentVariableTarget.Process);

        public static readonly string AddSentimentAnalysis = Environment.GetEnvironmentVariable(nameof(AddSentimentAnalysis), EnvironmentVariableTarget.Process);

        public static readonly string AddEntityRedaction = Environment.GetEnvironmentVariable(nameof(AddEntityRedaction), EnvironmentVariableTarget.Process);

        public static readonly string UseSqlDatabase = Environment.GetEnvironmentVariable(nameof(UseSqlDatabase), EnvironmentVariableTarget.Process);

        public static readonly string JsonResultOutputContainer = Environment.GetEnvironmentVariable(nameof(JsonResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string HtmlResultOutputContainer = Environment.GetEnvironmentVariable(nameof(HtmlResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string DatabaseConnectionString = Environment.GetEnvironmentVariable(nameof(DatabaseConnectionString), EnvironmentVariableTarget.Process);
    }
}
