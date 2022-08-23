// <copyright file="TransientFailureException.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    /// <summary>
    /// Exception that is being thrown when azure functions should trigger a retry (up to a max. retry limit)
    /// after a certain number of minutes (with exponential backoff).
    /// </summary>
    public sealed class TransientFailureException : Exception
    {
        public TransientFailureException()
        {
        }

        public TransientFailureException(string message)
            : base(message)
        {
        }

        public TransientFailureException(string message, Exception innerException)
            : base(message, innerException)
        {
        }
    }
}
