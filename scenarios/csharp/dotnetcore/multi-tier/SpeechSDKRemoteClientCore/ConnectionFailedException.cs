//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.Serialization;

namespace SpeechSDKSamples.Remote.Client.Core
{
    /// <summary>
    /// The exception that is thrown when a connection to the speech service fails.
    /// This can occur during initial connection attempts or while maintaining an existing connection.
    /// </summary>
    /// <remarks>
    /// Common scenarios where this exception might be thrown include:
    /// - Network connectivity issues
    /// - Invalid endpoint configuration
    /// - Service authentication failures
    /// - Connection timeouts
    /// </remarks>
    [Serializable]
    public sealed class ConnectionFailedException : Exception
    {
        /// <summary>
        /// Initializes a new instance of the ConnectionFailedException class.
        /// </summary>
        public ConnectionFailedException()
            : base("Connection to the speech service failed.")
        {
        }

        /// <summary>
        /// Initializes a new instance of the ConnectionFailedException class with a specified error message.
        /// </summary>
        /// <param name="message">The message that describes the error.</param>
        public ConnectionFailedException(string message)
            : base(message)
        {
        }

        /// <summary>
        /// Initializes a new instance of the ConnectionFailedException class with a specified error message
        /// and a reference to the inner exception that is the cause of this exception.
        /// </summary>
        /// <param name="message">The message that describes the error.</param>
        /// <param name="innerException">The exception that is the cause of the current exception.</param>
        public ConnectionFailedException(string message, Exception innerException)
            : base(message, innerException)
        {
        }

        /// <summary>
        /// Initializes a new instance of the ConnectionFailedException class with serialized data.
        /// </summary>
        /// <param name="info">The SerializationInfo that holds the serialized object data.</param>
        /// <param name="context">The StreamingContext that contains contextual information about the source or destination.</param>
        /// <exception cref="ArgumentNullException">The info parameter is null.</exception>
        /// <exception cref="SerializationException">The class name is null or the format of serialized data is invalid.</exception>
        private ConnectionFailedException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
