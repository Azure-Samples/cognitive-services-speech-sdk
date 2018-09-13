package com.microsoft.cognitiveservices.speech.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Define payload of translation recognizing/recognized events.
  */
public final class TranslationTextResultEventArgs //: System.EventArgs
{
    TranslationTextResultEventArgs(com.microsoft.cognitiveservices.speech.internal.TranslationTextResultEventArgs eventArg)
    {
        Contracts.throwIfNull(eventArg, "eventArg");

        this._Result = new TranslationTextResult(eventArg.GetResult());
        this._SessionId = eventArg.getSessionId();
    }

    /**
      * Specifies the recognition result.
      * @return the recognition result.
      */
    public final TranslationTextResult getResult()
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
    }
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
