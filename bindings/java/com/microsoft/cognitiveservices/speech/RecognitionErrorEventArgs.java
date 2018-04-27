package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines payload of RecognitionErrorEvent. 
  */
public class RecognitionErrorEventArgs //extends System.EventArgs
{
    public RecognitionErrorEventArgs(String sessionId, RecognitionStatus status)
    {
        this.status = status;
        this.sessionId = sessionId;
    }

    public RecognitionErrorEventArgs(String sessionId, com.microsoft.cognitiveservices.speech.internal.Reason reason)
    {
//        Debug.Assert((int)Internal.Reason.Recognized == (int)RecognitionStatus.Success);
//        Debug.Assert((int)Internal.Reason.IntermediateResult == (int)RecognitionStatus.IntermediateResult);
//        Debug.Assert((int)Internal.Reason.NoMatch == (int)RecognitionStatus.NoMatch);
//        Debug.Assert((int)Internal.Reason.Canceled == (int)RecognitionStatus.Canceled);
//        Debug.Assert((int)Internal.Reason.OtherRecognizer == (int)RecognitionStatus.OtherRecognizer);

        this.status = RecognitionStatus.values()[(int)reason.swigValue()];
        this.sessionId = sessionId;
    }

    /**
      * Specifies the error reason.
      */
    public final RecognitionStatus status;// { get; }

    /**
      * Specifies the session identifier.
      */
    public final String sessionId; // { get;  }

    /**
      * Returns a String that represents the recognition error event.
      * @return A String that represents the recognition error event.
      */
    @Override
    public String toString()
    {
        return "SessionId: " + sessionId + " Status: " + status + "."; 
    }
}
