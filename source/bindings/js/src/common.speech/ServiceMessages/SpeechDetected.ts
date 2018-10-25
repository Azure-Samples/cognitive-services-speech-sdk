//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

// speech.endDetected
export interface ISpeechDetected {
    Offset: number;
}

export class SpeechDetected implements ISpeechDetected {
    private speechStartDetected: ISpeechDetected;
    private constructor(json: string) {
        this.speechStartDetected = JSON.parse(json);
    }

    public static FromJSON(json: string): SpeechDetected {
        return new SpeechDetected(json);
    }

    public get Offset(): number {
        return this.speechStartDetected.Offset;
    }
}
