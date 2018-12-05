// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { CancellationReason, ResultReason } from "../sdk/Exports";
import { RecognitionStatus } from "./Exports";

export class EnumTranslation {
    public static implTranslateRecognitionResult(recognitionStatus: RecognitionStatus): ResultReason {
        let reason = ResultReason.Canceled;
        switch (recognitionStatus) {
            case RecognitionStatus.Success:
                reason = ResultReason.RecognizedSpeech;
                break;
            case RecognitionStatus.NoMatch:
            case RecognitionStatus.InitialSilenceTimeout:
            case RecognitionStatus.BabbleTimeout:
            case RecognitionStatus.EndOfDictation:
                reason = ResultReason.NoMatch;
                break;
            case RecognitionStatus.Error:
            default:
                reason = ResultReason.Canceled;
                break;
        }

        return reason;
    }

    public static implTranslateCancelResult(recognitionStatus: RecognitionStatus): CancellationReason {
        let reason = CancellationReason.EndOfStream;
        switch (recognitionStatus) {
            case RecognitionStatus.Success:
            case RecognitionStatus.EndOfDictation:
            case RecognitionStatus.NoMatch:
                reason = CancellationReason.EndOfStream;
                break;
            case RecognitionStatus.InitialSilenceTimeout:
            case RecognitionStatus.BabbleTimeout:
            case RecognitionStatus.Error:
            default:
                reason = CancellationReason.Error;
                break;
        }
        return reason;
    }
}
