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

            var path = $"{this.speechToTextBasePath}webhooks/";

            return this.PostAsJsonAsync<WebHook, WebHook>(path, webHook);
        }

        public Task<WebHook> GetWebHookAsync(Uri location)
        {
            if (location == null)
            {
                throw new ArgumentNullException(nameof(location));
            }

            return this.GetAsync<WebHook>(location.PathAndQuery);
        }

        public Task<WebHook> UpdateWebHookAsync(WebHookUpdate webHookUpdate)
        {
            if (webHookUpdate == null)
            {
                throw new ArgumentNullException(nameof(webHookUpdate));
            }

            var path = $"{this.speechToTextBasePath}webhooks/";

            return this.PatchAsJsonAsync<WebHookUpdate, WebHook>(path, webHookUpdate);
        }
    }
}
