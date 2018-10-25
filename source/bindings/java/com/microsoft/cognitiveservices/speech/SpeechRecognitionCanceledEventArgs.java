//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;

/**
 * Defines payload of speech recognition canceled events.
 */
public final class SpeechRecognitionCanceledEventArgs extends SpeechRecognitionEventArgs {

    SpeechRecognitionCanceledEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventArgs e) {
        super(e);

        Contracts.throwIfNull(e, "e");
        this._eventArgImpl = e;
        this._Result = new SpeechRecognitionResult(e.GetResult());

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
     * The error code of why the cancellation occurred.
     * @return An error code that represents the error reason.
     * Added in version 1.1.0.
     */
    public CancellationErrorCode getErrorCode() {
        return this._errorCode;
    }

    /**
     * The error details of why the cancellation occurred.
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
        return "SessionId:" + _SessionId +
                " ResultId:" + _Result.getResultId() +
                " CancellationReason:" + _cancellationReason  +
                " CancellationErrorCode:" + _errorCode +
                " Error details:<" + _errorDetails;
    }

    @SuppressWarnings("unused")
    private com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventArgs _eventArgImpl;
    private String _SessionId;
    private SpeechRecognitionResult _Result;
    private CancellationReason _cancellationReason;
    private CancellationErrorCode _errorCode;
    private String _errorDetails;
}
