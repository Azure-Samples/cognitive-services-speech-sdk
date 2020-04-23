//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Contains detailed information for NoMatch recognition results.
 */
public class NoMatchDetails {

    private com.microsoft.cognitiveservices.speech.NoMatchReason reason;

    /**
     * Creates an instance of NoMatchDetails object for the NoMatch SpeechRecognitionResults.
     * @param result The recognition result that has NoMatch returned in Reason.
     * @return The NoMatchDetails object being created.
     */
    public static com.microsoft.cognitiveservices.speech.NoMatchDetails fromResult(RecognitionResult result) {
        return new NoMatchDetails(result);
    }

    /*! \cond PROTECTED */

    protected NoMatchDetails(RecognitionResult result) {
        Contracts.throwIfNull(result, "result");

        IntRef intVal = new IntRef(0);
        Contracts.throwIfFail(getResultReason(result.getImpl(), intVal));
        this.reason = NoMatchReason.values()[(int)intVal.getValue() - 1]; // Native NoMatchReason enum starts at 1!!
    }

    /*! \endcond */

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
    }

    /**
     * The reason why NoMatch is returned.
     * @return Specifies the reason for NoMatch.
     */
    public com.microsoft.cognitiveservices.speech.NoMatchReason getReason() {
        return this.reason;
    }

    /**
     * Returns a String that represents the no match details.
     * @return A String that represents the no match details.
     */
    @Override
    public String toString() {
        return "NoMatchReason:" + this.reason;
    }

    private final native long getResultReason(SafeHandle resultHandle, IntRef reasonVal);
}
