//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.util.StringRef;

/**
 * Class that defines conversation transcription result.
 * Added in version 1.5.0
 */
public class ConversationTranscriptionResult extends SpeechRecognitionResult {

    /*! \cond PROTECTED */

    protected ConversationTranscriptionResult(long result) {
        super(result);

        if (result != 0) {
            StringRef userIdStr = new StringRef("");
            Contracts.throwIfFail(getUserId(resultHandle, userIdStr));
            this.userId = userIdStr.getValue();

            StringRef utteranceIdStr = new StringRef("");
            Contracts.throwIfFail(getUtteranceId(resultHandle, utteranceIdStr));
            this.utteranceId = utteranceIdStr.getValue();
        }
    }

    /*! \endcond */

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        super.close();
    }

    /**
     * A String that represents the user id in the conversation.
     * @return the user ID string.
     */
    public String getUserId() {
        return this.userId;
    }

    /**
     * A string that represents the utterance. This id is consistence for intermediates and final speech recognition result from one speaker.
     * @return the utterance ID string.
     */
    public String getUtteranceId() {
        return this.utteranceId;
    }

    /**
     * Returns a String that represents the conversation transcription result.
     * @return A String that represents the conversation transcription result.
     */
    @Override
    public String toString() {
        return "ResultId:" + this.getResultId()+
                " Status:" + this.getReason() +
                " UserId:" + this.userId +
                " UtteranceId:" + this.utteranceId +
                " Recognized text:<" + this.getText() + ">.";
    }

    private final native long getUserId(SafeHandle resultHandle, StringRef userId);
    private final native long getUtteranceId(SafeHandle resultHandle, StringRef utteranceId);

    private String userId;
    private String utteranceId;
}
