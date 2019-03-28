//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Supported audio input container formats.
    /// Added in version 1.4.0
    /// </summary>
    public enum AudioStreamContainerFormat
    {
        /// <summary>
        /// Stream ContainerFormat definition for OGG OPUS.
        /// </summary>
        OGG_OPUS = 0x101,

        /// <summary>
        /// Stream ContainerFormat definition for MP3.
        /// </summary>
        MP3 = 0x102,

        /// <summary>
        /// Stream ContainerFormat definition for FLAC. Not supported yet. 
        /// </summary>
        FLAC = 0x103,
    }
}
