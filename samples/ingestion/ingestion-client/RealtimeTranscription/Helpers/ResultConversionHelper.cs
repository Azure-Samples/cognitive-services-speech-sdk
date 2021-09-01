// <copyright file="ResultConversionHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace RealtimeTranscription
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Xml;
    using Connector;
    using Microsoft.Extensions.Logging;

    public static class ResultConversionHelper
    {
        public static SpeechTranscript CreateBatchResultFromRealtimeResults(string sourceName, List<JsonResult> realtimeResults, ILogger logger)
        {
            var timestamp = DateTime.UtcNow.ToString("O", CultureInfo.InvariantCulture);
            var durationInTicks = realtimeResults.Select(jsonResult => jsonResult.Offset + jsonResult.Duration).Max();
            var duration = XmlConvert.ToString(TimeSpan.FromTicks(durationInTicks));

            logger.LogInformation("Converting combined recognized phrases");

            var nonEmptyResults = realtimeResults
                .Where(r => r.NBest.Any() && !string.IsNullOrEmpty(r.NBest.First().Lexical));

            var combinedRecognizedPhrases = nonEmptyResults
                .GroupBy(r => r.SpeakerId, p => p, (key, g) => new
                {
                    channel = int.TryParse(key, out var channel) ? channel : 0,
                    orderedResults = g.OrderBy(j => j.Offset).Select(t => t.NBest.First())
                })
                .Select(group => new CombinedRecognizedPhrase(
                    group.channel,
                    string.Join(" ", group.orderedResults.Select(g => g.Lexical)),
                    string.Join(" ", group.orderedResults.Select(g => g.ITN)),
                    string.Join(" ", group.orderedResults.Select(g => g.MaskedITN)),
                    string.Join(" ", group.orderedResults.Select(g => g.Display)),
                    null));

            logger.LogInformation("Converting recognized phrases");
            var recognizedPhrases = nonEmptyResults.Select(r => new RecognizedPhrase(
                 r.RecognitionStatus,
                 int.TryParse(r.SpeakerId, out var channel) ? channel : 0,
                 int.TryParse(r.SpeakerId, out var speaker) ? speaker : 0,
                 XmlConvert.ToString(TimeSpan.FromTicks(r.Offset)),
                 XmlConvert.ToString(TimeSpan.FromTicks(r.Duration)),
                 r.Offset,
                 r.Duration,
                 r.NBest));

            logger.LogInformation("Creating speech transcript from realtime results");
            return new SpeechTranscript(
                sourceName,
                timestamp,
                durationInTicks,
                duration,
                combinedRecognizedPhrases,
                recognizedPhrases);
        }
    }
}
