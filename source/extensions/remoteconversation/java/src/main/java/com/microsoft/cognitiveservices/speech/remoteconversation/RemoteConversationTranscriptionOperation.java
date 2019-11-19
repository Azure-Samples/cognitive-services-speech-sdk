//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package com.microsoft.cognitiveservices.speech.remoteconversation;


/**
 * This class will tell the current state of a long running operation. It will tell if the audio is being uploaded or processed or it is successfully completed.
 * If there is an error it will tell what the error is.
 * Added in version 1.8.0
 */
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriptionResult;

import java.util.ArrayList;
import java.util.List;

public class RemoteConversationTranscriptionOperation {
    RemoteConversationTranscriptionOperation(String convId){
        this.id = convId;
    }

    void setError(String error) {
        errorString = error;
    }

    void setStatus(String serviceStatus){
        this.curServiceStatus = serviceStatus;
    }

    /**
     * Return the id associated with the remote conversation transcription operation
     * @return The id associated with the remote operation.
     */
    public String getId(){
        return id;
    }

    /**
     * Return the service status associated with the remote conversation transcription operation
     * @return The status associated with the remote operation.
     */
    public String getServiceStatus(){
        return curServiceStatus;
    }

    /**
     * Return the error message in case of any error
     * @return The error message in case of error.
     */
    public String getError()
    {
        return errorString;
    }

    private String errorString = null;
    private String id;
    private String curServiceStatus;
}
