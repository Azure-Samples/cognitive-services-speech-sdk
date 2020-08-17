//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines the pronunciation evaluation granularity; default value is Phoneme.
 * Added in version 1.14.0
 */
public enum PronunciationAssessmentGranularity
{
    /**
     * Shows the score on the full text, word and phoneme level
     */
    Phoneme (1),

    /**
     * Shows the score on the full text and word level
     */
    Word (2),

    /**
     * Shows the score on the full text level only
     */
    FullText (3);

    private PronunciationAssessmentGranularity(int id) {
        this.id = id;
    }

    /**
     * Returns the value of pronunciation assessment granularity
     *
     * @return the granularity id value
     */
    public int getValue() { return this.id; }

    private final int id;
}
