// <copyright file="IngestionClientDbContextExtensions.cs" company="Microsoft Corporation">
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

    public static class IngestionClientDbContextExtensions
    {
        private const int MaxNBestsPerRecognizedPhrase = 1;

        public static async Task StoreTranscriptionAsync(
            this IngestionClientDbContext ingestionClientDbContext,
            Guid transcriptionId,
            string locale,
            string fileName,
            float approximateCost,
            SpeechTranscript speechTranscript)
        {
            _ = ingestionClientDbContext ?? throw new ArgumentNullException(nameof(ingestionClientDbContext));
            _ = speechTranscript ?? throw new ArgumentNullException(nameof(speechTranscript));

            var transcription = new Transcription(
                id: transcriptionId,
                locale: locale,
                name: fileName,
                source: speechTranscript.Source,
                timestamp: DateTime.Parse(speechTranscript.Timestamp, CultureInfo.InvariantCulture),
                duration: speechTranscript.Duration ?? string.Empty,
                durationInSeconds: TimeSpan.FromTicks(speechTranscript.DurationInTicks).TotalSeconds,
                numberOfChannels: speechTranscript.CombinedRecognizedPhrases.Count(),
                approximateCost: approximateCost);
            var combinedRecognizedPhrases = new List<CombinedRecognizedPhrase>();

            var phrasesByChannel = speechTranscript.RecognizedPhrases.GroupBy(t => t.Channel);

            foreach (var phrases in phrasesByChannel)
            {
                var channel = phrases.Key;
                var combinedPhrase = speechTranscript.CombinedRecognizedPhrases.Where(t => t.Channel == channel).FirstOrDefault();
                var combinedRecognizedPhraseDb = AddCombinedRecognizedPhrase(combinedPhrase, channel, phrases);
                combinedRecognizedPhrases.Add(combinedRecognizedPhraseDb);
            }

            transcription = transcription.WithCombinedRecognizedPhrases(combinedRecognizedPhrases);

            ingestionClientDbContext.Add(transcription);
            var entitiesAdded = await ingestionClientDbContext.SaveChangesAsync().ConfigureAwait(false);
        }

        private static CombinedRecognizedPhrase AddCombinedRecognizedPhrase(Connector.CombinedRecognizedPhrase combinedRecognizedPhrase, int channel, IEnumerable<Connector.RecognizedPhrase> recognizedPhrases)
        {
            var combinedRecognizedPhraseDb = new CombinedRecognizedPhrase(
                id: Guid.NewGuid(),
                channel: channel,
                lexical: combinedRecognizedPhrase?.Lexical ?? string.Empty,
                itn: combinedRecognizedPhrase?.Lexical ?? string.Empty,
                maskedItn: combinedRecognizedPhrase?.Lexical ?? string.Empty,
                display: combinedRecognizedPhrase?.Lexical ?? string.Empty,
                sentimentNegative: combinedRecognizedPhrase?.Sentiment?.Negative ?? 0d,
                sentimentNeutral: combinedRecognizedPhrase?.Sentiment?.Neutral ?? 0d,
                sentimentPositive: combinedRecognizedPhrase?.Sentiment?.Positive ?? 0d);

            var recognizedPhrasesDb = new List<RecognizedPhrase>();

            var orderedPhrases = recognizedPhrases.OrderBy(p => p.OffsetInTicks);
            var previousEndInMs = 0.0;
            foreach (var phrase in orderedPhrases)
            {
                var silenceBetweenCurrentAndPreviousSegmentInMs = Convert.ToInt32(Math.Max(0, TimeSpan.FromTicks(phrase.OffsetInTicks).TotalMilliseconds - previousEndInMs));

                var recognizedPhraseDb = AddRecognizedPhrase(phrase, silenceBetweenCurrentAndPreviousSegmentInMs);
                previousEndInMs = (TimeSpan.FromTicks(phrase.OffsetInTicks) + TimeSpan.FromTicks(phrase.DurationInTicks)).TotalMilliseconds;

                recognizedPhrasesDb.Add(recognizedPhraseDb);
            }

            combinedRecognizedPhraseDb = combinedRecognizedPhraseDb.WithRecognizedPhrases(recognizedPhrasesDb);
            return combinedRecognizedPhraseDb;
        }

        private static RecognizedPhrase AddRecognizedPhrase(Connector.RecognizedPhrase recognizedPhrase, int silenceBetweenCurrentAndPreviousSegmentInMs)
        {
            var recognizedPhraseDb = new RecognizedPhrase(
                id: Guid.NewGuid(),
                recognitionStatus: recognizedPhrase.RecognitionStatus,
                speaker: recognizedPhrase.Speaker,
                channel: recognizedPhrase.Channel,
                offset: recognizedPhrase.Offset,
                duration: recognizedPhrase.Duration,
                silenceBetweenCurrentAndPreviousSegmentInMs: silenceBetweenCurrentAndPreviousSegmentInMs);

            var nbestsDb = new List<NBest>();

            foreach (var nbestResult in recognizedPhrase.NBest.Take(MaxNBestsPerRecognizedPhrase))
            {
                var nbestDb = AddNBestResult(nbestResult);
                nbestsDb.Add(nbestDb);
            }

            recognizedPhraseDb = recognizedPhraseDb.WithNBests(nbestsDb);
            return recognizedPhraseDb;
        }

        private static NBest AddNBestResult(Connector.NBest nbest)
        {
            var nbestDb = new NBest(
                id: Guid.NewGuid(),
                confidence: nbest.Confidence,
                lexical: nbest.Lexical,
                itn: nbest.ITN,
                maskedItn: nbest.MaskedITN,
                display: nbest.Display,
                sentimentNegative: nbest.Sentiment?.Negative ?? 0d,
                sentimentNeutral: nbest.Sentiment?.Neutral ?? 0d,
                sentimentPositive: nbest.Sentiment?.Positive ?? 0d);

            if (nbest.Words != null)
            {
                var wordsDb = new List<Word>();

                foreach (var word in nbest.Words)
                {
                    var wordDb = CreateWord(word);
                    wordsDb.Add(wordDb);
                }

                nbestDb = nbestDb.WithWords(wordsDb);
            }

            return nbestDb;
        }

        private static Word CreateWord(Connector.Words word)
        {
            var wordDb = new Word(
                id: Guid.NewGuid(),
                wordText: word.Word,
                offset: word.Offset,
                duration: word.Duration,
                confidence: word.Confidence);

            return wordDb;
        }
    }
}