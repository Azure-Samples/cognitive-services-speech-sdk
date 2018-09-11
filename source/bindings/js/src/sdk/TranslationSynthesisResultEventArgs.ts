//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { TranslationSynthesisResult } from "./Exports";

/**
 * Translation Synthesis result event arguments
 * @class
 */
export class TranslationSynthesisResultEventArgs {
    /**
     * Specifies the translation synthesis result.
     * @property
     * @returns Specifies the translation synthesis result.
     */
    public result: TranslationSynthesisResult;

    /**
     * Specifies the session identifier.
     * @property
     * @returns Specifies the session identifier.
     */
    public sessionId: string;
}
