//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 *  Defines error code in case that CancellationReason is Error. 
 *  Added in version 1.1.0.
 */
public enum CancellationErrorCode
{
    /**
     * Indicates that an error occurred during speech recognition. Use getErrorDetails() contains detailed error response.
     */
    NoError(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.NoError),

    /**
     * Indicates an authentication error.
     */
    AuthenticationFailure(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.AuthenticationFailure),

    /**
     * Indicates that one or more recognition parameters are invalid or the audio format is not supported.
     */
    BadRequest(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.BadRequest),

    /**
     * Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
     */
    TooManyRequests(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.TooManyRequests),

    /**
     * Indicates that the free subscription used by the request ran out of quota.
     */
    Forbidden(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.Forbidden),

    /**
     * Indicates a connection error.
     */
    ConnectionFailure(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.ConnectionFailure),

    /**
     * Indicates a time-out error when waiting for response from service.
     */
    ServiceTimeout(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.ServiceTimeout),

    /**
     * Indicates that an error is returned by the service.
     */
    ServiceError(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.ServiceError),

    /**
     * Indicates that the service is currently unavailable.
     */
    ServiceUnavailable(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.ServiceUnavailable),

    /**
     * Indicates an unexpected runtime error.
     */
    RuntimeError(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.RuntimeError);

    private CancellationErrorCode(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode id) {
        this.id = id;
    }

    /**
     * Returns the internal value error code
     *
     * @return the error code
     */
    public com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode id;    
}
