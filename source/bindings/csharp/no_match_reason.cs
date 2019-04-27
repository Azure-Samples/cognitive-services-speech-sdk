//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the possible reasons a recognition result might not be recognized.
    /// </summary>
    public enum NoMatchReason
    {
        /// <summary>
        /// Indicates that speech was detected, but not recognized.
        /// </summary>
        NotRecognized = 1,

        /// <summary>
        /// Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
        /// </summary>
        InitialSilenceTimeout = 2,

        /// <summary>
        /// Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
        /// </summary>
        InitialBabbleTimeout = 3,

        /// <summary>
        /// Indicates that the spotted keyword has been rejected by the keyword verification service.
        /// Added in version 1.5.0
        /// </summary>
        KeywordNotRecognized = 4
    }
}
