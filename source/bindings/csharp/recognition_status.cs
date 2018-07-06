//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines speech recognition status.
    /// </summary>
    public enum RecognitionStatus
    {
        /// <summary>
        /// Indicates the result is a phrase that has been successfully recognized.
        /// </summary>
        Recognized,

        /// <summary>
        /// Indicates the result is a hypothesis text that has been recognized.
        /// </summary>
        IntermediateResult,

        /// <summary>
        /// Indicates that speech was detected in the audio stream, but no words from the target language were matched.
        /// Possible reasons could be wrong setting of the target language or wrong format of audio stream.
        /// </summary>
        NoMatch,

        /// <summary>
        /// Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
        /// </summary>
        InitialSilenceTimeout,

        /// <summary>
        /// Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
        /// </summary>
        InitialBabbleTimeout,

        /// <summary>
        /// Indicates that an error occurred during recognition. The RecognitionFailureReason property contains detailed error reasons.
        /// </summary>
        Canceled
    }
}
