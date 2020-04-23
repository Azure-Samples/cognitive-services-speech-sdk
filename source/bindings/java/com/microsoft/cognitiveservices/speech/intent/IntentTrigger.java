//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Defines intent trigger object.
 */
public final class IntentTrigger {

    /**
     * Creates an instance of a intent trigger from simple phrase.
     * @param simplePhrase The simple phrase.
     * @return A IntentTrigger instance.
     */
    public static IntentTrigger fromPhrase(String simplePhrase) {
        IntRef trigger = new IntRef(0);
        Contracts.throwIfFail(createFromPhrase(trigger, simplePhrase));
        return new IntentTrigger(trigger);
    }

    /**
     * Creates an instance of a intent trigger from LanguageUnderstandingModel.
     * @param modelHandle The model handle.
     * @return A IntentTrigger instance.
     */
    public static IntentTrigger fromModel(SafeHandle modelHandle) {
        IntRef trigger = new IntRef(0);
        Contracts.throwIfFail(createFromLanguageUnderstandingModel(trigger, modelHandle, null));
        return new IntentTrigger(trigger);
    }

    /**
     * Creates an instance of a intent trigger from LanguageUnderstandingModel.
     * @param modelHandle The model handle.
     * @param intentName The intent name. 
     * @return A IntentTrigger instance.
     */
    public static IntentTrigger fromModel(SafeHandle modelHandle, String intentName) {
        IntRef trigger = new IntRef(0);
        Contracts.throwIfFail(createFromLanguageUnderstandingModel(trigger, modelHandle, intentName));
        return new IntentTrigger(trigger);
    }

    /*! \cond INTERNAL */

    public SafeHandle getImpl() {
        return triggerHandle;
    }
    
    /*! \endcond */

    private IntentTrigger(IntRef trigger) {
        triggerHandle = new SafeHandle(trigger.getValue(), SafeHandleType.IntentTrigger);
    }

    private SafeHandle triggerHandle = null;

    private final static native long createFromPhrase(IntRef triggerHandle, String simplePhrasePtr);
    private final static native long createFromLanguageUnderstandingModel(IntRef triggerHandle, SafeHandle luModelHandle, String intentName);
}
