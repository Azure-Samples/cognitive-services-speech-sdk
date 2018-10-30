//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the possible reasons a recognition result might be canceled.
    /// </summary>
    public enum CancellationReason
    {
        /// <summary>
        /// Indicates that an error occurred during speech recognition. Use ErrorDetails property contains detailed error response.
        /// </summary>
        Error = 1,

        /// <summary>
        /// Indicates that the end of the audio stream was reached.
        /// </summary>
        EndOfStream = 2
    }

    /// <summary>
    /// Defines error code in case that CancellationReason is Error. 
    /// Added in version 1.1.0.
    /// </summary>

    public enum CancellationErrorCode
    {
        /// <summary>
        /// No error.
        /// </summary>
        NoError = 0,

        /// <summary>
        /// Indicates an authentication error.
        /// </summary>
        AuthenticationFailure = 1,

        /// <summary>
        /// Indicates that one or more recognition parameters are invalid or the audio format is not supported.
        /// </summary>
        BadRequest = 2,

        /// <summary>
        /// Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
        /// </summary>
        TooManyRequests = 3,

        /// <summary>
        /// Indicates that the free subscription used by the request ran out of quota.
        /// </summary>
        Forbidden = 4,

        /// <summary>
        /// Indicates a connection error.
        /// </summary>
        ConnectionFailure = 5,

        /// <summary>
        /// Indicates a time-out error when waiting for response from service.
        /// </summary>
        ServiceTimeout = 6,

        /// <summary>
        /// Indicates that an error is returned by the service.
        /// </summary>
        ServiceError = 7,

        /// <summary>
        /// Indicates that the service is currently unavailable.
        /// </summary>
        ServiceUnavailable = 8,

        /// <summary>
        /// Indicates an unexpected runtime error.
        /// </summary>
        RuntimeError = 9
    }
}

