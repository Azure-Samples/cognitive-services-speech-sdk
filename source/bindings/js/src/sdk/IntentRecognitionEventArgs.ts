//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { IntentRecognitionResult } from "./Exports";

/**
 * Intent recognition result event arguments.
 * @class
 */
export class IntentRecognitionEventArgs {
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
}
