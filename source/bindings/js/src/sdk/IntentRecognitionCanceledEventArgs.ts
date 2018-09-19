//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
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
 */
export class IntentRecognitionCanceledEventArgs extends IntentRecognitionEventArgs {
    private privReason: CancellationReason;
    private privErrorDetails: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result The result of the intent recognition.
     * @param offset The offset.
     * @param sessionId The session id.
     */
    public constructor(reason: CancellationReason, errorDetails: string, result?: IntentRecognitionResult, offset?: number, sessionId?: string) {
        super(result, offset, sessionId);

        this.privReason = reason;
        this.privErrorDetails = errorDetails;
    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privReason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @return A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
