package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Defines payload for session events like SessionStarted/Stopped, SoundStarted/Stopped.
/// </summary>
public class SessionEventArgs // extends EventArgs
{
    public SessionEventArgs(SessionEventType type,  com.microsoft.cognitiveservices.speech.internal.SessionEventArgs arg)
    {
        this.sessionId = arg.getSessionId();
        this.eventType = type;
    }

    /// <summary>
    /// Represents the event type.
    /// </summary>
    public final SessionEventType eventType;// { get; }

    /// <summary>
    /// Represents the seesion identifier.
    /// </summary>
    public final String sessionId; // { get; }

    /// <summary>
    /// Returns a String that represents the session event.
    /// </summary>
    /// <returns>A String that represents the session event.</returns>
    @Override
    public String toString() {
        return "EventType: " + eventType.toString() + " SessionId: " + sessionId.toString() + ".";
    }
}
