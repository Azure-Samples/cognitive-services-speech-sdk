//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    RecognitionStatus,
} from "../Exports";

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
    private detailedSpeechPhrase: IDetailedSpeechPhrase;
    private constructor(json: string) {
        this.detailedSpeechPhrase = JSON.parse(json);
        this.detailedSpeechPhrase.RecognitionStatus = (RecognitionStatus as any)[this.detailedSpeechPhrase.RecognitionStatus];
    }

    public static FromJSON(json: string): DetailedSpeechPhrase {
        return new DetailedSpeechPhrase(json);
    }

    public get RecognitionStatus(): RecognitionStatus {
        return this.detailedSpeechPhrase.RecognitionStatus;
    }
    public get NBest(): IPhrase[] {
        return this.detailedSpeechPhrase.NBest;
    }
    public get Duration(): number {
        return this.detailedSpeechPhrase.Duration;
    }
    public get Offset(): number {
        return this.detailedSpeechPhrase.Offset;
    }
}
