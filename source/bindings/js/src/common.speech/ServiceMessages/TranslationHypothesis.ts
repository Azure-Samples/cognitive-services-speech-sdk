// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

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
    private privTranslationHypothesis: ITranslationHypothesis;

    private constructor(json: string) {
        this.privTranslationHypothesis = JSON.parse(json);
        this.privTranslationHypothesis.Translation.TranslationStatus = (TranslationStatus as any)[this.privTranslationHypothesis.Translation.TranslationStatus];
    }

    public static fromJSON(json: string): TranslationHypothesis {
        return new TranslationHypothesis(json);
    }

    public get Duration(): number {
        return this.privTranslationHypothesis.Duration;
    }

    public get Offset(): number {
        return this.privTranslationHypothesis.Offset;
    }

    public get Text(): string {
        return this.privTranslationHypothesis.Text;
    }

    public get Translation(): ITranslations {
        return this.privTranslationHypothesis.Translation;
    }
}
