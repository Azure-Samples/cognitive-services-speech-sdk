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
     * Indicates an authentication error. The errorDetails property contains detailed error response.
     */
    AuthenticationFailure(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.AuthenticationFailure),

    /**
     * Indicates that one or more recognition parameters are invalid.
     */
    BadRequestParameters(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.BadRequestParameters),

    /**
     * Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
     */
    TooManyRequests(com.microsoft.cognitiveservices.speech.internal.CancellationErrorCode.TooManyRequests),

    /**
     * Indicates an connection error. The errorDetails property contains detailed error response.
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
     * Indicates an unexpected runtime error. The errorDetails property contains detailed error response.
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
