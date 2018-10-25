//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    ITranslations,
    RecognitionStatus,
} from "../Exports";
import { TranslationStatus } from "../TranslationStatus";

// translation.phrase
export interface ITranslationPhrase {
    RecognitionStatus: RecognitionStatus;
    Offset: number;
    Duration: number;
    Text: string;
    Translation: ITranslations;
}

export class TranslationPhrase implements ITranslationPhrase {
    private translationPhrase: ITranslationPhrase;

    private constructor(json: string) {
        this.translationPhrase = JSON.parse(json);
        this.translationPhrase.RecognitionStatus = (RecognitionStatus as any)[this.translationPhrase.RecognitionStatus];
        if (this.translationPhrase.Translation !== undefined) {
            this.translationPhrase.Translation.TranslationStatus = (TranslationStatus as any)[this.translationPhrase.Translation.TranslationStatus];
        }
    }

    public static FromJSON(json: string): TranslationPhrase {
        return new TranslationPhrase(json);
    }

    public get RecognitionStatus(): RecognitionStatus {
        return this.translationPhrase.RecognitionStatus;
    }
    public get Offset(): number {
        return this.translationPhrase.Offset;
    }
    public get Duration(): number {
        return this.translationPhrase.Duration;
    }
    public get Text(): string {
        return this.translationPhrase.Text;
    }
    public get Translation(): ITranslations {
        return this.translationPhrase.Translation;
    }
}
