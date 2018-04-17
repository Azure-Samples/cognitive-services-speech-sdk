package com.microsoft.cognitiveservices.speech.recognition;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Defines payload of RecognitionErrorEvent. 
/// </summary>
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

    /// <summary>
    /// Specifies the error reason.
    /// </summary>
    public final RecognitionStatus status;// { get; }

    /// <summary>
    /// Specifies the session identifier.
    /// </summary>
    public final String sessionId; // { get;  }

    /// <summary>
    /// Returns a String that represents the recognition error event.
    /// </summary>
    /// <returns>A String that represents the recognition error event.</returns>
    @Override
    public String toString()
    {
        return "SessionId: " + sessionId + " Status: " + status + "."; 
    }
}
