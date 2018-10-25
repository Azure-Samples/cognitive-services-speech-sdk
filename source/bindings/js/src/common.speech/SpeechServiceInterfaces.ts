//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import { RecognitionCompletionStatus } from "../../src/common.speech/Exports";
import { TranslationStatus } from "./TranslationStatus";

export interface ITranslations {
    TranslationStatus: TranslationStatus;
    Translations: ITranslation[];
    FailureReason: string;
}

export interface ITranslation {
    Language: string;
    Text: string;
}

export interface ISpeechEndDetectedResult {
    Offset?: number;
}

// turn.start
export interface ITurnStart {
    context: ITurnStartContext;
}

export interface ITurnStartContext {
    serviceTag: string;
}

export interface IResultErrorDetails {
    errorText: string;
    recogSate: RecognitionCompletionStatus;
}
