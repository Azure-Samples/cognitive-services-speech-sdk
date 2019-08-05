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
        /// Stream ContainerFormat definition for FLAC. Added in version 1.7.0. 
        /// </summary>
        FLAC = 0x103,

        /// <summary>
        /// Stream ContainerFormat definition for ALAW. Added in version 1.7.0. 
        /// </summary>
        ALAW = 0x104,

        /// <summary>
        /// Stream ContainerFormat definition for MULAW. Added in version 1.7.0. 
        /// </summary>
        MULAW = 0x105,

        /// <summary>
        /// Stream ContainerFormat definition for AMRNB. Currently not supported. 
        /// </summary>
        AMRNB = 0x106,

        /// <summary>
        /// Stream ContainerFormat definition for AMRWB. Currently not supported. 
        /// </summary>
        AMRWB = 0x107,
    }
}
