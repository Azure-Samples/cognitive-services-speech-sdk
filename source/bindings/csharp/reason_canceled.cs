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
}
