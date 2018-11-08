//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines the possible reasons a recognition result might be canceled.
 */
public enum CancellationReason
{
    /**
     * Indicates that an error occurred during speech recognition.
     */
    Error(com.microsoft.cognitiveservices.speech.internal.CancellationReason.Error),

    /**
     * Indicates that the end of the audio stream was reached.
     */
    EndOfStream(com.microsoft.cognitiveservices.speech.internal.CancellationReason.EndOfStream);

    private CancellationReason(com.microsoft.cognitiveservices.speech.internal.CancellationReason id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @return the speech property id
     */
    public com.microsoft.cognitiveservices.speech.internal.CancellationReason getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.CancellationReason id;
}
