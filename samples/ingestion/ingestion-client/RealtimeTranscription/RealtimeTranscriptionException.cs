// <copyright file="RealtimeTranscriptionException.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace RealtimeTranscription
{
    using System;
    using Microsoft.CognitiveServices.Speech;

    public sealed class RealtimeTranscriptionException : Exception
    {
        public RealtimeTranscriptionException(CancellationErrorCode cancellationErrorCode, string message)
            : base(message)
        {
            this.CancellationErrorCode = cancellationErrorCode;
        }

        public RealtimeTranscriptionException(string message)
            : base(message)
        {
        }

        public RealtimeTranscriptionException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        public RealtimeTranscriptionException()
        {
        }

        public CancellationErrorCode CancellationErrorCode { get; } = CancellationErrorCode.NoError;
    }
}
