package com.microsoft.cognitiveservices.speech.recognition.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Define payload of intent intermediate/final result events.
/// </summary>
public class IntentRecognitionResultEventArgs // : System.EventArgs
{
    IntentRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs e)
    {
        this._Result = new IntentRecognitionResult(e.getResult());
        this._SessionId = e.getSessionId();
    }

    /// <summary>
    /// Represents the intent recognition result.
    /// </summary>
    public final IntentRecognitionResult getResult()
    {
        return _Result;
    }//{ get; }
    private IntentRecognitionResult  _Result;

    /// <summary>
    /// A String represents the session identifier.
    /// </summary>
    public final String getSessionId()
    {
        return _SessionId;
    }
    // { get; }
    private String _SessionId;

    /// <summary>
    /// Returns a String that represents the session id and the intent recognition result event.
    /// </summary>
    /// <returns>A String that represents the intent recognition result event.</returns>
    @Override
    public String toString()
    {
        return "SessionId:" + _SessionId + " ResultId:" + _Result.getResultId() + " Status:" + _Result.getReason() + " IntentId:<" + _Result.getIntentId() + "> Recognized text:<" + _Result.getRecognizedText() + ">."; 
    }
}
