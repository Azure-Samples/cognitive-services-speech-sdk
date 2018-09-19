//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import {
    ISimpleSpeechPhrase,
    RecognitionStatus2,
} from "../common.speech/Exports";
import {
    CancellationReason,
    RecognitionResult,
} from "./Exports";

/**
 * Contains detailed information about why a result was canceled.
 */
export class CancellationDetails {

    private reason: CancellationReason;
    private errorDetails: string;

    private constructor(reason: CancellationReason, errorDetails: string) {
        this.reason = reason;
        this.errorDetails = errorDetails;
    }

    /**
     * Creates an instance of CancellationDetails object for the canceled RecognitionResult.
     * @param result The result that was canceled.
     * @return The cancellation details object being created.
     */
    public static fromResult(result: RecognitionResult): CancellationDetails {
        const simpleSpeech: ISimpleSpeechPhrase = JSON.parse(result.json);

        let reason = CancellationReason.EndOfStream;
        const recognitionStatus2: string = "" + simpleSpeech.RecognitionStatus;
        const recstatus2 = (RecognitionStatus2 as any)[recognitionStatus2];
        switch (recstatus2) {
            case RecognitionStatus2.Success:
            case RecognitionStatus2.EndOfDictation:
            case RecognitionStatus2.NoMatch:
                reason = CancellationReason.EndOfStream;
                break;
            case RecognitionStatus2.InitialSilenceTimeout:
            case RecognitionStatus2.BabbleTimeout:
            case RecognitionStatus2.Error:
            default:
                reason = CancellationReason.Error;
                break;
        }

        return new CancellationDetails(reason, result.errorDetails);

    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public get Reason(): CancellationReason {
        return this.reason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @return A String that represents the error details.
     */
    public get ErrorDetails(): string {
        return this.errorDetails;
    }
}
