// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { CancellationErrorCode, CancellationReason, IntentRecognitionEventArgs, IntentRecognitionResult } from "./Exports";

/**
 * Define payload of intent recognition canceled result events.
 * @class IntentRecognitionCanceledEventArgs
 */
export class IntentRecognitionCanceledEventArgs extends IntentRecognitionEventArgs {
    private privReason: CancellationReason;
    private privErrorDetails: string;
    private privErrorCode: CancellationErrorCode;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} result - The result of the intent recognition.
     * @param {string} offset - The offset.
     * @param {IntentRecognitionResult} sessionId - The session id.
     */
    public constructor(reason: CancellationReason, errorDetails: string, errorCode: CancellationErrorCode, result?: IntentRecognitionResult, offset?: number, sessionId?: string) {
        super(result, offset, sessionId);

        this.privReason = reason;
        this.privErrorDetails = errorDetails;
        this.privErrorCode = errorCode;
    }

    /**
     * The reason the recognition was canceled.
     * @member IntentRecognitionCanceledEventArgs.prototype.reason
     * @function
     * @public
     * @returns {CancellationReason} Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privReason;
    }

    /**
     * The error code in case of an unsuccessful recognition.
     * Added in version 1.1.0.
     * @return An error code that represents the error reason.
     */
    public get errorCode(): CancellationErrorCode {
        return this.privErrorCode;
    }

    /**
     * In case of an unsuccessful recognition, provides details of the occurred error.
     * @member IntentRecognitionCanceledEventArgs.prototype.errorDetails
     * @function
     * @public
     * @returns {string} A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
