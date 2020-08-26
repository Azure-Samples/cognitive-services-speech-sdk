// <copyright file="CreateTranscriptionEnvironmentVariables.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public static class CreateTranscriptionEnvironmentVariables
    {
        public static readonly string AzureServiceBus = Environment.GetEnvironmentVariable(nameof(AzureServiceBus), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesKey = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesKey), EnvironmentVariableTarget.Process);

        public static readonly string AzureSpeechServicesRegion = Environment.GetEnvironmentVariable(nameof(AzureSpeechServicesRegion), EnvironmentVariableTarget.Process);

        public static readonly string AudioInputContainer = Environment.GetEnvironmentVariable(nameof(AudioInputContainer), EnvironmentVariableTarget.Process);

        public static readonly bool AddSentimentAnalysis = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddSentimentAnalysis), EnvironmentVariableTarget.Process), out AddSentimentAnalysis) && AddSentimentAnalysis;

        public static readonly bool AddEntityRedaction = bool.TryParse(Environment.GetEnvironmentVariable(nameof(AddEntityRedaction), EnvironmentVariableTarget.Process), out AddEntityRedaction) && AddEntityRedaction;

        public static readonly bool CreateReceiptFile = bool.TryParse(Environment.GetEnvironmentVariable(nameof(CreateReceiptFile), EnvironmentVariableTarget.Process), out CreateReceiptFile) && CreateReceiptFile;

        public static readonly string Locale = Environment.GetEnvironmentVariable(nameof(Locale), EnvironmentVariableTarget.Process);

        public static readonly string SecondaryLocale = Environment.GetEnvironmentVariable(nameof(SecondaryLocale), EnvironmentVariableTarget.Process);

        public static readonly string ErrorReportOutputContainer = Environment.GetEnvironmentVariable(nameof(ErrorReportOutputContainer), EnvironmentVariableTarget.Process);

        public static readonly string CreateTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(CreateTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string AzureWebJobsStorage = Environment.GetEnvironmentVariable(nameof(AzureWebJobsStorage), EnvironmentVariableTarget.Process);

        public static readonly string ReceiptsContainer = Environment.GetEnvironmentVariable(nameof(ReceiptsContainer), EnvironmentVariableTarget.Process);

        public static readonly string AudioDatasetSize = Environment.GetEnvironmentVariable(nameof(AudioDatasetSize), EnvironmentVariableTarget.Process);

        public static readonly string AcousticModelId = Environment.GetEnvironmentVariable(nameof(AcousticModelId), EnvironmentVariableTarget.Process);

        public static readonly string SecondaryAcousticModelId = Environment.GetEnvironmentVariable(nameof(SecondaryAcousticModelId), EnvironmentVariableTarget.Process);

        public static readonly string LanguageModelId = Environment.GetEnvironmentVariable(nameof(LanguageModelId), EnvironmentVariableTarget.Process);

        public static readonly string SecondaryLanguageModelId = Environment.GetEnvironmentVariable(nameof(SecondaryLanguageModelId), EnvironmentVariableTarget.Process);

        public static readonly string FetchTranscriptionServiceBusConnectionString = Environment.GetEnvironmentVariable(nameof(FetchTranscriptionServiceBusConnectionString), EnvironmentVariableTarget.Process);

        public static readonly string ProfanityFilterMode = Environment.GetEnvironmentVariable(nameof(ProfanityFilterMode), EnvironmentVariableTarget.Process);

        public static readonly string PunctuationMode = Environment.GetEnvironmentVariable(nameof(PunctuationMode), EnvironmentVariableTarget.Process);

        public static readonly string AddDiarization = Environment.GetEnvironmentVariable(nameof(AddDiarization), EnvironmentVariableTarget.Process);

        public static readonly string AddWordLevelTimestamps = Environment.GetEnvironmentVariable(nameof(AddWordLevelTimestamps), EnvironmentVariableTarget.Process);
    }
}
