// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// speech.hypothesis
export interface ISpeechHypothesis {
    Text: string;
    Offset: number;
    Duration: number;
}

export class SpeechHypothesis implements ISpeechHypothesis {
    private speechHypothesis: ISpeechHypothesis;
    private constructor(json: string) {
        this.speechHypothesis = JSON.parse(json);
    }

    public static FromJSON(json: string): SpeechHypothesis {
        return new SpeechHypothesis(json);
    }
    public get Text(): string {
        return this.speechHypothesis.Text;
    }
    public get Offset(): number {
        return this.speechHypothesis.Offset;
    }
    public get Duration(): number {
        return this.speechHypothesis.Duration;
    }
}
