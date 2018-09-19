//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { PropertyCollection, RecognitionResult, ResultReason } from "./Exports";

/**
 * Defines result of speech recognition.
 * @class
 */
export class SpeechRecognitionResult extends RecognitionResult {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param resultId The result id.
     * @param reason The reason.
     * @param text The recognized text.
     * @param duration The duration.
     * @param offset The offset into the stream.
     * @param errorDetails Error details, if provided.
     * @param json Additional Json, if provided.
     * @param properties Additional properties, if provided.
     */
    public constructor(resultId?: string, reason?: ResultReason, text?: string, duration?: number, offset?: number, errorDetails?: string, json?: string, properties?: PropertyCollection) {
        super(resultId, reason, text, duration, offset, errorDetails, json, properties);
    }
}
