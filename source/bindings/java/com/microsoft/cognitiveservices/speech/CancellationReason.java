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
    Error(1),

    /**
     * Indicates that the end of the audio stream was reached.
     */
    EndOfStream(2);

    private CancellationReason(int id) {
        this.id = id;
    }

    /**
     * Returns the value of cancellation reason
     *
     * @return the cancellation id value
     */
    public int getValue() { return this.id; }

    private final int id;
}
