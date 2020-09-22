// <copyright file="FetchTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public static class FetchTranscriptionEnvironmentVariables
    {
        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly bool CreateHtmlResultFile = bool.TryParse(Environment.GetEnvironmentVariable(nameof(CreateHtmlResultFile), EnvironmentVariableTarget.Process), out CreateHtmlResultFile) && CreateHtmlResultFile;

        public static readonly string FetchTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(FetchTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string TextAnalyticsKey = Environment.GetEnvironmentVariable(nameof(TextAnalyticsKey), EnvironmentVariableTarget.Process);

        public static readonly string TextAnalyticsRegion = Environment.GetEnvironmentVariable(nameof(TextAnalyticsRegion), EnvironmentVariableTarget.Process);

        public static readonly bool UseSqlDatabase = bool.TryParse(Environment.GetEnvironmentVariable(nameof(UseSqlDatabase), EnvironmentVariableTarget.Process), out UseSqlDatabase) && UseSqlDatabase;

        public static readonly string JsonResultOutputContainer = Environment.GetEnvironmentVariable(nameof(JsonResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string HtmlResultOutputContainer = Environment.GetEnvironmentVariable(nameof(HtmlResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string DatabaseConnectionString = Environment.GetEnvironmentVariable(nameof(DatabaseConnectionString), EnvironmentVariableTarget.Process);
    }
}
