// <copyright file="Constants.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Constants
{
    public static class Constants
    {
        public const int MaxRetryLimit = 16;

        public const int DefaultRetryLimit = 4;

        public const int MaxInitialPollingDelayInMinutes = 120;

        public const int DefaultInitialPollingDelayInMinutes = 2;

        public const int DefaultMaxPollingDelayInMinutes = 120;

        public const char Delimiter = ';';

        public const int MaxMessagesPerFunctionExecution = 10000;

        public const int DefaultMessagesPerFunctionExecution = 2000;

        public const int MaxFilesPerTranscriptionJob = 1000;

        public const int DefaultFilesPerTranscriptionJob = 100;

        public const int DefaultConversationAnalysisMaxChunkSize = 5000;

        public const string SummarizationSupportedLocalePrefix = "en";
    }
}