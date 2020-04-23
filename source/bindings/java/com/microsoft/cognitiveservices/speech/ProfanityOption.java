//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Define profanity option for response result.
 * Added in version 1.5.0.
 */
public enum ProfanityOption {
    /**
     * Replaces profanity with asterisks.
     */
    Masked (0),

    /**
     * Removes all profanity from the result.
     */
    Removed (1),

    /**
     * Includes the profanity in the result.
     */
    Raw (2);

    private ProfanityOption(int profanity) {
        this.profanity = profanity;
    }

    /**
     * Returns the internal value of profanity option.
     *
     * @return the internal profanity value.
     */
    public int getValue() { return this.profanity; }

    private final int profanity;
}
