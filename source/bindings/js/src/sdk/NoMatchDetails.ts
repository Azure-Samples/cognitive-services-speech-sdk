//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    ISimpleSpeechPhrase,
    RecognitionStatus2,
} from "../../src/common.speech/Exports";
import {
    IntentRecognitionResult,
    NoMatchReason,
    SpeechRecognitionResult,
    TranslationRecognitionResult,
} from "./Exports";
import { RecognitionResult } from "./RecognitionResult";

/**
 * Contains detailed information for NoMatch recognition results.
 */
export class NoMatchDetails {
    private privReason: NoMatchReason;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param reason The no-match reason.
     */
    private constructor(reason: NoMatchReason) {
        this.privReason = reason;
    }

    /**
     * Creates an instance of NoMatchDetails object for the NoMatch SpeechRecognitionResults.
     * @member
     * @param The recognition result that was not recognized.
     * @return The no match details object being created.
     */
    public static fromResult(result: SpeechRecognitionResult | IntentRecognitionResult | TranslationRecognitionResult): NoMatchDetails {
        const simpleSpeech: ISimpleSpeechPhrase = JSON.parse(result.json);

        let reason: NoMatchReason = NoMatchReason.NotRecognized;

        const realReason = (RecognitionStatus2 as any)[simpleSpeech.RecognitionStatus];

        switch (realReason) {
            case RecognitionStatus2.BabbleTimeout:
                reason = NoMatchReason.InitialBabbleTimeout;
                break;
            case RecognitionStatus2.InitialSilenceTimeout:
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
     * @property
     * @return Specifies the reason canceled.
     */
    public get reason(): NoMatchReason {
        return this.privReason;
    }
}
