//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { IntentRecognitionResult, ISpeechProperties } from "./Exports";

/**
 * Intent recognition result event arguments.
 * @class
 */
export class IntentRecognitionResultEventArgs {
    /**
     * Represents the intent recognition result.
     * @property
     * @returns Represents the intent recognition result.
     */
    public result: IntentRecognitionResult;

    /**
     * A String represents the session identifier.
     * @property
     * @returns A String represents the session identifier.
     */
    public sessionId: string;

    /**
     * A set of properties.
     * @property
     * @returns A set of properties assoicated with this event.
     */
    public properties: ISpeechProperties;
}
