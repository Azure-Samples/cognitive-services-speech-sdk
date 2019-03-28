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
     * Stream ContainerFormat definition for FLAC. Not supported yet.
     */
    FLAC(com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat.FLAC);

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
