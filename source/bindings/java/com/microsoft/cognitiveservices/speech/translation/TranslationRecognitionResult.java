//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import java.util.HashMap;
import java.util.Map;

import com.microsoft.cognitiveservices.speech.RecognitionResult;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.StringMapRef;

/**
  * Defines translation recognition result.
  */
//FIXME is this ok to make not final?
public class TranslationRecognitionResult extends RecognitionResult {

    //FIXME is this ok to make public?
    /**
     * Constructor
     * @param result
     */
    public TranslationRecognitionResult(long result) {
        super(result);
        Contracts.throwIfNull(resultHandle, "resultHandle");
        StringMapRef translationsRef = new StringMapRef();
        Contracts.throwIfFail(getTranslations(resultHandle, translationsRef));
        this.translations = new HashMap<String, String>(); 
        if (!translationsRef.getValue().isEmpty()) {
            this.translations.putAll(translationsRef.getValue());
        }
    }

    /**
     * Presents the translation results. Each item in the dictionary represents translation result in one of target languages, where the key
     * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
     * @return the current translation map.
     */
    public final Map<String, String> getTranslations() {
        return this.translations;
    }

    /**
     * Returns a String that represents the translation recognition result.
     * @return A String that represents the translation recognition result.
     */
    @Override
    public String toString() {
        String text = "ResultId:" + this.getResultId() +
                " Reason:" + this.getReason()  +
                ", Recognized text:<" + this.getText() + ">.\n";

        for(String key : this.translations.keySet()) {
            text += "    Translation in " + key + ": <" + this.translations.get(key) + ">.\n";
        }

        return text;
    }

    private final native long getTranslations(SafeHandle resultHandle, StringMapRef translations);

    private Map<String, String> translations;
}
