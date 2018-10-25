//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import { ITranslations } from "../Exports";
import { TranslationStatus } from "../TranslationStatus";

// translation.hypothesis
export interface ITranslationHypothesis {
    Duration: number;
    Offset: number;
    Text: string;
    Translation: ITranslations;
}

export class TranslationHypothesis implements ITranslationHypothesis {
    private translationHypothesis: ITranslationHypothesis;

    private constructor(json: string) {
        this.translationHypothesis = JSON.parse(json);
        this.translationHypothesis.Translation.TranslationStatus = (TranslationStatus as any)[this.translationHypothesis.Translation.TranslationStatus];
    }

    public static FromJSON(json: string): TranslationHypothesis {
        return new TranslationHypothesis(json);
    }

    public get Duration(): number {
        return this.translationHypothesis.Duration;
    }

    public get Offset(): number {
        return this.translationHypothesis.Offset;
    }

    public get Text(): string {
        return this.translationHypothesis.Text;
    }

    public get Translation(): ITranslations {
        return this.translationHypothesis.Translation;
    }
}
