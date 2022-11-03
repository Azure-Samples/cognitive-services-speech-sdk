// <copyright file="DatabaseConnector2.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database
{
    using System;
    using System.Collections.Generic;
    using System.Data.SqlClient;
    using System.Globalization;
    using System.Linq;
    using System.Threading.Channels;
    using System.Threading.Tasks;

    using Connector.Database.Models;

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

            var transcriptions = new Transcriptions(
                id: transcriptionId,
                locale: locale,
                name: fileName,
                source: speechTranscript.Source,
                timestamp: DateTime.Parse(speechTranscript.Timestamp, CultureInfo.InvariantCulture),
                duration: speechTranscript.Duration ?? string.Empty,
                durationInSeconds: TimeSpan.FromTicks(speechTranscript.DurationInTicks).TotalSeconds,
                numberOfChannels: speechTranscript.CombinedRecognizedPhrases.Count(),
                approximateCost: approximateCost,
                combinedRecognizedPhrases: null);


            var phrasesByChannel = speechTranscript.RecognizedPhrases.GroupBy(t => t.Channel);

            foreach (var phrases in phrasesByChannel)
            {
                var channel = phrases.Key;

                var combinedPhrase = speechTranscript.CombinedRecognizedPhrases.Where(t => t.Channel == channel).FirstOrDefault();

                var combinedRecognizedPhrases = new CombinedRecognizedPhrases(
                    id: Guid.NewGuid(),
                    channel: channel,
                    lexical: combinedPhrase?.Lexical ?? string.Empty,
                    itn: combinedPhrase?.Lexical ?? string.Empty,
                    maskedItn: combinedPhrase?.Lexical ?? string.Empty,
                    display: combinedPhrase?.Lexical ?? string.Empty,
                    sentimentNegative: combinedPhrase?.Sentiment?.Negative ?? 0d,
                    sentimentNeutral: combinedPhrase?.Sentiment?.Neutral ?? 0d,
                    sentimentPositive: combinedPhrase?.Sentiment?.Positive ?? 0d,
                    recognizedPhrases: null);

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
                        



                    var phraseId = Guid.NewGuid();
                    var query = "INSERT INTO dbo.RecognizedPhrases (ID, CombinedRecognizedPhraseID, RecognitionStatus, Speaker, Channel, Offset, Duration, SilenceBetweenCurrentAndPreviousSegmentInMs)" +
                        " VALUES (@id, @combinedRecognizedPhraseID, @recognitionStatus, @speaker, @channel, @offset, @duration, @silenceBetweenCurrentAndPreviousSegmentInMs)";

                    using var command = new SqlCommand(query, this.connection);
                    command.Parameters.AddWithValue("@id", phraseId);
                    command.Parameters.AddWithValue("@combinedRecognizedPhraseID", combinedPhraseID);
                    command.Parameters.AddWithValue("@recognitionStatus", recognizedPhrase.RecognitionStatus);
                    command.Parameters.AddWithValue("@speaker", recognizedPhrase.Speaker);
                    command.Parameters.AddWithValue("@channel", recognizedPhrase.Channel);
                    command.Parameters.AddWithValue("@offset", recognizedPhrase.Offset);
                    command.Parameters.AddWithValue("@duration", recognizedPhrase.Duration);
                    command.Parameters.AddWithValue("@silenceBetweenCurrentAndPreviousSegmentInMs", silenceBetweenCurrentAndPreviousSegmentInMs);

                    var result = await command.ExecuteNonQueryAsync().ConfigureAwait(false);

                    if (result < 0)
                    {
                        this.logger.LogInformation("Did not store phrase in Db, command did not update table");
                    }
                    else
                    {
                        foreach (var nbestResult in recognizedPhrase.NBest)
                        {
                            await this.StoreNBestAsync(phraseId, nbestResult).ConfigureAwait(false);
                        }
                    }


                    previousEndInMs = (TimeSpan.FromTicks(phrase.OffsetInTicks) + TimeSpan.FromTicks(phrase.DurationInTicks)).TotalMilliseconds;
                }
            }


           


        }
    }
}