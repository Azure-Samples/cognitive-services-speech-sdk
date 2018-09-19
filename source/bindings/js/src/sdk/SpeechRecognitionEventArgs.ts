//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { PropertyCollection, RecognitionEventArgs, ResultReason, SpeechRecognitionResult } from "./Exports";

/**
 * Defines contents of speech recognizing/recognized event.
 * @class
 */
export class SpeechRecognitionEventArgs extends RecognitionEventArgs {
    private privResult: SpeechRecognitionResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result The speech recognition result.
     * @param offset The offset.
     * @param sessionId The session id.
     */
    public constructor(result: SpeechRecognitionResult, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privResult = result;
    }

    /**
     * Specifies the recognition result.
     * @property
     * @returns the recognition result.
     */
    public get result(): SpeechRecognitionResult {
        return this.privResult;
    }
}
