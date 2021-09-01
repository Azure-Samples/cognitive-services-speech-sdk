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
        public Task<PaginatedWebHooks> GetWebHooksAsync()
        {
            var path = $"{this.speechToTextBasePath}webhooks";
            return this.GetAsync<PaginatedWebHooks>(path);
        }

        public Task<PaginatedWebHooks> GetWebHooksAsync(Uri location)
        {
            return this.GetAsync<PaginatedWebHooks>(location.PathAndQuery);
        }

        public Task DeleteWebHookAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return transientFailureRetryingPolicy
                .ExecuteAsync(() => this.client.DeleteAsync(location.PathAndQuery));
        }

        public Task<WebHook> CreateWebHookAsync(WebHook webHook)
        {
            if (webHook == null)
            {
                throw new ArgumentNullException(nameof(webHook));
            }

            var path = $"{this.speechToTextBasePath}webHooks/";

            return this.PostAsJsonAsync<WebHook, WebHook>(path, webHook);
        }
    }
}
