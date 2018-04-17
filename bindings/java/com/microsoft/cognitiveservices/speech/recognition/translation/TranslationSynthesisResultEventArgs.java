package com.microsoft.cognitiveservices.speech.recognition.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Define payload of translation synthesis result events.
/// </summary>
public class TranslationSynthesisResultEventArgs //: System.EventArgs
{
    TranslationSynthesisResultEventArgs(com.microsoft.cognitiveservices.speech.recognition.translation.TranslationSynthesisResultEventArgs e)
    {
        this._Result = new TranslationSynthesisResult(e.getResult());
        this._sessionId = e.getSessionId();
    }

    /// <summary>
    /// Specifies the translation synthesis result.
    /// </summary>
    public final TranslationSynthesisResult getResult() {
        return _Result;
    }// { get; }
    private TranslationSynthesisResult _Result;

    /// <summary>
    /// Specifies the session identifier.
    /// </summary>
    public final String getSessionId()
    {
        return _sessionId;
    }// { get; }
    private String _sessionId;

    /// <summary>
    /// Returns a String that represents the speech recognition result event.
    /// </summary>
    /// <returns>A String that represents the speech recognition result event.</returns>
    @Override
    public String toString()
    {
        return "TranslationSynthesisResult";
    }
}
