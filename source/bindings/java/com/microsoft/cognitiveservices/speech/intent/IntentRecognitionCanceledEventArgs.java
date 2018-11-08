//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Define payload of intent recognition canceled result events.
 */
public final class IntentRecognitionCanceledEventArgs extends IntentRecognitionEventArgs {

    IntentRecognitionCanceledEventArgs(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionCanceledEventArgs e) {
        super(e);
        Contracts.throwIfNull(e, "e");

        this._eventArgImpl = e;

        com.microsoft.cognitiveservices.speech.internal.CancellationDetails cancellation = e.GetCancellationDetails();
        this._cancellationReason  = CancellationReason.values()[cancellation.getReason().swigValue() - 1]; // Native CancellationReason enum starts at 1!!
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
     * Added in version 1.1.0.
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
     * Returns a String that represents the speech recognition canceled event args.
     * @return A String that represents the speech recognition canceled event args.
     */
    @Override
    public String toString() {
        return "SessionId:" + this.getSessionId() +
                " ResultId:" + this.getResult().getResultId() +
                " IntentId:" + this.getResult().getIntentId() +
                " CancellationReason:" + _cancellationReason  +
                " CancellationErrorCode:" + _errorCode +
                " Error details:" + _errorDetails;
    }

    @SuppressWarnings("unused")
    private com.microsoft.cognitiveservices.speech.internal.IntentRecognitionCanceledEventArgs _eventArgImpl;
    private CancellationReason _cancellationReason;
    private CancellationErrorCode _errorCode;
    private String _errorDetails;
}
