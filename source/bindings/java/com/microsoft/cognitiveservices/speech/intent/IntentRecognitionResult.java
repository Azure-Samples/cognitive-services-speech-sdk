//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;

/**
 * Defines result of intent recognition.
 */
public final class IntentRecognitionResult extends com.microsoft.cognitiveservices.speech.SpeechRecognitionResult {
    private com.microsoft.cognitiveservices.speech.internal.IntentRecognitionResult _resultImpl;

    IntentRecognitionResult(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionResult result) {
        super(result);
        Contracts.throwIfNull(result, "result");

        this._resultImpl = result;
        this._intentId = result.getIntentId();
        Contracts.throwIfNull(this._intentId, "IntentId");
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        if (this._resultImpl != null) {
            this._resultImpl.delete();
        }
        this._resultImpl = null;

        super.close();
    }

    /**
     * A String that represents the intent identifier being recognized.
     * @return A String that represents the intent identifier being recognized.
     */
    public String getIntentId() {
        return _intentId;
    }
    private String _intentId;

    /**
     * A String that represents the intent including properties being recognized.
     * @return A String that represents the intent including properties being recognized.
     */
    public String getLanguageUnderstanding() {
        return getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult);
    }

    /**
     * Returns a String that represents the intent recognition result.
     * @return A String that represents the intent recognition result.
     */
    @Override
    public String toString() {
        return "ResultId:" + getResultId() +
                " Reason:" + getReason() +
                " IntentId:<" + _intentId +
                "> Recognized text:<" + getText() +
                "> Recognized json:<" + getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult) +
                "> LanguageUnderstandingJson <" + getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult) +
                ">.";
    }
}
