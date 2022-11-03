// <copyright file="DatabaseConnector2.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Threading.Tasks;

    using Connector.Database.Models;

    using Microsoft.EntityFrameworkCore;
    using Microsoft.Extensions.Logging;

    public class DatabaseConnector2
    {
        private readonly ILogger logger;

        private readonly IngestionClientDbContext ingestionClientDbContext;

        public DatabaseConnector2(ILogger logger, IngestionClientDbContext ingestionClientDbContext)
        {
            this.logger = logger;
            this.ingestionClientDbContext = ingestionClientDbContext;
        }

        public async Task<bool> StoreTranscriptionAsync(
            Guid transcriptionId,
            string locale,
            string fileName,
            float approximateCost,
            SpeechTranscript speechTranscript)
        {
            _ = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var transcription = new Transcriptions(
            id: transcriptionId,
            locale: locale,
            name: fileName,
            source: speechTranscript.Source,
            timestamp: DateTime.Parse(speechTranscript.Timestamp, CultureInfo.InvariantCulture),
            duration: speechTranscript.Duration ?? string.Empty,
            durationInSeconds: TimeSpan.FromTicks(speechTranscript.DurationInTicks).TotalSeconds,
            numberOfChannels: speechTranscript.CombinedRecognizedPhrases.Count(),
            approximateCost: approximateCost);
            var combinedRecognizedPhrases = new List<CombinedRecognizedPhrases>();

            var phrasesByChannel = speechTranscript.RecognizedPhrases.GroupBy(t => t.Channel);

            foreach (var phrases in phrasesByChannel)
            {
                var channel = phrases.Key;

                var combinedPhrase = speechTranscript.CombinedRecognizedPhrases.Where(t => t.Channel == channel).FirstOrDefault();

                var combinedRecognizedPhrase = new CombinedRecognizedPhrases(
                    id: Guid.NewGuid(),
                    channel: channel,
                    lexical: combinedPhrase?.Lexical ?? string.Empty,
                    itn: combinedPhrase?.Lexical ?? string.Empty,
                    maskedItn: combinedPhrase?.Lexical ?? string.Empty,
                    display: combinedPhrase?.Lexical ?? string.Empty,
                    sentimentNegative: combinedPhrase?.Sentiment?.Negative ?? 0d,
                    sentimentNeutral: combinedPhrase?.Sentiment?.Neutral ?? 0d,
                    sentimentPositive: combinedPhrase?.Sentiment?.Positive ?? 0d);

                var recognizedPhrases = new List<RecognizedPhrases>();

                var orderedPhrases = phrases.OrderBy(p => p.OffsetInTicks);
                var previousEndInMs = 0.0;
                foreach (var phrase in orderedPhrases)
                {
                    var silenceBetweenCurrentAndPreviousSegmentInMs = Math.Max(0, TimeSpan.FromTicks(phrase.OffsetInTicks).TotalMilliseconds - previousEndInMs);

                    var recognizedPhrases = new RecognizedPhrases(
                        id: Guid.NewGuid(),
                        recognitionStatus: phrase.RecognitionStatus,
                        speaker: phrase.Speaker,
                        channel: phrase.Channel,
                        offset: phrase.Offset,
                        duration: phrase.Duration,
                        silenceBetweenCurrentAndPreviousSegmentInMs: silenceBetweenCurrentAndPreviousSegmentInMs,
                        nBests: null);

                    previousEndInMs = (TimeSpan.FromTicks(phrase.OffsetInTicks) + TimeSpan.FromTicks(phrase.DurationInTicks)).TotalMilliseconds;

                    foreach (var nbestResult in phrase.NBest)
                    {
                        var nbests = new NBests(
                            id: Guid.NewGuid(),
                            confidence: nbestResult.Confidence,
                            lexical: nbestResult.Lexical,
                            itn: nbestResult.ITN,
                            maskedItn: nbestResult.MaskedITN,
                            display: nbestResult.Display,
                            sentimentNegative: nbestResult.Sentiment?.Negative ?? 0d,
                            sentimentNeutral: nbestResult.Sentiment?.Neutral ?? 0d,
                            sentimentPositive: nbestResult.Sentiment?.Positive ?? 0d,
                            words: null);

                        if (nbests.Words == null)
                        {
                            foreach (var word in nbests.Words)
                            {
                                var words = new Words(
                                    id: Guid.NewGuid(),
                                    word: word.Word,
                                    offset: word.Offset,
                                    duration: word.Duration,
                                    confidence: word.Confidence);

                                await this.StoreWordsAsync(nbestID, word).ConfigureAwait(false);
                            }
                        }
                    }
                }

                combinedRecognizedPhrases.Add(combinedRecognizedPhrase);
            }

            transcription = transcription.WithCombinedRecognizedPhrases(combinedRecognizedPhrases);




        }
    }
}