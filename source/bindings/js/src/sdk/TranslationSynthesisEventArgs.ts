//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { SessionEventArgs, TranslationSynthesisResult } from "./Exports";

/**
 * Translation Synthesis event arguments
 * @class
 */
export class TranslationSynthesisEventArgs extends SessionEventArgs {
    private privResult: TranslationSynthesisResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result The translation synthesis result.
     * @param sessionId The session id.
     */
    public constructor(result: TranslationSynthesisResult, sessionId?: string) {
        super(sessionId);

        this.privResult = result;
    }

    /**
     * Specifies the translation synthesis result.
     * @property
     * @returns Specifies the translation synthesis result.
     */
    public get result(): TranslationSynthesisResult {
        return this.privResult;
    }
}
