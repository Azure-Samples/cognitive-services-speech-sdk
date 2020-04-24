//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Defines content of an keyword recognizing/recognized events.
 */
public class KeywordRecognitionEventArgs extends com.microsoft.cognitiveservices.speech.RecognitionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a KeywordRecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    KeywordRecognitionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);

    }
    
    KeywordRecognitionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /*! \endcond */

    /**
     * Represents the keyword recognition result.
     * @return Represents the keyword recognition result.
     */
    public final KeywordRecognitionResult getResult() {
        return result;
    }

    /**
     * Returns a String that represents the session id and the keyword recognition result event.
     * @return A String that represents the keyword recognition result event.
     */
    @Override
    public String toString() {
        return "SessionId:" + getSessionId() +
                " ResultId:" + result.getResultId() +
                " Reason:" + result.getReason() +
                "> Recognized text:<" + result.getText() + ">.";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getRecognitionResult(eventHandle, resultHandle));
        this.result = new KeywordRecognitionResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }
    
    private KeywordRecognitionResult result;
}
