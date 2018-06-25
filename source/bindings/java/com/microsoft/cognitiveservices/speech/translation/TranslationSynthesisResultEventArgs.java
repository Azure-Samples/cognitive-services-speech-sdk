package com.microsoft.cognitiveservices.speech.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Defines the content of translation synthesis result event.
  */
public final class TranslationSynthesisResultEventArgs //: System.EventArgs
{
    TranslationSynthesisResultEventArgs(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResultEventArgs eventArg)
    {
        Contracts.throwIfNull(eventArg, "eventArg");

        this._Result = new TranslationSynthesisResult(eventArg.getResult());
        this._sessionId = eventArg.getSessionId();
    }

    /**
      * Specifies the translation synthesis result.
      * @return Specifies the translation synthesis result.
      */
    public final TranslationSynthesisResult getResult() {
        return _Result;
    }// { get; }
    private TranslationSynthesisResult _Result;

    /**
      * Specifies the session identifier.
      * @return Specifies the session identifier.
      */
    public final String getSessionId()
    {
        return _sessionId;
    }// { get; }
    private String _sessionId;

    /**
      * Returns a String that represents the speech recognition result event.
      * @return A String that represents the speech recognition result event.
      */
    @Override
    public String toString()
    {
        return "SessionId:" + _sessionId + " Result:" + _Result.toString()  + ".";
    }
}
