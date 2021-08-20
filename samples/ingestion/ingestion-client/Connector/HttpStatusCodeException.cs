// <copyright file="HttpStatusCodeException.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Net;

    public sealed class HttpStatusCodeException : Exception
    {
        public HttpStatusCodeException()
        {
        }

        public HttpStatusCodeException(string message)
            : base(message)
        {
        }

        public HttpStatusCodeException(HttpStatusCode httpStatusCode, string message)
            : base(message)
        {
            this.HttpStatusCode = httpStatusCode;
        }

        public HttpStatusCodeException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        public HttpStatusCode? HttpStatusCode { get; private set; }
    }
}
