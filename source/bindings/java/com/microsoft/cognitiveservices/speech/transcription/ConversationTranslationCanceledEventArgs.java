//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.CancellationDetails;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/** Defines a payload for the Canceled event. Added in version 1.12.0. */
public class ConversationTranslationCanceledEventArgs extends ConversationTranslationEventArgs {

    ConversationTranslationCanceledEventArgs(long eventArgs) {
        super(eventArgs);
    }

    ConversationTranslationCanceledEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /**
     * The reason the recognition was canceled.
     *
     * @return Specifies the reason canceled.
     */
    public CancellationReason getReason() {
        return this.cancellationReason;
    }

    /**
     * The error code in case of an unsuccessful recognition (when getReason() returns Error).
     *
     * @return An error code that represents the error reason.
     */
    public CancellationErrorCode getErrorCode() {
        return this.errorCode;
    }

    /**
     * The error message in case of an unsuccessful recognition (when getReason() returns Error).
     *
     * @return A String that represents the error details.
     */
    public String getErrorDetails() {
        return this.errorDetails;
    }

    @Override
    public String toString() {
        return "SessionId:"
                + sessionId
                + " ResultId:"
                + getResult().getResultId()
                + " CancellationReason:"
                + cancellationReason
                + " CancellationErrorCode:"
                + errorCode
                + " Error details:<"
                + errorDetails;
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        this.sessionId = getSessionId();
        Contracts.throwIfNull(this.sessionId, "SessionId");

        CancellationDetails cancellation = CancellationDetails.fromResult(getResult());
        this.cancellationReason = cancellation.getReason();
        this.errorCode = cancellation.getErrorCode();
        this.errorDetails = cancellation.getErrorDetails();
        if (disposeNativeResources == true) {
            super.close();
        }
    }

    private String sessionId;
    private CancellationReason cancellationReason;
    private CancellationErrorCode errorCode;
    private String errorDetails;
}
