// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// speech.hypothesis
export interface ISpeechHypothesis {
    Text: string;
    Offset: number;
    Duration: number;
}

export class SpeechHypothesis implements ISpeechHypothesis {
    private privSpeechHypothesis: ISpeechHypothesis;

    private constructor(json: string) {
        this.privSpeechHypothesis = JSON.parse(json);
    }

    public static fromJSON(json: string): SpeechHypothesis {
        return new SpeechHypothesis(json);
    }

    public get Text(): string {
        return this.privSpeechHypothesis.Text;
    }

    public get Offset(): number {
        return this.privSpeechHypothesis.Offset;
    }

    public get Duration(): number {
        return this.privSpeechHypothesis.Duration;
    }
}
