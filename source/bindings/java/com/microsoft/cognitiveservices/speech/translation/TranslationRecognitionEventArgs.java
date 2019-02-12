//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines payload of translation recognizing/recognized events.
 */
public class TranslationRecognitionEventArgs extends com.microsoft.cognitiveservices.speech.RecognitionEventArgs {
    TranslationRecognitionEventArgs(com.microsoft.cognitiveservices.speech.internal.TranslationRecognitionEventArgs eventArg) {
        super(eventArg);
        Contracts.throwIfNull(eventArg, "eventArg");

        this._Result = new TranslationRecognitionResult(eventArg.GetResult());
    }

    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public final TranslationRecognitionResult getResult() {
        return _Result;
    }
    private TranslationRecognitionResult _Result;

    /**
     * Returns a String that represents the speech recognition result event.
     * @return A String that represents the speech recognition result event.
     */
    @Override
    public String toString() {
        String text = "TranslationRecognitionResult " + super.toString();

        for(String key : _Result.getTranslations().keySet()) {
            text += "    Translation in " + key + ": <" + _Result.getTranslations().get(key) + ">.\n";
        }

        return text;
    }
}
