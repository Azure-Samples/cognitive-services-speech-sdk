//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Contains detailed information about why a result was canceled.
 */
public class CancellationDetails {

    /**
     * Creates an instance of CancellationDetails object for the canceled RecognitionResult.
     * @param result The result that was canceled.
     * @return The cancellation details object being created.
     */
    public static com.microsoft.cognitiveservices.speech.CancellationDetails fromResult(RecognitionResult result) {
        return new com.microsoft.cognitiveservices.speech.CancellationDetails(result);
    }

    /*! \cond PROTECTED */

    protected CancellationDetails(RecognitionResult result) {
        Contracts.throwIfNull(result, "result");
        Contracts.throwIfNull(result.getImpl(), "result.resultHandle");        
        IntRef value = new IntRef(0);
        Contracts.throwIfFail(getCanceledReason(result.getImpl(), value));
        this.reason = CancellationReason.values()[(int)value.getValue() - 1]; // Native CancellationReason enum starts at 1!!
        Contracts.throwIfFail(getCanceledErrorCode(result.getImpl(), value));
        this.errorCode = CancellationErrorCode.values()[(int)value.getValue()];
        this.errorDetails = result.getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
    }

    /*! \endcond */

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public CancellationReason getReason() {
        return this.reason;
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
     * Returns a String that represents the cancellation details.
     * @return A String that represents the cancellation details.
     */
    @Override
    public String toString() {
        return "CancellationReason:" + this.reason +
                " ErrorCode: " + this.errorCode +
                " ErrorDetails:" + this.errorDetails;
    }

    private CancellationReason reason;
    private String errorDetails;
    private CancellationErrorCode errorCode;
    
    private final native long getCanceledReason(SafeHandle resultHandle, IntRef value);
    private final native long getCanceledErrorCode(SafeHandle resultHandle, IntRef value);
}
