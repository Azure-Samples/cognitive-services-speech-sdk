//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;

/**
 * Class that defines conversation transcription result.
 * Added in version 1.5.0
 */
public class ConversationTranscriptionResult extends SpeechRecognitionResult {

    /*! \cond PROTECTED */

    protected ConversationTranscriptionResult(com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionResult result) {
        super(result);

        resultImpl = result;
    }

    /*! \endcond */

    /**
     * A string that represents the user id.
     * @return the user ID string.
     */
    public String getUserId() {
        return resultImpl.getUserId();
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        super.close();
    }

    /**
     * Returns a String that represents the conversation transcription result.
     * @return A String that represents the conversation transcription result.
     */
    @Override
    public String toString() {
        return "ResultId:" + this.getResultId()+
                " Status:" + this.getReason() +
                " UserId:" + this.getUserId() +
                " Recognized text:<" + this.getText() + ">.";
    }

    private com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionResult resultImpl;
}
