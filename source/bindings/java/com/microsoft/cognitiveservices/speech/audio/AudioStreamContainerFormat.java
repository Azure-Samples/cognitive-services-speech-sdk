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
    OGG_OPUS(0x101),

    /**
     * Stream ContainerFormat definition for MP3.
    */
    MP3(0x102),

    /**
     * Stream ContainerFormat definition for FLAC. Added in version 1.7.0.
    */
    FLAC(0x103),

    /**
     * Stream ContainerFormat definition for ALAW. Added in version 1.7.0.
    */
    ALAW(0x104),

    /**
     * Stream ContainerFormat definition for MULAW. Added in version 1.7.0.
    */
    MULAW(0x105),

    /**
     * Stream ContainerFormat definition for AMRNB. Currently not supported.
    */
    AMRNB(0x106),

    /**
     * Stream ContainerFormat definition for AMRWB. Currently not supported.
    */
    AMRWB(0x107);

    private AudioStreamContainerFormat(int id) {
        this.id = id;
    }

    /**
     * Returns the internal value Audio Stream Container Format
     *
     * @return the Audio Stream Container Format
    */
    public int getValue() { return this.id; }

    private final int id;
}
