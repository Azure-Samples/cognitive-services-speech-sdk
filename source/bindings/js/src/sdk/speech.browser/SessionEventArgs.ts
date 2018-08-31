//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { SessionEventType } from "./SessionEventType";

/**
 * Defines content for session events like SessionStarted/Stopped, SoundStarted/Stopped.
 */
export class SessionEventArgs {
    /**
     * Represents the event type.
     * @return Represents the event type.
     */
    public eventType: SessionEventType;

    /**
     * Represents the session identifier.
     * @return Represents the session identifier.
     */
    public SessionId: string;
}
