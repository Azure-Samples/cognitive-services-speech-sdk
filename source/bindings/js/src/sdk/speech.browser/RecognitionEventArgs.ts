//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { RecognitionEventType } from "./RecognitionEventType";

/**
 * Defines payload for session events like Speech Start/End Detected
 */
export class RecognitionEventArgs {
    /**
     * Represents the event type.
     */
    public eventType: RecognitionEventType;

    /**
     * Represents the session identifier.
     */
    public sessionId: string;

    /**
     * Represents the message offset
     */
    public offset: number;
}
