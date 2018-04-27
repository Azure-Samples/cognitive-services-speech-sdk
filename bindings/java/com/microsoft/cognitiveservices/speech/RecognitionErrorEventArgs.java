package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines content of a RecognitionErrorEvent. 
  */
public class RecognitionErrorEventArgs //extends System.EventArgs
{
    public RecognitionErrorEventArgs(String sessionId, RecognitionStatus status)
    {
        this._status = status;
        this._sessionId = sessionId;
    }

    public RecognitionErrorEventArgs(String sessionId, com.microsoft.cognitiveservices.speech.internal.Reason reason)
    {
//        Debug.Assert((int)Internal.Reason.Recognized == (int)RecognitionStatus.Success);
//        Debug.Assert((int)Internal.Reason.IntermediateResult == (int)RecognitionStatus.IntermediateResult);
//        Debug.Assert((int)Internal.Reason.NoMatch == (int)RecognitionStatus.NoMatch);
//        Debug.Assert((int)Internal.Reason.Canceled == (int)RecognitionStatus.Canceled);
//        Debug.Assert((int)Internal.Reason.OtherRecognizer == (int)RecognitionStatus.OtherRecognizer);

        this._status = RecognitionStatus.values()[(int)reason.swigValue()];
        this._sessionId = sessionId;
    }

    /**
      * Specifies the error reason.
      * @return Specifies the error reason.
      */
    public RecognitionStatus getStatus() {
        return _status;
    }// { get; }
    private RecognitionStatus _status;// { get; }

    /**
      * Specifies the session identifier.
      * @return Specifies the session identifier.
      */
    public String sessionId() {
        return _sessionId;
    } // { get;  }
    private String _sessionId;

    /**
      * Returns a String that represents the recognition error event.
      * @return A String that represents the recognition error event.
      */
    @Override
    public String toString()
    {
        return "SessionId: " + _sessionId + " Status: " + _status + "."; 
    }
}
