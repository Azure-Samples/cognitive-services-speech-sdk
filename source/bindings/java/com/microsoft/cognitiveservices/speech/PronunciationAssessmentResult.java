//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents the result of pronunciation assessment.
 * Added in version 1.14.0
 */
public final class PronunciationAssessmentResult
{
     /**
     * Creates an instance of the PronunciationAssessmentResult from a speech recognition result
     * @param speechRecognitionResult Specifies the speech recognition result
     * @return The PronunciationAssessmentResult being created.
     */
    public static PronunciationAssessmentResult fromResult(SpeechRecognitionResult speechRecognitionResult) {
        Contracts.throwIfNull(speechRecognitionResult, "speechRecognitionResult cannot be null");
        if (speechRecognitionResult.getProperties().getProperty("AccuracyScore").isEmpty()) {
            return null;
        }
        return new PronunciationAssessmentResult(speechRecognitionResult.getProperties());
    }

     /**
     * Gets the pronunciation accuracy of the given speech, which indicates
     * how closely the phonemes match a native speaker's pronunciation
     * @return The accuracy score
     */
    public Double getAccuracyScore() {
        return accuracyScore;
    }

    /**
     * Gets the overall score indicating the pronunciation quality of the given speech.
     * This is calculated from AccuracyScore, FluencyScore and CompletenessScore with weight.
     * @return The pronunciation score.
     */
    public Double getPronunciationScore() {
        return pronunciationScore;
    }

    /**
     * Gets the score indicating the completeness of the given speech by calculating the ratio of pronounced words towards entire input.
     * @return The completeness score.
     */
    public Double getCompletenessScore() {
        return completenessScore;
    }

    /**
     * Gets the score indicating the fluency of the given speech.
     * @return The fluency score.
     */
    public Double getFluencyScore() {
        return fluencyScore;
    }

    private PronunciationAssessmentResult(PropertyCollection properties) {
        accuracyScore = Double.parseDouble(properties.getProperty("AccuracyScore"));
        pronunciationScore = Double.parseDouble(properties.getProperty("PronScore"));
        completenessScore = Double.parseDouble(properties.getProperty("CompletenessScore"));
        fluencyScore = Double.parseDouble(properties.getProperty("FluencyScore"));
    }

    private double accuracyScore;
    private double pronunciationScore;
    private double completenessScore;
    private double fluencyScore;
}
