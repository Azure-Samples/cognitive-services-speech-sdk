//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import { CancellationReason, RecognitionEventArgs } from "./Exports";

/**
 * Defines content of a RecognitionErrorEvent.
 * @class SpeechRecognitionCanceledEventArgs
 */
export class SpeechRecognitionCanceledEventArgs extends RecognitionEventArgs {
    private privReason: CancellationReason;
    private privErrorDetails: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} reason - The cancellation reason.
     * @param {string} errorDetails - Error details, if provided.
     * @param {number} offset - The offset.
     * @param {string} sessionId - The session id.
     */
    public constructor(reason: CancellationReason, errorDetails: string, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privReason = reason;
        this.privErrorDetails = errorDetails;
    }

    /**
     * The reason the recognition was canceled.
     * @member SpeechRecognitionCanceledEventArgs.prototype.reason
     * @function
     * @public
     * @return Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privReason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @member SpeechRecognitionCanceledEventArgs.prototype.errorDetails
     * @function
     * @public
     * @return A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
