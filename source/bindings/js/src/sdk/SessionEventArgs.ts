//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Defines content for session events like SessionStarted/Stopped, SoundStarted/Stopped.
 * @class SessionEventArgs
 */
export class SessionEventArgs {
    private privSessionId: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {string} sessionId - The session id.
     */
    public constructor(sessionId: string) {
        this.privSessionId = sessionId;
    }

    /**
     * Represents the session identifier.
     * @member SessionEventArgs.prototype.sessionId
     * @function
     * @public
     * @returns Represents the session identifier.
     */
    public get sessionId(): string {
        return this.privSessionId;
    }
}
