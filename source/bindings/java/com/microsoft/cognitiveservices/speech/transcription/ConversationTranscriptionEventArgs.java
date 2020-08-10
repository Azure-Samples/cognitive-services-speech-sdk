//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.RecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Class that defines conversation transcription event arguments.
 * Added in version 1.5.0
 */
public class ConversationTranscriptionEventArgs extends RecognitionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a ConversationTranscriptionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    ConversationTranscriptionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);

    }
    
    ConversationTranscriptionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }
    
    /*! \endcond */

    /**
     * Represents the conversation transcription result.
     * @return The conversation transcription result.
     */
    public final ConversationTranscriptionResult getResult() {
        return result;
    }

    /**
     * Returns a String that represents the conversation transcription result event.
     * @return A String that represents the conversation transcription result event.
     */
    @Override
    public String toString() {
        return "SessionId:" + this.getSessionId() +
                " ResultId:" + result.getResultId() +
                " Reason:" + result.getReason() +
                " UserId:" + result.getUserId() +
                " UtteranceId:" + result.getUtteranceId() +
                " Recognized text:<" + result.getText() + ">.";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getRecognitionResult(eventHandle, resultHandle));
        this.result = new ConversationTranscriptionResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }
    
    private ConversationTranscriptionResult result;
}
