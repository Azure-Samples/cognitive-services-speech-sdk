// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
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
