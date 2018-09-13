//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { RecognitionEventType } from "./RecognitionEventType";

/**
 * Defines payload for session events like Speech Start/End Detected
 * @class
 */
export class RecognitionEventArgs {
    /**
     * Represents the session identifier.
     * @property
     */
    public sessionId: string;

    /**
     * Represents the message offset
     * @property
     */
    public offset: number;
}
