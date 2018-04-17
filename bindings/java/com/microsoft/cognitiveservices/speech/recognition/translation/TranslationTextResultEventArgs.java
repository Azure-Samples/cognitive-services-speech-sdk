package com.microsoft.cognitiveservices.speech.recognition.translation;

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/// <summary>
/// Define payload of translation intermediate/final result events.
/// </summary>
public class TranslationTextResultEventArgs //: System.EventArgs
{
    TranslationTextResultEventArgs(com.microsoft.cognitiveservices.speech.recognition.translation.TranslationTextResultEventArgs e)
    {
        this._Result = new TranslationTextResult(e.getResult());
        this._SessionId = e.getSessionId();
    }

    /// <summary>
    /// Specifies the recognition result.
    /// </summary>
    public final TranslationTextResult getResult() // { get; }
    {
        return _Result;
    }
    private TranslationTextResult _Result;

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
        return "TranslationTextResult";
    }
}
