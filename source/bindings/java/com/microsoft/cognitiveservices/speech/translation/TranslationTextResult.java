//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import java.util.HashMap;
import java.util.Map;

import com.microsoft.cognitiveservices.speech.RecognitionResult;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines translation text result.
 */
public final class TranslationTextResult extends RecognitionResult {
    TranslationTextResult(com.microsoft.cognitiveservices.speech.internal.TranslationTextResult result) {
        super(result);

        Contracts.throwIfNull(result, "result");
        this._Translations = new HashMap<String, String>();

        com.microsoft.cognitiveservices.speech.internal.StdMapStringString imap = result.getTranslations();
        for (String key : imap) {
            this._Translations.put(key, imap.get(key));
        }
    }

    /**
     * Presents the translation results. Each item in the dictionary represents translation result in one of target languages, where the key
     * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
     * @return the current translation map.
     */
    public final Map<String, String> getTranslations() {
        return this._Translations;
    }
    private Map<String, String> _Translations;

    /**
     * Returns a String that represents the speech recognition result.
     * @return A String that represents the speech recognition result.
     */
    @Override
    public String toString() {
        String text = "ResultId:" + this.getResultId() +
                " Reason:" + this.getReason()  +
                ", Recognized text:<" + this.getText() + ">.\n";

        for(String key : this._Translations.keySet()) {
            text += "    Translation in " + key + ": <" + this._Translations.get(key) + ">.\n";
        }

        return text;
    }
}
