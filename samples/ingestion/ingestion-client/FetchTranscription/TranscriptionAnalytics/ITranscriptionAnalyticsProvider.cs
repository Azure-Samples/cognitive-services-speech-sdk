// <copyright file="ITranscriptionAnalyticsProvider.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System.Collections.Generic;
    using System.Threading.Tasks;

    using Connector;
    using Connector.Enums;
    using Connector.Serializable.TranscriptionStartedServiceBusMessage;

    public interface ITranscriptionAnalyticsProvider
    {
        Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobStatusAsync(IEnumerable<AudioFileInfo> audioFileInfos);

        Task<IEnumerable<string>> SubmitTranscriptionAnalyticsJobsAsync(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings);

        Task<IEnumerable<string>> AddTranscriptionAnalyticsResultsToTranscripts(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings);
    }
}
