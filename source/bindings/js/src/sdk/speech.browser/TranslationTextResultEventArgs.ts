//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { TranslationTextResult } from "./Exports";

export class TranslationTextResultEventArgs {
    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public result: TranslationTextResult;

    /**
     * Specifies the session identifier.
     * @return the session identifier.
     */
    public sessionId: string;
}
