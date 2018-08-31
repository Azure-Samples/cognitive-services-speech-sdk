//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { TranslationSynthesisResult } from "./Exports";

export class TranslationSynthesisResultEventArgs {
    /**
     * Specifies the translation synthesis result.
     * @return Specifies the translation synthesis result.
     */
    public result: TranslationSynthesisResult;

    /**
     * Specifies the session identifier.
     * @return Specifies the session identifier.
     */
    public sessionId: string;
}
