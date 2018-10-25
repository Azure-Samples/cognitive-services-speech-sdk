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
        /// Indicates an authentication error. The errorDetails property contains detailed error response.
        /// </summary>
        AuthenticationFailure = 1,

        /// <summary>
        /// Indicates that one or more recognition parameters are invalid.
        /// </summary>
        BadRequestParameters = 2,

        /// <summary>
        /// Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
        /// </summary>
        TooManyRequests = 3,

        /// <summary>
        /// Indicates an connection error. The errorDetails property contains detailed error response.
        /// </summary>
        ConnectionFailure = 4,

        /// <summary>
        /// Indicates a time-out error when waiting for response from service.
        /// </summary>
        ServiceTimeout = 5,

        /// <summary>
        /// Indicates that an error is returned by the service.
        /// </summary>
        ServiceError = 6,

        /// <summary>
        /// Indicates an unexpected runtime error. The errorDetails property contains detailed error response.
        /// </summary>
        RuntimeError = 7
    };
}

