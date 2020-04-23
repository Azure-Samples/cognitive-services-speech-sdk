//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Defines payload of translation recognizing/recognized events.
 */
public class TranslationRecognitionEventArgs extends com.microsoft.cognitiveservices.speech.RecognitionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a TranslationRecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    TranslationRecognitionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);

    }
    
    TranslationRecognitionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }
    
    /*! \endcond */

    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public final TranslationRecognitionResult getResult() {
        return result;
    }

    /**
     * Returns a String that represents the translation recognition result event.
     * @return A String that represents the translation recognition result event.
     */
    @Override
    public String toString() {
        String text = "TranslationRecognitionResult " + super.toString();

        for(String key : result.getTranslations().keySet()) {
            text += "    Translation in " + key + ": <" + result.getTranslations().get(key) + ">.\n";
        }

        return text;
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getRecognitionResult(eventHandle, resultHandle));
        this.result = new TranslationRecognitionResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }
    
    private TranslationRecognitionResult result;
}
