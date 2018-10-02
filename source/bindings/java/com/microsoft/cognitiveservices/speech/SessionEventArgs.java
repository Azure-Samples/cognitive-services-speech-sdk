//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import com.microsoft.cognitiveservices.speech.util.Contracts;


/**
 * Defines payload for SessionStarted/Stopped events.
 */
public class SessionEventArgs // extends EventArgs
{
    /**
     * Constructs a SessionEventArgs object.
     * @param arg The native SessionEventArgs
     */
    public SessionEventArgs(com.microsoft.cognitiveservices.speech.internal.SessionEventArgs arg) {
        Contracts.throwIfNull(arg, "arg");

        this._sessionId = arg.getSessionId();

        Contracts.throwIfNull(this._sessionId, "SessionId");
    }

    /**
     * Represents the session identifier.
     * @return Represents the session identifier.
     */
    public String getSessionId() {
        return _sessionId;
    }

    private final String _sessionId;

    /**
     * Returns a String that represents the session event.
     * @return a String that represents the session event.
     */
    @Override
    public String toString() {
        return "SessionId: " + _sessionId.toString() + ".";
    }
}
