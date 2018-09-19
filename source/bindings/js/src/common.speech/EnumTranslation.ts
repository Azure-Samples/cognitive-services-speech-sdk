//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import {
    CancellationReason,
    ResultReason,
} from "../sdk/Exports";
import {
    RecognitionStatus2,
} from "./Exports";

export class EnumTranslation {
    public static implTranslateRecognitionResult(recognitionStatus: RecognitionStatus2): ResultReason {
        let reason = ResultReason.Canceled;
        const recognitionStatus2: string = "" + recognitionStatus;
        const recstatus2 = (RecognitionStatus2 as any)[recognitionStatus2];
        switch (recstatus2) {
            case RecognitionStatus2.Success:
                reason = ResultReason.RecognizedSpeech;
                break;
            case RecognitionStatus2.NoMatch:
            case RecognitionStatus2.InitialSilenceTimeout:
            case RecognitionStatus2.BabbleTimeout:
                reason = ResultReason.NoMatch;
                break;
            case RecognitionStatus2.EndOfDictation:
            case RecognitionStatus2.Error:
            default:
                reason = ResultReason.Canceled;
                break;
        }

        return reason;
    }

    public static implTranslateCancelResult(recognitionStatus: RecognitionStatus2): CancellationReason {
        let reason = CancellationReason.EndOfStream;
        const recognitionStatus2: string = "" + recognitionStatus;
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
        return reason;
    }
}
