package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines contents of speech intermediate/final result event.
  */
public final class SpeechRecognitionResultEventArgs {
    
    SpeechRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs e) {
        this._Result = new SpeechRecognitionResult(e.getResult());
        this._SessionId = e.getSessionId();
    }

    /**
      * Specifies the recognition result.
      * @return the recognition result.
      */
    public SpeechRecognitionResult getResult() {
        return _Result;
    }// { get; }
    private SpeechRecognitionResult _Result;

    /**
      * Specifies the session identifier.
      * @return the session identifier.
      */
    public final String getSessionId() {
        return _SessionId;
    }// { get; }
    private String _SessionId;

    /**
      * Returns a String that represents the speech recognition result event.
      * @return A String that represents the speech recognition result event.
      */
    @Override
    public String toString() {
        return "SessionId:" + _SessionId +
               " ResultId:" + _Result.getResultId() +
               " Status:" + _Result.getReason() +
               " Recognized text:<" + _Result.getRecognizedText() +
               ">.";
    }
}
