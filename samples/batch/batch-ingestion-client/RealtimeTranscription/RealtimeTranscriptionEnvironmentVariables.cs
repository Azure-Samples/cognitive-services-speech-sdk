// <copyright file="RealtimeTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace RealtimeTranscription
{
    using System;

    public static class RealtimeTranscriptionEnvironmentVariables
    {
        public static readonly bool AddWordLevelTimestamps = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddWordLevelTimestamps), EnvironmentVariableTarget.Process), out AddWordLevelTimestamps) && AddWordLevelTimestamps;

        public static readonly string AudioInputContainer = Environment.GetEnvironmentVariable(nameof(AudioInputContainer), EnvironmentVariableTarget.Process);

        public static readonly string AudioProcessedContainer = Environment.GetEnvironmentVariable(nameof(AudioProcessedContainer), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesRegion = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesRegion), EnvironmentVariableTarget.Process);

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly string CustomEndpointId = Environment.GetEnvironmentVariable(nameof(CustomEndpointId), EnvironmentVariableTarget.Process);

        public static readonly string ErrorFilesOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorFilesOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string JsonResultOutputContainer = Environment.GetEnvironmentVariable(nameof(JsonResultOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string Locale = Environment.GetEnvironmentVariable(nameof(Locale), EnvironmentVariableTarget.Process);

        public static readonly string ProfanityFilterMode = Environment.GetEnvironmentVariable(nameof(ProfanityFilterMode), EnvironmentVariableTarget.Process);
    }
}
