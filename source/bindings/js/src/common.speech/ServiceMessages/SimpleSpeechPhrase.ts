// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { RecognitionStatus } from "../Exports";

// speech.phrase
export interface ISimpleSpeechPhrase {
    RecognitionStatus: RecognitionStatus;
    DisplayText: string;
    Offset?: number;
    Duration?: number;
}

export class SimpleSpeechPhrase implements ISimpleSpeechPhrase {
    private privSimpleSpeechPhrase: ISimpleSpeechPhrase;

    private constructor(json: string) {
        this.privSimpleSpeechPhrase = JSON.parse(json);
        this.privSimpleSpeechPhrase.RecognitionStatus = (RecognitionStatus as any)[this.privSimpleSpeechPhrase.RecognitionStatus];
    }

    public static fromJSON(json: string): SimpleSpeechPhrase {
        return new SimpleSpeechPhrase(json);
    }

    public get RecognitionStatus(): RecognitionStatus {
        return this.privSimpleSpeechPhrase.RecognitionStatus;
    }

    public get DisplayText(): string {
        return this.privSimpleSpeechPhrase.DisplayText;
    }

    public get Offset(): number {
        return this.privSimpleSpeechPhrase.Offset;
    }

    public get Duration(): number {
        return this.privSimpleSpeechPhrase.Duration;
    }
}
