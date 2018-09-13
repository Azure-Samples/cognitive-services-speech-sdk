package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Defines contents of speech recognizing/recognized event.
  */
public final class SpeechRecognitionResultEventArgs {
    
    SpeechRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs e) {
        Contracts.throwIfNull(e, "e");

        this._Result = new SpeechRecognitionResult(e.GetResult());
        this._SessionId = e.getSessionId();
    
        Contracts.throwIfNull(this._SessionId, "SessionId");
    }

    /**
      * Specifies the recognition result.
      * @return the recognition result.
      */
    public SpeechRecognitionResult getResult() {
        return _Result;
    }
    private SpeechRecognitionResult _Result;

    /**
      * Specifies the session identifier.
      * @return the session identifier.
      */
    public final String getSessionId() {
        return _SessionId;
    }
    private String _SessionId;

    /**
      * Returns a String that represents the speech recognition result event.
      * @return A String that represents the speech recognition result event.
      */
    @Override
    public String toString() {
        return "SessionId:" + _SessionId +
               " ResultId:" + _Result.getResultId() +
               " Reason:" + _Result.getReason() +
               " Recognized text:<" + _Result.getText() +
               ">.";
    }
}
