// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    RecognitionStatus,
} from "../Exports";

// speech.phrase
export interface ISimpleSpeechPhrase {
    RecognitionStatus: RecognitionStatus;
    DisplayText: string;
    Offset?: number;
    Duration?: number;
}

export class SimpleSpeechPhrase implements ISimpleSpeechPhrase {
    private simpleSpeechPhrase: ISimpleSpeechPhrase;
    private constructor(json: string) {
        this.simpleSpeechPhrase = JSON.parse(json);
        this.simpleSpeechPhrase.RecognitionStatus = (RecognitionStatus as any)[this.simpleSpeechPhrase.RecognitionStatus];
    }

    public static FromJSON(json: string): SimpleSpeechPhrase {
        return new SimpleSpeechPhrase(json);
    }

    public get RecognitionStatus(): RecognitionStatus {
        return this.simpleSpeechPhrase.RecognitionStatus;
    }
    public get DisplayText(): string {
        return this.simpleSpeechPhrase.DisplayText;
    }
    public get Offset(): number {
        return this.simpleSpeechPhrase.Offset;
    }
    public get Duration(): number {
        return this.simpleSpeechPhrase.Duration;
    }
}
