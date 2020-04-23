//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;


import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.CancellationErrorCode;

/**
 * Contains detailed information about why a speech synthesis was canceled.
 * Added in version 1.7.0
 */
public class SpeechSynthesisCancellationDetails {

    private CancellationReason reason;
    private String errorDetails;
    private CancellationErrorCode errorCode;

    /**
     * Creates an instance of SpeechSynthesisCancellationDetails object for the canceled SpeechSynthesisResult.
     * @param result The speech synthesis result that was canceled.
     * @return The cancellation details object being created.
     */
    public static com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails fromResult(SpeechSynthesisResult result) {
        return new com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails(result);
    }

    /**
     * Creates an instance of SpeechSynthesisCancellationDetails object for the canceled AudioDataStream.
     * @param stream The audio data stream that was canceled.
     * @return The cancellation details object being created.
     */
    public static com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails fromStream(AudioDataStream stream) {
        return new com.microsoft.cognitiveservices.speech.SpeechSynthesisCancellationDetails(stream);
    }

    private SpeechSynthesisCancellationDetails(SpeechSynthesisResult result) {
        Contracts.throwIfNull(result, "result");
        Contracts.throwIfNull(result.getImpl(), "resultHandle");

        IntRef valueRef = new IntRef(0);
        Contracts.throwIfFail(getCanceledReasonFromSynthResult(result.getImpl(), valueRef));
        this.reason = CancellationReason.values()[(int)valueRef.getValue() - 1]; // Native CancellationReason enum starts at 1!!
        Contracts.throwIfFail(getCanceledErrorCodeFromSynthResult(result.getImpl(), valueRef));
        this.errorCode = CancellationErrorCode.values()[(int)valueRef.getValue()];
        this.errorDetails = result.getProperties().getProperty(PropertyId.CancellationDetails_ReasonDetailedText);
    }

    private SpeechSynthesisCancellationDetails(AudioDataStream stream) {
        Contracts.throwIfNull(stream, "stream");
        Contracts.throwIfNull(stream.getImpl(), "streamHandle");
        
        IntRef valueRef = new IntRef(0);
        Contracts.throwIfFail(getCanceledReasonFromStream(stream.getImpl(), valueRef));
        this.reason = CancellationReason.values()[(int)valueRef.getValue() - 1]; // Native CancellationReason enum starts at 1!!
        Contracts.throwIfFail(getCanceledErrorCodeFromStream(stream.getImpl(), valueRef));
        this.errorCode = CancellationErrorCode.values()[(int)valueRef.getValue()];
        this.errorDetails = stream.getProperties().getProperty(PropertyId.CancellationDetails_ReasonDetailedText);
    }

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

    private final native long getCanceledReasonFromSynthResult(SafeHandle resultHandle, IntRef valueRef);
    private final native long getCanceledErrorCodeFromSynthResult(SafeHandle resultHandle, IntRef valueRef);
    private final native long getCanceledReasonFromStream(SafeHandle streamHandle, IntRef valueRef);
    private final native long getCanceledErrorCodeFromStream(SafeHandle streamHandle, IntRef valueRef);
}
