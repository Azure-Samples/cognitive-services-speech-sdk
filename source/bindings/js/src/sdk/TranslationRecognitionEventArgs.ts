//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { RecognitionEventArgs, TranslationRecognitionResult } from "./Exports";

/**
 * Translation text result event arguments.
 * @class
 */
export class TranslationRecognitionEventArgs extends RecognitionEventArgs {
    private privResult: TranslationRecognitionResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result The translation recognition result.
     * @param offset The offset.
     * @param sessionId The session id.
     */
    public constructor(result: TranslationRecognitionResult, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privResult = result;
    }

    /**
     * Specifies the recognition result.
     * @property
     * @returns the recognition result.
     */
    public get result(): TranslationRecognitionResult {
        return this.privResult;
    }
}
