package com.microsoft.cognitiveservices.speech.recognition.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Define payload of speech intermediate/final result events.
/// </summary>
public class SpeechRecognitionResultEventArgs //: System.EventArgs
{
    SpeechRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs e)
    {
        this._Result = new SpeechRecognitionResult(e.getResult());
        this._SessionId = e.getSessionId();
    }

    /// <summary>
    /// Specifies the recognition result.
    /// </summary>
    public SpeechRecognitionResult getResult()
    {
        return _Result;
    }// { get; }
    private SpeechRecognitionResult _Result;

    /// <summary>
    /// Specifies the session identifier.
    /// </summary>
    public final String getSessionId()
    {
        return _SessionId;
    }// { get; }
    private String _SessionId;

    /// <summary>
    /// Returns a String that represents the speech recognition result event.
    /// </summary>
    /// <returns>A String that represents the speech recognition result event.</returns>
    @Override
    public String toString()
    {
        return "SessionId:" + _SessionId + " ResultId:" + _Result.getResultId() + " Status:" + _Result.getReason() + " Recognized text:<" + _Result.getRecognizedText() + ">.";
    }
}
