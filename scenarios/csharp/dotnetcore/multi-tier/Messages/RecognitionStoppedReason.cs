//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace SpeechSDKSamples.Remote.Core
{
    /// <summary>
    /// Reason the recognition stopped.
    /// </summary>
    public enum RecognitionStoppedReason
    {
        /// <summary>
        /// An error occurred.
        /// </summary>
        Error,

        /// <summary>
        /// The end of the input stream was reached.
        /// </summary>
        EndOfStream,

        /// <summary>
        /// The recognition was stopped by a request.
        /// </summary>
        StopRequested,
        Unknown
    }
}
