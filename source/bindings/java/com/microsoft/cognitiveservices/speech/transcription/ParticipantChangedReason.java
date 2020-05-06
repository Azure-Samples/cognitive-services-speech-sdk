//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//
package com.microsoft.cognitiveservices.speech.transcription;

/** Why the participant changed event was raised. Added in version 1.12.0 */
public enum ParticipantChangedReason {
    /** Participant has joined the conversation. */
    JoinedConversation(0),

    /**
     * Participant has left the conversation. This could be voluntary, or involuntary (e.g. they are
     * experiencing networking issues).
     */
    LeftConversation(1),

    /** The participants' state has changed (e.g. they became muted, changed their nickname). */
    Updated(2);

    private ParticipantChangedReason(int id) {
        this.id = id;
    }

    /**
     * Returns the value of participant changed reason
     *
     * @return the changed reason id value
     */
    public int getValue() {
        return this.id;
    }

    private final int id;
}
