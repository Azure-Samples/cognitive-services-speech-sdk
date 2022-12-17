//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Threading.Tasks;
    using System.Web;

    public partial class BatchClient
    {
        public Task<PaginatedEndpoints> GetEndpointsAsync()
        {
            var path = $"{this.speechToTextBasePath}endpoints";
            return this.GetAsync<PaginatedEndpoints>(path);
        }

        public Task<PaginatedEndpoints> GetEndpointsAsync(Uri location)
        {
            return this.GetAsync<PaginatedEndpoints>(location.PathAndQuery);
        }

        public Task DeleteEndpointAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }

        public Task<Endpoint> CreateEndpointAsync(Endpoint endpoint)
        {
            if (endpoint == null)
            {
                throw new ArgumentNullException(nameof(endpoint));
            }

            var path = $"{this.speechToTextBasePath}endpoints/";

            return this.PostAsJsonAsync<Endpoint, Endpoint>(path, endpoint);
        }

        public Task<Endpoint> GetEndpointAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<Endpoint>(location.PathAndQuery);
        }

        public Task<Endpoint> UpdateEndpointAsync(EndpointUpdate endpointUpdate)
        {
            if (endpointUpdate == null)
            {
                throw new ArgumentNullException(nameof(endpointUpdate));
            }

            var path = $"{this.speechToTextBasePath}endpoints/";

            return this.PatchAsJsonAsync<EndpointUpdate, Endpoint>(path, endpointUpdate);
        }

        public Task<PaginatedFiles> GetEndpointLogsAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<PaginatedFiles>(location.PathAndQuery);
        }

        public Task<File> GetEndpointLogAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<File>(location.PathAndQuery);
        }

        public Task DeleteEndpointLogAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }

        public Task DeleteEndpointLogsAsync(Uri location, DateTime? endDate)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            if (endDate.HasValue)
            {
                var uriBuilder = new UriBuilder(location);
                var query = HttpUtility.ParseQueryString(uriBuilder.Query);
                query["endDate"] = endDate.Value.ToString("yyyy-MM-dd");
                uriBuilder.Query = query.ToString();

                location = uriBuilder.Uri;
            }


            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }
    }
}
