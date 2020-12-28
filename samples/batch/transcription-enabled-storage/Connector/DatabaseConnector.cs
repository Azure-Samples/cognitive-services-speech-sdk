// <copyright file="DatabaseConnector.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Data.SqlClient;
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.Extensions.Logging;

    public class DatabaseConnector : IDisposable
    {
        private ILogger Logger;

        private string DBConnectionString;

        private SqlConnection Connection;

        public DatabaseConnector(ILogger logger, string dbConnectionString)
        {
            Logger = logger;
            DBConnectionString = dbConnectionString;
        }

        public async Task<bool> StoreTranscriptionAsync(
            Guid transcriptionId,
            string locale,
            string fileName,
            float approximateCost,
            SpeechTranscript speechTranscript)
        {
            if (speechTranscript == null)
            {
                throw new ArgumentNullException(nameof(speechTranscript));
            }

            try
            {
                Connection = new SqlConnection(DBConnectionString);
                Connection.Open();

                var query = "INSERT INTO dbo.Transcriptions (ID, Locale, Name, Source, Timestamp, Duration, NumberOfChannels, ApproximateCost)" +
                    " VALUES (@id, @locale, @name, @source, @timestamp, @duration, @numberOfChannels, @approximateCost)";

                using (var command = new SqlCommand(query, Connection))
                {
                    command.Parameters.AddWithValue("@id", transcriptionId);
                    command.Parameters.AddWithValue("@locale", locale);
                    command.Parameters.AddWithValue("@name", fileName);
                    command.Parameters.AddWithValue("@source", speechTranscript.Source);
                    command.Parameters.AddWithValue("@timestamp", speechTranscript.Timestamp);
                    command.Parameters.AddWithValue("@duration", speechTranscript.Duration);
                    command.Parameters.AddWithValue("@numberOfChannels", speechTranscript.CombinedRecognizedPhrases.Count());
                    command.Parameters.AddWithValue("@approximateCost", approximateCost);

                    var result = await command.ExecuteNonQueryAsync().ConfigureAwait(false);

                    if (result < 0)
                    {
                        Logger.LogInformation("Did not store json in Db, command did not update table");
                    }
                    else
                    {
                        var phrasesByChannel = speechTranscript.RecognizedPhrases.GroupBy(t => t.Channel);

                        foreach (var phrases in phrasesByChannel)
                        {
                            var channel = phrases.Key;
                            await StoreCombinedRecognizedPhrasesAsync(transcriptionId, channel, speechTranscript, phrases).ConfigureAwait(false);
                        }
                    }
                }

                Connection.Close();
            }
            catch (SqlException e)
            {
                Logger.LogInformation(e.ToString());
                return false;
            }

            return true;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            Logger.LogInformation("Disposing DBConnector");
            if (disposing)
            {
                Connection?.Dispose();
            }
        }

        private async Task StoreCombinedRecognizedPhrasesAsync(Guid transcriptionId, int channel, SpeechTranscript speechTranscript, IEnumerable<RecognizedPhrase> recognizedPhrases)
        {
            var combinedRecognizedPhraseID = Guid.NewGuid();

            var combinedPhrases = speechTranscript.CombinedRecognizedPhrases.Where(t => t.Channel == channel).FirstOrDefault();

            var query = "INSERT INTO dbo.CombinedRecognizedPhrases (ID, TranscriptionID, Channel, Lexical, Itn, MaskedItn, Display, SentimentPositive, SentimentNeutral, SentimentNegative)" +
                " VALUES (@id, @transcriptionID, @channel, @lexical, @itn, @maskedItn, @display, @sentimentPositive, @sentimentNeutral, @sentimentNegative)";

            using (var command = new SqlCommand(query, Connection))
            {
                command.Parameters.AddWithValue("@id", combinedRecognizedPhraseID);
                command.Parameters.AddWithValue("@transcriptionID", transcriptionId);
                command.Parameters.AddWithValue("@channel", channel);

                command.Parameters.AddWithValue("@lexical", combinedPhrases.Lexical ?? string.Empty);
                command.Parameters.AddWithValue("@itn", combinedPhrases.ITN ?? string.Empty);
                command.Parameters.AddWithValue("@maskedItn", combinedPhrases.MaskedITN ?? string.Empty);
                command.Parameters.AddWithValue("@display", combinedPhrases.Display ?? string.Empty);

                command.Parameters.AddWithValue("@sentimentPositive", combinedPhrases?.Sentiment?.Positive ?? 0f);
                command.Parameters.AddWithValue("@sentimentNeutral", combinedPhrases?.Sentiment?.Neutral ?? 0f);
                command.Parameters.AddWithValue("@sentimentNegative", combinedPhrases?.Sentiment?.Negative ?? 0f);

                var result = await command.ExecuteNonQueryAsync().ConfigureAwait(false);

                if (result < 0)
                {
                    Logger.LogInformation("Did not store combined phrase in Db, command did not update table");
                }
                else
                {
                    foreach (var phrase in recognizedPhrases)
                    {
                        await StoreRecognizedPhraseAsync(combinedRecognizedPhraseID, phrase).ConfigureAwait(false);
                    }
                }
            }
        }

        private async Task StoreRecognizedPhraseAsync(Guid combinedPhraseID, RecognizedPhrase recognizedPhrase)
        {
            var phraseId = Guid.NewGuid();
            var query = "INSERT INTO dbo.RecognizedPhrases (ID, CombinedRecognizedPhraseID, RecognitionStatus, Speaker, Channel, Offset, Duration)" +
                " VALUES (@id, @combinedRecognizedPhraseID, @recognitionStatus, @speaker, @channel, @offset, @duration)";

            using (var command = new SqlCommand(query, Connection))
            {
                command.Parameters.AddWithValue("@id", phraseId);
                command.Parameters.AddWithValue("@combinedRecognizedPhraseID", combinedPhraseID);
                command.Parameters.AddWithValue("@recognitionStatus", recognizedPhrase.RecognitionStatus);
                command.Parameters.AddWithValue("@speaker", recognizedPhrase.Speaker);
                command.Parameters.AddWithValue("@channel", recognizedPhrase.Channel);
                command.Parameters.AddWithValue("@offset", recognizedPhrase.Offset);
                command.Parameters.AddWithValue("@duration", recognizedPhrase.Duration);

                var result = await command.ExecuteNonQueryAsync().ConfigureAwait(false);

                if (result < 0)
                {
                    Logger.LogInformation("Did not store phrase in Db, command did not update table");
                }
                else
                {
                    foreach (var nBestResult in recognizedPhrase.NBest)
                    {
                        await StoreNBestAsync(phraseId, nBestResult).ConfigureAwait(false);
                    }
                }
            }
        }

        private async Task StoreNBestAsync(Guid recognizedPhraseID, NBest nBest)
        {
            var nBestID = Guid.NewGuid();
            var query = "INSERT INTO dbo.NBests (ID, RecognizedPhraseID, Confidence, Lexical, Itn, MaskedItn, Display, SentimentNegative, SentimentNeutral, SentimentPositive)" +
                " VALUES (@id, @recognizedPhraseID, @confidence, @lexical, @itn, @maskedItn, @display, @sentimentNegative, @sentimentNeutral, @sentimentPositive)";

            using (var command = new SqlCommand(query, Connection))
            {
                command.Parameters.AddWithValue("@id", nBestID);
                command.Parameters.AddWithValue("@recognizedPhraseID", recognizedPhraseID);
                command.Parameters.AddWithValue("@confidence", nBest.Confidence);
                command.Parameters.AddWithValue("@lexical", nBest.Lexical);
                command.Parameters.AddWithValue("@itn", nBest.ITN);
                command.Parameters.AddWithValue("@maskedItn", nBest.MaskedITN);
                command.Parameters.AddWithValue("@display", nBest.Display);

                command.Parameters.AddWithValue("@sentimentNegative", nBest?.Sentiment?.Negative ?? 0f);
                command.Parameters.AddWithValue("@sentimentNeutral", nBest?.Sentiment?.Neutral ?? 0f);
                command.Parameters.AddWithValue("@sentimentPositive", nBest?.Sentiment?.Positive ?? 0f);

                var result = await command.ExecuteNonQueryAsync().ConfigureAwait(false);

                if (result < 0)
                {
                    Logger.LogInformation("Did not store nbest in Db, command did not update table");
                }
                else
                {
                    if (nBest.Words == null)
                    {
                        return;
                    }

                    foreach (var word in nBest.Words)
                    {
                        await StoreWordsAsync(nBestID, word).ConfigureAwait(false);
                    }
                }
            }
        }

        private async Task StoreWordsAsync(Guid nBestId, Words word)
        {
            var wordID = Guid.NewGuid();
            var query = "INSERT INTO dbo.Words (ID, NBestID, Word, Offset, Duration, Confidence)" +
                " VALUES (@id, @nBestID, @word, @offset, @duration, @confidence)";

            using (var command = new SqlCommand(query, Connection))
            {
                command.Parameters.AddWithValue("@id", wordID);
                command.Parameters.AddWithValue("@nBestID", nBestId);
                command.Parameters.AddWithValue("@word", word.Word);
                command.Parameters.AddWithValue("@offset", word.Offset);
                command.Parameters.AddWithValue("@duration", word.Duration);
                command.Parameters.AddWithValue("@confidence", word.Confidence);

                var result = await command.ExecuteNonQueryAsync().ConfigureAwait(false);
                if (result < 0)
                {
                    Logger.LogInformation("Did not Store word result in Db, command did not update table");
                }
            }
        }
    }
}