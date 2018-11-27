// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// speech.endDetected
export interface ISpeechDetected {
    Offset: number;
}

export class SpeechDetected implements ISpeechDetected {
    private privSpeechStartDetected: ISpeechDetected;

    private constructor(json: string) {
        this.privSpeechStartDetected = JSON.parse(json);
    }

    public static fromJSON(json: string): SpeechDetected {
        return new SpeechDetected(json);
    }

    public get Offset(): number {
        return this.privSpeechStartDetected.Offset;
    }
}
