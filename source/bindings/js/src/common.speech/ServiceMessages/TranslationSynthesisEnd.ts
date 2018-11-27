// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { SynthesisStatus } from "../Exports";

// translation.synthesis.end
export interface ITranslationSynthesisEnd {
    SynthesisStatus: SynthesisStatus;
    FailureReason: string;
}

export class TranslationSynthesisEnd implements ITranslationSynthesisEnd {
    private privSynthesisEnd: ITranslationSynthesisEnd;

    private constructor(json: string) {
        this.privSynthesisEnd = JSON.parse(json);
        this.privSynthesisEnd.SynthesisStatus = (SynthesisStatus as any)[this.privSynthesisEnd.SynthesisStatus];
    }

    public static fromJSON(json: string): TranslationSynthesisEnd {
        return new TranslationSynthesisEnd(json);
    }

    public get SynthesisStatus(): SynthesisStatus {
        return this.privSynthesisEnd.SynthesisStatus;
    }

    public get FailureReason(): string {
        return this.privSynthesisEnd.FailureReason;
    }
}
