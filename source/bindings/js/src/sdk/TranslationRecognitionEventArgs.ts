//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { RecognitionEventArgs, TranslationRecognitionResult } from "./Exports";

/**
 * Translation text result event arguments.
 * @class TranslationRecognitionEventArgs
 */
export class TranslationRecognitionEventArgs extends RecognitionEventArgs {
    private privResult: TranslationRecognitionResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result - The translation recognition result.
     * @param offset - The offset.
     * @param sessionId - The session id.
     */
    public constructor(result: TranslationRecognitionResult, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privResult = result;
    }

    /**
     * Specifies the recognition result.
     * @member TranslationRecognitionEventArgs.prototype.result
     * @returns the recognition result.
     */
    public get result(): TranslationRecognitionResult {
        return this.privResult;
    }
}
