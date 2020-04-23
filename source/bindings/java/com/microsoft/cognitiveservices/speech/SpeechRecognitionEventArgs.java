//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Defines contents of speech recognizing/recognized event.
 */
public class SpeechRecognitionEventArgs extends RecognitionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a SpeechRecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    public SpeechRecognitionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);
    }

    public SpeechRecognitionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /*! \endcond */

    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public final SpeechRecognitionResult getResult() {
        return this.result;
    }

    /**
     * Returns a String that represents the speech recognition result event.
     * @return A String that represents the speech recognition result event.
     */
    @Override
    public String toString() {
        return "SessionId:" + this.getSessionId() +
                " ResultId:" + this.result.getResultId() +
                " Reason:" + this.result.getReason() +
                " Recognized text:<" + this.result.getText() + ">.";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getRecognitionResult(eventHandle, resultHandle));
        this.result = new SpeechRecognitionResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }
    
    private SpeechRecognitionResult result;
}
