//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { TranslationTextResult } from "./Exports";

/**
 * Translation text result event arguments.
 * @class
 */
export class TranslationTextResultEventArgs {
    /**
     * Specifies the recognition result.
     * @property
     * @returns the recognition result.
     */
    public result: TranslationTextResult;

    /**
     * Specifies the session identifier.
     * @property
     * @returns the session identifier.
     */
    public sessionId: string;
}
