// <copyright file="HttpStatusCodeExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Net;

    public static class HttpStatusCodeExtensions
    {
        public static bool IsRetryableStatus(this HttpStatusCode statusCode)
        {
            if (statusCode == HttpStatusCode.TooManyRequests ||
                statusCode == HttpStatusCode.GatewayTimeout ||
                statusCode == HttpStatusCode.RequestTimeout ||
                statusCode == HttpStatusCode.BadGateway ||
                statusCode == HttpStatusCode.InternalServerError)
            {
                return true;
            }

            return false;
        }
    }
}
