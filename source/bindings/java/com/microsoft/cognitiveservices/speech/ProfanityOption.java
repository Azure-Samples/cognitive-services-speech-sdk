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
    Masked (com.microsoft.cognitiveservices.speech.internal.ProfanityOption.Masked),

    /**
     * Removes all profanity from the result.
     */
    Removed (com.microsoft.cognitiveservices.speech.internal.ProfanityOption.Removed),

    /**
     * Includes the profanity in the result.
     */
    Raw (com.microsoft.cognitiveservices.speech.internal.ProfanityOption.Raw);

    private ProfanityOption(com.microsoft.cognitiveservices.speech.internal.ProfanityOption profanity) {
        this.profanity = profanity;
    }

    /**
     * Returns the internal value of profanity option.
     *
     * @return the internal profanity value.
     */
    public com.microsoft.cognitiveservices.speech.internal.ProfanityOption getValue() { return this.profanity; }

    private final com.microsoft.cognitiveservices.speech.internal.ProfanityOption profanity;
}
