//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Threading.Tasks;
    using Newtonsoft.Json;

    public partial class BatchClient
    {
        public Task<PaginatedTranscriptions> GetTranscriptionsAsync()
        {
            var path = $"{this.speechToTextBasePath}transcriptions?api-version={this.apiVersion}";
            return this.GetAsync<PaginatedTranscriptions>(path);
        }

        public Task<PaginatedTranscriptions> GetTranscriptionsAsync(Uri location)
        {
            return this.GetAsync<PaginatedTranscriptions>(location.PathAndQuery);
        }

        public Task<PaginatedFiles> GetTranscriptionFilesAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<PaginatedFiles>(location.PathAndQuery);
        }

        public Task<Transcription> GetTranscriptionAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<Transcription>(location.PathAndQuery);
        }

        public async Task<RecognitionResults> GetTranscriptionResultAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            var response = await transientFailureRetryingPolicy
                .ExecuteAsync(async () => await this.client.GetAsync(location).ConfigureAwait(false))
                .ConfigureAwait(false);

            if (response.IsSuccessStatusCode)
            {
                var json = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
                return JsonConvert.DeserializeObject<RecognitionResults>(json, SpeechJsonContractResolver.ReaderSettings);
            }

            throw await CreateExceptionAsync(response);
        }

        public Task<Transcription> CreateTranscriptionAsync(Transcription transcription)
        {
            if (transcription == null)
            {
                throw new ArgumentNullException(nameof(transcription));
            }

            var path = $"{this.speechToTextBasePath}transcriptions:submit?api-version={this.apiVersion}";

            return this.PostAsJsonAsync<Transcription, Transcription>(path, transcription);
        }

        public Task DeleteTranscriptionAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }
    }
}
