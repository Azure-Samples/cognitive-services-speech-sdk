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
        public Task<PaginatedModels> GetBaseModelsAsync()
        {
            var path = $"{this.speechToTextBasePath}models/base";
            return this.GetAsync<PaginatedModels>(path);
        }

        public Task<PaginatedModels> GetCustomModelsAsync()
        {
            var path = $"{this.speechToTextBasePath}models";
            return this.GetAsync<PaginatedModels>(path);
        }

        public Task<PaginatedModels> GetModelsAsync(Uri location)
        {
            return this.GetAsync<PaginatedModels>(location.PathAndQuery);
        }

        public Task DeleteModelAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }

        public Task<Model> CreateModelAsync(Model Model)
        {
            if (Model == null)
            {
                throw new ArgumentNullException(nameof(Model));
            }

            var path = $"{this.speechToTextBasePath}models/";

            return this.PostAsJsonAsync<Model, Model>(path, Model);
        }

        public Task<Model> GetBaseModelAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<Model>(location.PathAndQuery);
        }

        public Task<Model> GetCustomModelAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<Model>(location.PathAndQuery);
        }

        public Task<Model> UpdateModelAsync(ModelUpdate ModelUpdate)
        {
            if (ModelUpdate == null)
            {
                throw new ArgumentNullException(nameof(ModelUpdate));
            }

            var path = $"{this.speechToTextBasePath}models/";

            return this.PatchAsJsonAsync<ModelUpdate, Model>(path, ModelUpdate);
        }

        public Task<Model> CopyModelAsync(Uri copyUri, ModelCopy modelCopy)
        {
            if (copyUri == null)
            {
                throw new ArgumentNullException(nameof(copyUri));
            }

            return this.PostAsJsonAsync<ModelCopy, Model>(copyUri.PathAndQuery, modelCopy);
        }
    }
}
