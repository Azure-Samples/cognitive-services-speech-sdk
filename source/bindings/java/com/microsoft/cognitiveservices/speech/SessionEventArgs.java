package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;


/**
 * Defines content for session events like SessionStarted/Stopped, SoundStarted/Stopped.
 */
public final class SessionEventArgs // extends EventArgs
{
    /**
      * Constructs a SessionEcentArgs.
      * @param type the SessionEventType
      * @param arg The native SessionEventArgs
      */
    public SessionEventArgs(SessionEventType type, com.microsoft.cognitiveservices.speech.internal.SessionEventArgs arg) {
        Contracts.throwIfNull(arg, "arg");

        this._sessionId = arg.getSessionId();
        this._eventType = type;

        Contracts.throwIfNull(this._sessionId, "SessionId");
    }

    /**
      * Represents the event type.
      * @return Represents the event type.
      */
    public SessionEventType getEventType() {
        return _eventType;
    }
    private final SessionEventType _eventType;

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
        return "EventType: " + _eventType.toString() +
               " SessionId: " + _sessionId.toString() +
               ".";
    }
}
