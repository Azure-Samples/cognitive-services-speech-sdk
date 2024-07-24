//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Net;
    using System.Runtime.Serialization;

    public sealed class FailedHttpClientRequestException : Exception
    {
        public FailedHttpClientRequestException()
        {
            this.StatusCode = HttpStatusCode.Unused;
        }

        public FailedHttpClientRequestException(string message)
            : base(message)
        {
            this.StatusCode = HttpStatusCode.Unused;
        }

        public FailedHttpClientRequestException(string message, Exception exception)
            : base(message, exception)
        {
            this.StatusCode = HttpStatusCode.Unused;
        }

        public FailedHttpClientRequestException(HttpStatusCode status, string reasonPhrase)
            : base(reasonPhrase)
        {
            this.StatusCode = status;
        }

        public HttpStatusCode StatusCode { get; private set; }

        public string ReasonPhrase => this.Message;
    }
}
