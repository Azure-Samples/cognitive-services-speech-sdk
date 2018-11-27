// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

/**
 *  Defines error code in case that CancellationReason is Error.
 *  Added in version 1.1.0.
 */
export enum CancellationErrorCode {
    /**
     * Indicates that no error occurred during speech recognition.
     */
    NoError,

    /**
     * Indicates an authentication error.
     */
    AuthenticationFailure,

    /**
     * Indicates that one or more recognition parameters are invalid.
     */
    BadRequestParameters,

    /**
     * Indicates that the number of parallel requests exceeded the number of allowed
     * concurrent transcriptions for the subscription.
     */
    TooManyRequests,

    /**
     * Indicates a connection error.
     */
    ConnectionFailure,

    /**
     * Indicates a time-out error when waiting for response from service.
     */
    ServiceTimeout,

    /**
     * Indicates that an error is returned by the service.
     */
    ServiceError,

    /**
     * Indicates an unexpected runtime error.
     */
    RuntimeError,
}
