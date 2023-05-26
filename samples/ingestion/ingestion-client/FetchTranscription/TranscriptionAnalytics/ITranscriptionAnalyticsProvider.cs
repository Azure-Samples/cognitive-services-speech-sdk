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
        /// <summary>
        /// Gets the status of the transcription analytics jobs that are monitored by the provider
        /// </summary>
        /// <param name="audioFileInfos">The audio file infos with transcription analytics jobs info</param>
        /// <returns>The overall status of all jobs monitored by the provider</returns>
        Task<TranscriptionAnalyticsJobStatus> GetTranscriptionAnalyticsJobStatusAsync(IEnumerable<AudioFileInfo> audioFileInfos);

        /// <summary>
        /// Submits transcription analytics jobs based on the transcript in speechtranscript and sets the job ids in the corresponding audio file infos.
        /// </summary>
        /// <param name="speechTranscriptMappings">The mapping from audio file info to transcript</param>
        /// <returns>The errors if any.</returns>
        Task<IEnumerable<string>> SubmitTranscriptionAnalyticsJobsAsync(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings);

        /// <summary>
        /// Fetches the transcription analytics results and adds them to the corresponding speech transcript
        /// </summary>
        /// <param name="speechTranscriptMappings">The mapping from audio file info to transcript</param>
        /// <returns>The errors if any.</returns>
        Task<IEnumerable<string>> AddTranscriptionAnalyticsResultsToTranscriptsAsync(Dictionary<AudioFileInfo, SpeechTranscript> speechTranscriptMappings);
    }
}
