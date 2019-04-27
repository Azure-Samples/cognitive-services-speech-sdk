//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.conversation;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.RecognitionEventArgs;

/**
 * Class that defines conversation transcription event arguments.
 * Added in version 1.5.0
 */
public class ConversationTranscriptionEventArgs extends RecognitionEventArgs {

    ConversationTranscriptionEventArgs(com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionEventArgs e) {
        super(e);

        Contracts.throwIfNull(e, "e");
        this._Result = new ConversationTranscriptionResult(e.GetResult());

        Contracts.throwIfNull(this.getSessionId(), "SessionId");
    }

    /**
     * Specifies the conversation transcription result.
     * @return the conversation transcription result.
     */
    public final ConversationTranscriptionResult getResult() {
        return _Result;
    }
    private ConversationTranscriptionResult _Result;

    /**
     * Returns a String that represents the conversation transcription result event.
     * @return A String that represents the conversation transcription result event.
     */
    @Override
    public String toString() {
        return "SessionId:" + this.getSessionId() +
                " ResultId:" + _Result.getResultId() +
                " Reason:" + _Result.getReason() +
                " UserId:" + _Result.getUserId() +
                " Recognized text:<" + _Result.getText() + ">.";
    }
}
