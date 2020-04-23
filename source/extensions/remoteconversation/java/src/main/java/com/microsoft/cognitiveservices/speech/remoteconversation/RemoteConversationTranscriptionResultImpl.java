//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriptionResult;
import java.math.BigInteger;

class RemoteConversationTranscriptionResultImpl extends ConversationTranscriptionResult {
    public RemoteConversationTranscriptionResultImpl() {
        super(0);
    }

    /**
     * A string that represents the user id.
     * @return the user ID string.
     */
    @Override
    public String getUserId() {
        return this.userId;
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


    /**
     * Specifies the result identifier.
     * @return Specifies the result identifier.
     */
    @Override
    public String getResultId() {
        return this.resultId;
    }

    /**
     * Specifies reason the result was created.
     * @return Specifies reason of the result.
     */
    @Override
    public ResultReason getReason() {
        return this.reason;
    }

    /**
     * Presents the recognized text in the result.
     * @return Presents the recognized text in the result.
     */
    public String getText() {
        return this.text;
    }

    /**
     * Duration of recognized speech in 100nsec increments.
     * @return BigInteger of recognized speech in 100nsec increments.
     */
    public BigInteger getDuration() {
        return this.duration;
    }

    /**
     * Offset of recognized speech in 100nsec increments.
     * @return Offset of recognized speech in 100nsec increments.
     */
    public BigInteger getOffset() {
        return this.offset;
    }

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public PropertyCollection getProperties() {
        return propertyCollection;
    }

    public void SetAllFields(String usrId, String resId, ResultReason reason, BigInteger duration, BigInteger offset, String text, PropertyCollection collection) {
        this.userId = usrId;
        this.resultId = resId;
        this.reason = reason;
        this.duration = duration;
        this.offset = offset;
        this.text = text;
        this.propertyCollection = collection;
    }

    private String userId;
    private String resultId;
    private ResultReason reason;
    private BigInteger duration;
    private BigInteger offset;
    private String text;
    private PropertyCollection propertyCollection;

}