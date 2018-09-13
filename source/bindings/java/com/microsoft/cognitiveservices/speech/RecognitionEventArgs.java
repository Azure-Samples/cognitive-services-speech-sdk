package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.math.*;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines payload for session events like Speech Start/End Detected
 */
public final class RecognitionEventArgs // extends EventArgs
{
    public RecognitionEventArgs(RecognitionEventType type, com.microsoft.cognitiveservices.speech.internal.RecognitionEventArgs arg)
    {
        Contracts.throwIfNull(arg, "arg");

        this.sessionId = arg.getSessionId();
        this.eventType = type;
        this.offset = arg.getOffset();

        Contracts.throwIfNull(this.sessionId, "SessionId");
    }

    /**
     * Represents the event type.
     */
    public final RecognitionEventType eventType;

    /**
     * Represents the session identifier.
     */
    public final String sessionId;

    /**
     * Represents the message offset
     */
    public final BigInteger offset;

    /**
     * Returns a String that represents the session event.
     * @return A String that represents the session event.
     */
    @Override
    public String toString() {
        return "EventType: " + eventType.toString() + " SessionId: " + sessionId.toString() + " Offset: " + offset.toString() + ".";
    }
}
