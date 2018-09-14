//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import {
    IntentRecognitionResult,
    NoMatchReason,
    SpeechRecognitionResult,
    TranslationTextResult,
} from "./Exports";

/**
 * Contains detailed information for NoMatch recognition results.
 */
export class NoMatchDetails {
    private reason: NoMatchReason;
    /**
     * Creates an instance of NoMatchDetails object for the NoMatch SpeechRecognitionResults.
     * @param The recognition result that was not recognized.
     * @return The no match details object being created.
     */
    public static fromResult(result: SpeechRecognitionResult | IntentRecognitionResult | TranslationTextResult): NoMatchDetails {
        throw new Error("NYI");
    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public get Reason(): NoMatchReason {
        return this.reason;
    }
}
