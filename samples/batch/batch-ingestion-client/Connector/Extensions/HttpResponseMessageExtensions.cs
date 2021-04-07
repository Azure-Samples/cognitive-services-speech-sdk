// <copyright file="HttpResponseMessageExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Net.Http;
    using System.Threading.Tasks;

    public static class HttpResponseMessageExtensions
    {
        public static async Task EnsureSuccessStatusCodeAsync(this HttpResponseMessage responseMessage)
        {
            responseMessage = responseMessage ?? throw new ArgumentNullException(nameof(responseMessage));

            if (!responseMessage.IsSuccessStatusCode)
            {
                if (responseMessage.Content != null)
                {
                    var body = await responseMessage.Content.ReadAsStringAsync().ConfigureAwait(false);
                    throw new HttpStatusCodeException(responseMessage.StatusCode, $"{responseMessage.ReasonPhrase}: {body}");
                }

                throw new HttpStatusCodeException(responseMessage.StatusCode, responseMessage.ReasonPhrase);
            }
        }
    }
}
