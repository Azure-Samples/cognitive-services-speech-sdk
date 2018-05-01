//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines status of translation synthesis message.
    /// </summary>
    public enum TranslationSynthesisStatus
    {
        /// <summary>
        /// The response contains valid audio data.
        /// </summary>
        Success,

        /// <summary>
        /// Indicates the end of audio data. No valid audio data is included in the message.
        /// </summary>
        SynthesisEnd,

        /// <summary>
        /// Indicates an error occurred during synthesis data processing.
        /// </summary>
        Error
    }
}
