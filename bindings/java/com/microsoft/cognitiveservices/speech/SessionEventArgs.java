package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines payload for session events like SessionStarted/Stopped, SoundStarted/Stopped.
 */
public class SessionEventArgs // extends EventArgs
{
    public SessionEventArgs(SessionEventType type,  com.microsoft.cognitiveservices.speech.internal.SessionEventArgs arg)
    {
        this.sessionId = arg.getSessionId();
        this.eventType = type;
    }

    /**
      * Represents the event type.
      */
    public final SessionEventType eventType;// { get; }

    /**
      * Represents the seesion identifier.
      */
    public final String sessionId; // { get; }

    /**
     * Returns a String that represents the session event.
     */
    @Override
    public String toString() {
        return "EventType: " + eventType.toString() + " SessionId: " + sessionId.toString() + ".";
    }
}
