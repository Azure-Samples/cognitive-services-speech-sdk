// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import {
    RecognitionEventArgs,
    SpeechRecognitionResult,
} from "./Exports";

/**
 * Defines contents of speech recognizing/recognized event.
 * @class SpeechRecognitionEventArgs
 */
export class SpeechRecognitionEventArgs extends RecognitionEventArgs {
    private privResult: SpeechRecognitionResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {SpeechRecognitionResult} result - The speech recognition result.
     * @param {number} offset - The offset.
     * @param {string} sessionId - The session id.
     */
    public constructor(result: SpeechRecognitionResult, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privResult = result;
    }

    /**
     * Specifies the recognition result.
     * @member SpeechRecognitionEventArgs.prototype.result
     * @function
     * @public
     * @returns {SpeechRecognitionResult} the recognition result.
     */
    public get result(): SpeechRecognitionResult {
        return this.privResult;
    }
}
