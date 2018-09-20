//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    CancellationReason,
    IntentRecognitionEventArgs,
    IntentRecognitionResult,
    PropertyCollection,
    ResultReason,
} from "./Exports";

/**
 * Define payload of intent recognition canceled result events.
 * @class IntentRecognitionCanceledEventArgs
 */
export class IntentRecognitionCanceledEventArgs extends IntentRecognitionEventArgs {
    private privReason: CancellationReason;
    private privErrorDetails: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} result - The result of the intent recognition.
     * @param {string} offset - The offset.
     * @param {IntentRecognitionResult} sessionId - The session id.
     */
    public constructor(reason: CancellationReason, errorDetails: string, result?: IntentRecognitionResult, offset?: number, sessionId?: string) {
        super(result, offset, sessionId);

        this.privReason = reason;
        this.privErrorDetails = errorDetails;
    }

    /**
     * The reason the recognition was canceled.
     * @member IntentRecognitionCanceledEventArgs.prototype.reason
     * @return Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privReason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @member IntentRecognitionCanceledEventArgs.prototype.errorDetails
     * @return A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
