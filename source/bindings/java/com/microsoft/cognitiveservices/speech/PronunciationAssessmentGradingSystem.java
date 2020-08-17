//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines the point system for pronunciation score calibration; default value is FivePoint.
 * Added in version 1.14.0
 */
public enum PronunciationAssessmentGradingSystem
{
    /**
     * Five point calibration
     */
    FivePoint(1),

    /**
     * Hundred mark
     */
    HundredMark(2);

    private PronunciationAssessmentGradingSystem(int id) {
        this.id = id;
    }

    /**
     * Returns the value of pronunciation assessment grading system
     *
     * @return the grading system id value
     */
    public int getValue() { return this.id; }

    private final int id;
}
