//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.RecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/** Defines a payload for Transcribing, and Transcribed events. Added in 1.12.0. */
public class ConversationTranslationEventArgs extends RecognitionEventArgs {

    ConversationTranslationEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);
    }

    ConversationTranslationEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /**
     * Gets the conversation translation result.
     * @return the result.
     */
    public final ConversationTranslationResult getResult() {
        return result;
    }

    @Override
    public String toString() {
        return "SessionId:"
                + this.getSessionId()
                + " ResultId:"
                + result.getResultId()
                + " Reason:"
                + result.getReason()
                + " OriginalLang:"
                + result.getOriginalLang()
                + " ParticipantId:"
                + result.getParticipantId()
                + " Recognized text:<"
                + result.getText()
                + ">.";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getRecognitionResult(eventHandle, resultHandle));
        this.result = new ConversationTranslationResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }

    private ConversationTranslationResult result;
}
