//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.conversation;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.conversation.ConversationTranscriptionResult;

/**
 * Class that defines conversation transcription canceled events.
 * Added in version 1.5.0
 */
public final class ConversationTranscriptionCanceledEventArgs extends ConversationTranscriptionEventArgs {

    ConversationTranscriptionCanceledEventArgs(com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionCanceledEventArgs e) {
        super(e);

        Contracts.throwIfNull(e, "e");
        this._eventArgImpl = e;
        this._Result = new ConversationTranscriptionResult(e.GetResult());

        this._SessionId = e.getSessionId();
        Contracts.throwIfNull(this._SessionId, "SessionId");

        com.microsoft.cognitiveservices.speech.internal.CancellationDetails cancellation = e.GetCancellationDetails();
        this._cancellationReason  = com.microsoft.cognitiveservices.speech.CancellationReason.values()[cancellation.getReason().swigValue() - 1]; // Native CancellationReason enum starts at 1!!
        this._errorCode = com.microsoft.cognitiveservices.speech.CancellationErrorCode.values()[cancellation.getErrorCode().swigValue()];
        this._errorDetails = cancellation.getErrorDetails();
    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public CancellationReason getReason() {
        return this._cancellationReason ;
    }

    /**
     * The error code in case of an unsuccessful recognition (when getReason() returns Error).
     * @return An error code that represents the error reason.
     */
    public CancellationErrorCode getErrorCode() {
        return this._errorCode;
    }

    /**
     * The error message in case of an unsuccessful recognition (when getReason() returns Error).
     * @return A String that represents the error details.
     */
    public String getErrorDetails() {
        return this._errorDetails;
    }

    /**
     * Returns a String that represents the conversation transcribing canceled event args.
     * @return A String that represents the conversation transcribing canceled event args.
     */
    @Override
    public String toString() {
        return "SessionId:" + _SessionId +
                " ResultId:" + _Result.getResultId() +
                " CancellationReason:" + _cancellationReason  +
                " CancellationErrorCode:" + _errorCode +
                " Error details:<" + _errorDetails;
    }

    @SuppressWarnings("unused")
    private com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionCanceledEventArgs _eventArgImpl;
    private String _SessionId;
    private ConversationTranscriptionResult _Result;
    private CancellationReason _cancellationReason;
    private CancellationErrorCode _errorCode;
    private String _errorDetails;
}
