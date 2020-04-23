//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.CancellationDetails;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;

/**
 * Define payload of intent recognition canceled result events.
 */
public final class IntentRecognitionCanceledEventArgs extends IntentRecognitionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a IntentRecognitionCanceledEventArgs object.
     * @param eventArgs recognition canceled event args object.
     */
    IntentRecognitionCanceledEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);
    }

    IntentRecognitionCanceledEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /*! \endcond */

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public CancellationReason getReason() {
        return this.cancellationReason;
    }

    /**
     * The error code in case of an unsuccessful recognition (when getReason() returns Error).
     * Added in version 1.1.0.
     * @return An error code that represents the error reason.
     */
    public CancellationErrorCode getErrorCode() {
        return this.errorCode;
    }

    /**
     * The error message in case of an unsuccessful recognition (when getReason() returns Error).
     * @return A String that represents the error details.
     */
    public String getErrorDetails() {
        return this.errorDetails;
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
                " CancellationReason:" + cancellationReason  +
                " CancellationErrorCode:" + errorCode +
                " Error details:" + errorDetails;
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        CancellationDetails cancellation = CancellationDetails.fromResult(getResult());
        this.cancellationReason  = cancellation.getReason();
        this.errorCode = cancellation.getErrorCode();
        this.errorDetails = cancellation.getErrorDetails();
        if (disposeNativeResources == true) {
            super.close();
        }
    }

    private CancellationReason cancellationReason;
    private CancellationErrorCode errorCode;
    private String errorDetails;
}
