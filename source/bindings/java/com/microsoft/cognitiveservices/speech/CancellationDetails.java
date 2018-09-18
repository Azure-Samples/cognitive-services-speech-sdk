//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;


import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Contains detailed information about why a result was canceled.
 */
public class CancellationDetails {

    private CancellationReason reason;
    private String errorDetails;
    private com.microsoft.cognitiveservices.speech.internal.CancellationDetails _cancellationImpl;

    /**
     * Creates an instance of CancellationDetails object for the canceled RecognitionResult.
     * @param result The result that was canceled.
     * @return The cancellation details object being created.
     */
    public static com.microsoft.cognitiveservices.speech.CancellationDetails fromResult(RecognitionResult result) {
        com.microsoft.cognitiveservices.speech.internal.CancellationDetails cancellation = com.microsoft.cognitiveservices.speech.internal.CancellationDetails.FromResult(result.getResultImpl());
        return new com.microsoft.cognitiveservices.speech.CancellationDetails(cancellation);
    }

    protected CancellationDetails(com.microsoft.cognitiveservices.speech.internal.CancellationDetails cancellation) {
        Contracts.throwIfNull(cancellation, "cancellation");

        this._cancellationImpl = cancellation;
        this.reason = CancellationReason.values()[cancellation.getReason().swigValue() - 1]; // Native CancellationReason enum starts at 1!!
        this.errorDetails = cancellation.getErrorDetails();
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this._cancellationImpl != null) {
            this._cancellationImpl.delete();
        }
        this._cancellationImpl = null;
    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public CancellationReason getReason() {
        return this.reason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
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
                " ErrorDetails:" + this.errorDetails;
    }
}
