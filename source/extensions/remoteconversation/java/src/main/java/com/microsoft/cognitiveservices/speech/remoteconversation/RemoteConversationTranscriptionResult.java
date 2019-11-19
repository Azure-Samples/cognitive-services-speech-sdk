//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriptionResult;

import java.util.ArrayList;
import java.util.List;

/**
 * Remote conversation transcription result class.
 * Added in version 1.8.0
 */

public class RemoteConversationTranscriptionResult {
    String id;
    List<ConversationTranscriptionResult> transcriptions = null;
    RemoteConversationTranscriptionResult() {
        transcriptions = new ArrayList<>();
    }

    /**
     * Return the id associated with the remote conversation transcription result
     * @return The id associated with the remote operation.
     */
    public String getId(){
        return id;
    }

    /**
     * Return the list of ConversationTranscriptionResult
     * @return list of ConversationTranscriptionResult.
     */
    public List<ConversationTranscriptionResult> getConversationTranscriptionResults() {
        return transcriptions;
    }
}
