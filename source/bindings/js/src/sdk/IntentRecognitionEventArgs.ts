// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { IntentRecognitionResult, RecognitionEventArgs } from "./Exports";

/**
 * Intent recognition result event arguments.
 * @class
 */
export class IntentRecognitionEventArgs extends RecognitionEventArgs {
    private privResult: IntentRecognitionResult;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result - The result of the intent recognition.
     * @param offset - The offset.
     * @param sessionId - The session id.
     */
    public constructor(result: IntentRecognitionResult, offset?: number, sessionId?: string) {
        super(offset, sessionId);

        this.privResult = result;
    }

    /**
     * Represents the intent recognition result.
     * @member IntentRecognitionEventArgs.prototype.result
     * @function
     * @public
     * @returns {IntentRecognitionResult} Represents the intent recognition result.
     */
    public get result(): IntentRecognitionResult {
        return this.privResult;
    }
}
