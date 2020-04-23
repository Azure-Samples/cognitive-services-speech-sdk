//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Defines result of intent recognition.
 */
public final class IntentRecognitionResult extends com.microsoft.cognitiveservices.speech.SpeechRecognitionResult {

    IntentRecognitionResult(long result) {
        super(result);
        Contracts.throwIfNull(resultHandle, "resultHandle");
        StringRef intentIdStr = new StringRef("");
        Contracts.throwIfFail(getIntentId(resultHandle, intentIdStr));
        this.intentId = intentIdStr.getValue();
        Contracts.throwIfNull(this.intentId, "IntentId");
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        super.close();
    }

    /**
     * A String that represents the intent identifier being recognized.
     * @return A String that represents the intent identifier being recognized.
     */
    public String getIntentId() {
        return intentId;
    }

    private String intentId;

    /**
     * Returns a String that represents the intent recognition result.
     * @return A String that represents the intent recognition result.
     */
    @Override
    public String toString() {
        return "ResultId:" + getResultId() +
                " Reason:" + getReason() +
                " IntentId:<" + intentId +
                "> Recognized text:<" + getText() +
                "> Recognized json:<" + getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult) +
                "> LanguageUnderstandingJson <" + getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult) +
                ">.";
    }

    private final native long getIntentId(SafeHandle resultHandle, StringRef intentId);
}
