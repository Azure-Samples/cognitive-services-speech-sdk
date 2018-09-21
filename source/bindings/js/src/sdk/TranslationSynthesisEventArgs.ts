//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { SessionEventArgs, TranslationSynthesisResult } from "./Exports";

/**
 * Translation Synthesis event arguments
 * @class TranslationSynthesisEventArgs
 */
export class TranslationSynthesisEventArgs extends SessionEventArgs {
    private privResult: TranslationSynthesisResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {TranslationSynthesisResult} result - The translation synthesis result.
     * @param {string} sessionId - The session id.
     */
    public constructor(result: TranslationSynthesisResult, sessionId?: string) {
        super(sessionId);

        this.privResult = result;
    }

    /**
     * Specifies the translation synthesis result.
     * @member TranslationSynthesisEventArgs.prototype.result
     * @function
     * @public
     * @returns {TranslationSynthesisResult} Specifies the translation synthesis result.
     */
    public get result(): TranslationSynthesisResult {
        return this.privResult;
    }
}
