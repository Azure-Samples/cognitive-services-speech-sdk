//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import {
    CancellationReason,
    IntentRecognitionResult,
    SpeechRecognitionResult,
    TranslationTextResult,
} from "./Exports";

/**
 * Contains detailed information about why a result was canceled.
 */
export class CancellationDetails {

    private reason: CancellationReason;
    private errorDetails: string;

    /**
     * Creates an instance of CancellationDetails object for the canceled SpeechRecognitionResult.
     * @param result The result that was canceled.
     * @return The cancellation details object being created.
     */
    public static fromResult(result: SpeechRecognitionResult | IntentRecognitionResult | TranslationTextResult): CancellationDetails {
        throw new Error("NYI");
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
