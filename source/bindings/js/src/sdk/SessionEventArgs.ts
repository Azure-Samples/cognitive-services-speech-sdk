//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines content for session events like SessionStarted/Stopped, SoundStarted/Stopped.
 * @class
 */
export class SessionEventArgs {
    private privSessionId: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param sessionId The session id.
     */
    public constructor(sessionId: string) {
        this.privSessionId = sessionId;
    }

    /**
     * Represents the session identifier.
     * @returns Represents the session identifier.
     */
    public get sessionId(): string {
        return this.privSessionId;
    }
}
