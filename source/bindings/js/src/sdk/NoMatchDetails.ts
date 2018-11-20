// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    RecognitionStatus,
    SimpleSpeechPhrase,
} from "../../src/common.speech/Exports";
import {
    IntentRecognitionResult,
    NoMatchReason,
    SpeechRecognitionResult,
    TranslationRecognitionResult,
} from "./Exports";

/**
 * Contains detailed information for NoMatch recognition results.
 * @class NoMatchDetails
 */
export class NoMatchDetails {
    private privReason: NoMatchReason;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {NoMatchReason} reason - The no-match reason.
     */
    private constructor(reason: NoMatchReason) {
        this.privReason = reason;
    }

    /**
     * Creates an instance of NoMatchDetails object for the NoMatch SpeechRecognitionResults.
     * @member NoMatchDetails.fromResult
     * @function
     * @public
     * @param {SpeechRecognitionResult | IntentRecognitionResult | TranslationRecognitionResult} result - The recognition result that was not recognized.
     * @returns {NoMatchDetails} The no match details object being created.
     */
    public static fromResult(result: SpeechRecognitionResult | IntentRecognitionResult | TranslationRecognitionResult): NoMatchDetails {
        const simpleSpeech: SimpleSpeechPhrase = SimpleSpeechPhrase.FromJSON(result.json);

        let reason: NoMatchReason = NoMatchReason.NotRecognized;

        switch (simpleSpeech.RecognitionStatus) {
            case RecognitionStatus.BabbleTimeout:
                reason = NoMatchReason.InitialBabbleTimeout;
                break;
            case RecognitionStatus.InitialSilenceTimeout:
                reason = NoMatchReason.InitialSilenceTimeout;
                break;
            default:
                reason = NoMatchReason.NotRecognized;
                break;
        }

        return new NoMatchDetails(reason);
    }

    /**
     * The reason the recognition was canceled.
     * @member NoMatchDetails.prototype.reason
     * @function
     * @public
     * @returns {NoMatchReason} Specifies the reason canceled.
     */
    public get reason(): NoMatchReason {
        return this.privReason;
    }
}
