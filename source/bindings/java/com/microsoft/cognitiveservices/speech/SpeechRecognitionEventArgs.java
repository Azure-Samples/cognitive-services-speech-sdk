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

    /**
     * Constructs an instance of a SpeechRecognitionEventArgs object.
     * @param arg internal recognition event args object.
     */
    public SpeechRecognitionEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs arg) {
        super(arg);

        Contracts.throwIfNull(arg, "arg");
        this._Result = new SpeechRecognitionResult(arg.GetResult());

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
