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
    NotRecognized(com.microsoft.cognitiveservices.speech.internal.NoMatchReason.NotRecognized),

    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     */
    InitialSilenceTimeout(com.microsoft.cognitiveservices.speech.internal.NoMatchReason.InitialSilenceTimeout),

    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     */
    InitialBabbleTimeout(com.microsoft.cognitiveservices.speech.internal.NoMatchReason.InitialBabbleTimeout);
    
    private NoMatchReason(com.microsoft.cognitiveservices.speech.internal.NoMatchReason id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @return the speech property id
     */
    public com.microsoft.cognitiveservices.speech.internal.NoMatchReason getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.NoMatchReason id;
}
