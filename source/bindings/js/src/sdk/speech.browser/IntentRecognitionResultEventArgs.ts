//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { IntentRecognitionResult, ISpeechProperties } from "./Exports";

export class IntentRecognitionResultEventArgs {
    /**
     * Represents the intent recognition result.
     * @return Represents the intent recognition result.
     */
    public result: IntentRecognitionResult;

    /**
     * A String represents the session identifier.
     * @return A String represents the session identifier.
     */
    public sessionId: string;

    public properties: ISpeechProperties;
}
