//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using SpeechSDKSamples.Remote.Core;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// Event arguments raised when speech recognition stops, either normally or due to an error.
    /// </summary>
    /// <remarks>
    /// This class provides information about why the recognition stopped, including:
    /// - Normal completion (end of stream)
    /// - User-requested stop
    /// - Error conditions (with optional exception details)
    /// </remarks>
    public sealed class StoppedEventArgs : EventArgs
    {
        /// <summary>
        /// Gets the exception that caused the recognition to stop, if any.
        /// </summary>
        /// <remarks>
        /// This property will be null unless <see cref="Reason"/> is <see cref="RecognitionStoppedReason.Error"/>.
        /// When an error occurs, this exception provides additional details about what went wrong.
        /// </remarks>
        public Exception Exception { get; }

        /// <summary>
        /// Gets the reason why the recognition stopped.
        /// </summary>
        /// <remarks>
        /// This indicates whether the stop was expected (like end of stream),
        /// requested by the user, or caused by an error condition.
        /// </remarks>
        public RecognitionStoppedReason Reason { get; }

        /// <summary>
        /// Initializes a new instance of the StoppedEventArgs class with the specified stop reason.
        /// </summary>
        /// <param name="reason">The reason why recognition stopped.</param>
        public StoppedEventArgs(RecognitionStoppedReason reason)
        {
            Reason = reason;
        }

        /// <summary>
        /// Initializes a new instance of the StoppedEventArgs class with the specified stop reason and exception.
        /// </summary>
        /// <param name="reason">The reason why recognition stopped.</param>
        /// <param name="exception">The exception that caused the stop, if any.</param>
        /// <remarks>
        /// If <paramref name="reason"/> is <see cref="RecognitionStoppedReason.Error"/>,
        /// the <paramref name="exception"/> parameter should provide details about the error.
        /// </remarks>
        public StoppedEventArgs(RecognitionStoppedReason reason, Exception exception)
        {
            if (reason == RecognitionStoppedReason.Error && exception == null)
                throw new ArgumentNullException(nameof(exception), "Exception must be provided when reason is Error");

            if (reason != RecognitionStoppedReason.Error && exception != null)
                throw new ArgumentException("Exception should only be provided when reason is Error", nameof(exception));

            Reason = reason;
            Exception = exception;
        }
    }
}
