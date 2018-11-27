// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { RecognitionStatus } from "../Exports";

// speech.phrase for detailed
export interface IDetailedSpeechPhrase {
    RecognitionStatus: RecognitionStatus;
    NBest: IPhrase[];
    Duration?: number;
    Offset?: number;
}

export interface IPhrase {
    Confidence?: number;
    Lexical: string;
    ITN: string;
    MaskedITN: string;
    Display: string;
}

export class DetailedSpeechPhrase implements IDetailedSpeechPhrase {
    private privDetailedSpeechPhrase: IDetailedSpeechPhrase;

    private constructor(json: string) {
        this.privDetailedSpeechPhrase = JSON.parse(json);
        this.privDetailedSpeechPhrase.RecognitionStatus = (RecognitionStatus as any)[this.privDetailedSpeechPhrase.RecognitionStatus];
    }

    public static fromJSON(json: string): DetailedSpeechPhrase {
        return new DetailedSpeechPhrase(json);
    }

    public get RecognitionStatus(): RecognitionStatus {
        return this.privDetailedSpeechPhrase.RecognitionStatus;
    }
    public get NBest(): IPhrase[] {
        return this.privDetailedSpeechPhrase.NBest;
    }
    public get Duration(): number {
        return this.privDetailedSpeechPhrase.Duration;
    }
    public get Offset(): number {
        return this.privDetailedSpeechPhrase.Offset;
    }
}
