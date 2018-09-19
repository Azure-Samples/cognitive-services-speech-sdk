//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { SessionEventArgs, TranslationSynthesisResult } from "./Exports";

/**
 * Translation Synthesis result event arguments
 * @class
 */
export class TranslationSynthesisResultEventArgs extends SessionEventArgs {
    /**
     * Specifies the translation synthesis result.
     * @property
     * @returns Specifies the translation synthesis result.
     */
    public result: TranslationSynthesisResult;
}
