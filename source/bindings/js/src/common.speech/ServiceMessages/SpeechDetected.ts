// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

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
