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
     * Indicates that no error occurred during speech recognition.
     * If CancellationReason is EndOfStream, CancellationErrorCode 
     * is set to NoError.
     */
    NoError(0),

    /**
     * Indicates an authentication error.
     * An authentication error occurs if subscription key or authorization token is invalid, expired, 
     * or does not match the region being used.
     */
    AuthenticationFailure(1),

    /**
     * Indicates that one or more recognition parameters are invalid or the audio format is not supported.
     */
    BadRequest(2),

    /**
     * Indicates that the number of parallel requests exceeded the number of allowed concurrent transcriptions for the subscription.
     */
    TooManyRequests(3),

    /**
     * Indicates that the free subscription used by the request ran out of quota.
     */
    Forbidden(4),

    /**
     * Indicates a connection error.
     */
    ConnectionFailure(5),

    /**
     * Indicates a time-out error when waiting for response from service.
     */
    ServiceTimeout(6),

    /**
     * Indicates that an error is returned by the service.
     */
    ServiceError(7),

    /**
     * Indicates that the service is currently unavailable.
     */
    ServiceUnavailable(8),

    /**
     * Indicates an unexpected runtime error.
     */
    RuntimeError(9);

    private CancellationErrorCode(int id) {
        this.id = id;
    }

    /**
     * Returns the internal value error code
     *
     * @return the error code
     */
    public int getValue() { return this.id; }

    private final int id;
}
