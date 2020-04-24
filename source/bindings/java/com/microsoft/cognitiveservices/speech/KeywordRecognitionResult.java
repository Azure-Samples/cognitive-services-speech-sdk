//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.RecognitionResult;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Defines result of keyword recognition.
 */
public final class KeywordRecognitionResult extends com.microsoft.cognitiveservices.speech.RecognitionResult {

    KeywordRecognitionResult(long result) {
        super(result);
        Contracts.throwIfNull(resultHandle, "resultHandle");
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        super.close();
    }

    /**
     * Returns a String that represents the keyword recognition result.
     * @return A String that represents the keyword recognition result.
     */
    @Override
    public String toString() {
        return "ResultId:" + getResultId() +
                " Reason:" + getReason() +
                "> Recognized text:<" + getText() +
                ">.";
    }

}
