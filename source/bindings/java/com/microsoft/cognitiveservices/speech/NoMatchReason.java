//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines the possible reasons a recognition result might not be recognized.
 */
public enum NoMatchReason {

    /**
     * Indicates that speech was detected, but not recognized.
     */
    NotRecognized(1),

    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     */
    InitialSilenceTimeout(2),

    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     */
    InitialBabbleTimeout(3),

    /**
     * Indicates that the spotted keyword has been rejected by the keyword verification service.
     * Added in version 1.5.0.
     */
    KeywordNotRecognized(4);

    private NoMatchReason(int id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @return the speech property id
     */
    public int getValue() { return this.id; }

    private final int id;
}
