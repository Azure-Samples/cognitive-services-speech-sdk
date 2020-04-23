//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Defines content of an intent recognizing/recognized events.
 */
public class IntentRecognitionEventArgs extends com.microsoft.cognitiveservices.speech.RecognitionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a IntentRecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    IntentRecognitionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);

    }
    
    IntentRecognitionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /*! \endcond */

    /**
     * Represents the intent recognition result.
     * @return Represents the intent recognition result.
     */
    public final IntentRecognitionResult getResult() {
        return result;
    }

    /**
     * Returns a String that represents the session id and the intent recognition result event.
     * @return A String that represents the intent recognition result event.
     */
    @Override
    public String toString() {
        return "SessionId:" + getSessionId() +
                " ResultId:" + result.getResultId() +
                " Reason:" + result.getReason() +
                " IntentId:<" + result.getIntentId() +
                "> Recognized text:<" + result.getText() +
                "> Recognized json:<" + result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult) +
                "> LanguageUnderstandingJson <" + result.getProperties().getProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult) + ">.";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getRecognitionResult(eventHandle, resultHandle));
        this.result = new IntentRecognitionResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }
    
    private IntentRecognitionResult result;
}
