//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines contents of speech recognizing/recognized event.
 */
public class SpeechRecognitionEventArgs extends RecognitionEventArgs {

    SpeechRecognitionEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs e) {
        super(e);

        Contracts.throwIfNull(e, "e");
        this._Result = new SpeechRecognitionResult(e.GetResult());

        Contracts.throwIfNull(this.getSessionId(), "SessionId");
    }

    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public final SpeechRecognitionResult getResult() {
        return _Result;
    }
    private SpeechRecognitionResult _Result;

    /**
     * Returns a String that represents the speech recognition result event.
     * @return A String that represents the speech recognition result event.
     */
    @Override
    public String toString() {
        return "SessionId:" + this.getSessionId() +
                " ResultId:" + _Result.getResultId() +
                " Reason:" + _Result.getReason() +
                " Recognized text:<" + _Result.getText() + ">.";
    }
}
