package com.microsoft.cognitiveservices.speech.recognition.translation;

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/**
  * Define payload of translation intermediate/final result events.
  */
public class TranslationTextResultEventArgs //: System.EventArgs
{
    TranslationTextResultEventArgs(com.microsoft.cognitiveservices.speech.recognition.translation.TranslationTextResultEventArgs e)
    {
        this._Result = new TranslationTextResult(e.getResult());
        this._SessionId = e.getSessionId();
    }

    /**
      * Specifies the recognition result.
      * @return the recognition result.
      */
    public final TranslationTextResult getResult() // { get; }
    {
        return _Result;
    }
    private TranslationTextResult _Result;

    /**
      * Specifies the session identifier.
      * @return the session identifier.
      */
    public final String getSessionId()
    {
        return _SessionId;
    }// { get; }
    private String _SessionId;

    /**
      * Returns a String that represents the speech recognition result event.
      * @return A String that represents the speech recognition result event.
      */
    @Override
    public String toString()
    {
        return "TranslationTextResult";
    }
}
