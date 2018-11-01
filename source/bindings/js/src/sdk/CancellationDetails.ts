//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    CancellationErrorCodePropertyName,
    EnumTranslation,
    SimpleSpeechPhrase,
} from "../common.speech/Exports";
import {
    CancellationErrorCode,
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
    private privErrorCode: CancellationErrorCode;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} reason - The cancellation reason.
     * @param {string} errorDetails - The error details, if provided.
     */
    private constructor(reason: CancellationReason, errorDetails: string, errorCode: CancellationErrorCode) {
        this.privReason = reason;
        this.privErrorDetails = errorDetails;
        this.privErrorCode = errorCode;
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
        let errorCode: CancellationErrorCode = CancellationErrorCode.NoError;

        if (!!result.json) {
            const simpleSpeech: SimpleSpeechPhrase = SimpleSpeechPhrase.FromJSON(result.json);
            reason = EnumTranslation.implTranslateCancelResult(simpleSpeech.RecognitionStatus);
        }

        if (!!result.properties) {
            errorCode = (CancellationErrorCode as any)[result.properties.getProperty(CancellationErrorCodePropertyName, CancellationErrorCode[CancellationErrorCode.NoError])];
        }

        return new CancellationDetails(reason, result.errorDetails, errorCode);

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

    /**
     * The error code of why the cancellation occurred.
     * @return An error code that represents the error reason.
     * Added in version 1.1.0.
     */
    public get ErrorCode(): CancellationErrorCode {
        return this.privErrorCode;
    }

}
