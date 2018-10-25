//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    EnumTranslation,
    SimpleSpeechPhrase,
} from "../common.speech/Exports";
import {
    CancellationReason,
    RecognitionResult,
} from "./Exports";

/**
 * Contains detailed information about why a result was canceled.
 * @class CancellationDetails
 */
export class CancellationDetails {
    private privReason: CancellationReason;
    private privErrorDetails: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} reason - The cancellation reason.
     * @param {string} errorDetails - The error details, if provided.
     */
    private constructor(reason: CancellationReason, errorDetails: string) {
        this.privReason = reason;
        this.privErrorDetails = errorDetails;
    }

    /**
     * Creates an instance of CancellationDetails object for the canceled RecognitionResult.
     * @member CancellationDetails.fromResult
     * @function
     * @public
     * @param {RecognitionResult} result - The result that was canceled.
     * @returns {CancellationDetails} The cancellation details object being created.
     */
    public static fromResult(result: RecognitionResult): CancellationDetails {
        let reason = CancellationReason.Error;

        if (!!result.json) {
            const simpleSpeech: SimpleSpeechPhrase = SimpleSpeechPhrase.FromJSON(result.json);
            reason = EnumTranslation.implTranslateCancelResult(simpleSpeech.RecognitionStatus);
        }

        return new CancellationDetails(reason, result.errorDetails);

    }

    /**
     * The reason the recognition was canceled.
     * @member CancellationDetails.prototype.reason
     * @function
     * @public
     * @returns {CancellationReason} Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privReason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @member CancellationDetails.prototype.errorDetails
     * @function
     * @public
     * @returns {string} A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
