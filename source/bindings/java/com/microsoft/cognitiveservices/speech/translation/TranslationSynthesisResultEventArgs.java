//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines the content of translation synthesis result event.
 */
public final class TranslationSynthesisResultEventArgs extends SessionEventArgs
{
    TranslationSynthesisResultEventArgs(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResultEventArgs eventArg) {
        super(eventArg);

        Contracts.throwIfNull(eventArg, "eventArg");

        this._Result = new TranslationSynthesisResult(eventArg.GetResult());
    }

    /**
     * Specifies the translation synthesis result.
     * @return Specifies the translation synthesis result.
     */
    public final TranslationSynthesisResult getResult() {
        return _Result;
    }
    private TranslationSynthesisResult _Result;

    /**
     * Returns a String that represents the speech recognition result event.
     * @return A String that represents the speech recognition result event.
     */
    @Override
    public String toString()
    {
        return "SessionId:" + this.getSessionId() + " Result:" + _Result.toString()  + ".";
    }
}
