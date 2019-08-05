//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.audio;

/**
 * Supported audio input container formats.
 * Added in version 1.4.0.
 */
public enum AudioStreamContainerFormat {
    /**
     * Stream ContainerFormat definition for OGG OPUS.
    */
    OGG_OPUS(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.OGG_OPUS),

    /**
     * Stream ContainerFormat definition for MP3.
    */
    MP3(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.MP3),

    /**
     * Stream ContainerFormat definition for FLAC. Added in version 1.7.0.
    */
    FLAC(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.FLAC),

    /**
     * Stream ContainerFormat definition for ALAW. Added in version 1.7.0.
    */
    ALAW(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.ALAW),

    /**
     * Stream ContainerFormat definition for MULAW. Added in version 1.7.0.
    */
    MULAW(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.MULAW),

    /**
     * Stream ContainerFormat definition for AMRNB. Currently not supported.
    */
    AMRNB(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.AMRNB),

    /**
     * Stream ContainerFormat definition for AMRWB. Currently not supported.
    */
    AMRWB(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.AMRWB);

    private AudioStreamContainerFormat(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat id) {
        this.id = id;
    }

    /**
     * Returns the internal value Audio Stream Container Format
     *
     * @return the Audio Stream Container Format
    */
    public com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat id;
}
