//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Threading.Tasks;

    public partial class BatchClient
    {
        public Task<PaginatedDatasets> GetDatasetsAsync()
        {
            var path = $"{this.speechToTextBasePath}datasets";
            return this.GetAsync<PaginatedDatasets>(path);
        }

        public Task<PaginatedDatasets> GetDatasetsAsync(Uri location)
        {
            return this.GetAsync<PaginatedDatasets>(location.PathAndQuery);
        }

        public Task DeleteDatasetAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }

        public Task<Dataset> CreateDatasetAsync(Dataset webHook)
        {
            if (webHook == null)
            {
                throw new ArgumentNullException(nameof(webHook));
            }

            var path = $"{this.speechToTextBasePath}datasets/";

            return this.PostAsJsonAsync<Dataset, Dataset>(path, webHook);
        }

        public Task<Dataset> GetDatasetAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<Dataset>(location.PathAndQuery);
        }

        public Task<Dataset> UpdateDatasetAsync(DatasetUpdate webHookUpdate)
        {
            if (webHookUpdate == null)
            {
                throw new ArgumentNullException(nameof(webHookUpdate));
            }

            var path = $"{this.speechToTextBasePath}datasets/";

            return this.PatchAsJsonAsync<DatasetUpdate, Dataset>(path, webHookUpdate);
        }

        public Task<PaginatedFiles> GetDatasetFilesAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<PaginatedFiles>(location.PathAndQuery);
        }

        public Task<File> GetDatasetFileAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<File>(location.PathAndQuery);
        }

    }
}
