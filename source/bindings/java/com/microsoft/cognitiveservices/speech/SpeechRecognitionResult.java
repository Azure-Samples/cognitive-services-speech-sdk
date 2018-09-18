//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.RecognitionResult;

/**
 * Defines result of speech recognition.
 */
public class SpeechRecognitionResult extends RecognitionResult {
    protected SpeechRecognitionResult(com.microsoft.cognitiveservices.speech.internal.RecognitionResult result) {
        super(result);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        super.close();
    }

    /**
     * Returns a String that represents the speech recognition result.
     * @return A String that represents the speech recognition result.
     */
    @Override
    public String toString() {
        return "ResultId:" + this.getResultId()+
                " Status:" + this.getReason() +
                " Recognized text:<" + this.getText() + ">.";
    }
}
