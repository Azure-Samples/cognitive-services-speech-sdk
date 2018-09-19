//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { IntentRecognitionResult, PropertyCollection, RecognitionEventArgs, RecognitionResult, ResultReason } from "./Exports";

/**
 * Intent recognition result event arguments.
 * @class
 */
export class IntentRecognitionEventArgs extends RecognitionEventArgs {
    private privResult: IntentRecognitionResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result The result of the intent recognition.
     * @param offset The offset.
     * @param sessionId The session id.
     */
    public constructor(result: IntentRecognitionResult, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privResult = result;
    }

    /**
     * Represents the intent recognition result.
     * @property
     * @returns Represents the intent recognition result.
     */
    public get result(): IntentRecognitionResult {
        return this.privResult;
    }
}
