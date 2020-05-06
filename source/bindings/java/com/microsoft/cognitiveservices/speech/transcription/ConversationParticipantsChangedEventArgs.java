//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import java.util.ArrayList;
import java.util.List;

/** Defines a payload for the ParticipantsChanged event. Added in version 1.12.0 */
public class ConversationParticipantsChangedEventArgs extends SessionEventArgs {

    ConversationParticipantsChangedEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(eventArgs, false);
    }

    ConversationParticipantsChangedEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(eventArgs, dispose);
    }

    /**
     * Why the participant changed event was raised (e.g. a participant joined).
     * @return the reason.
     */
    public ParticipantChangedReason getReason() {
        return this.reason;
    }

    /**
     * The participant(s) that joined, left, or were updated.
     * @return The participant(s).
     */
    public List<Participant> getParticipants() {
        return this.participants;
    }

    @Override
    public String toString() {
        return "SessionId:"
                + getSessionId()
                + " Reason:"
                + this.reason
                + " Participants:"
                + this.participants.size();
    }

    private void storeEventData(long eventArgs, boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");

        IntRef intVal = new IntRef(0);
        Contracts.throwIfFail(getReason(eventHandle, intVal));
        this.reason = ParticipantChangedReason.values()[(int) intVal.getValue()];

        participants = new ArrayList<Participant>();

        IntRef resultRef = new IntRef(0);
        int index = 0;
        long result;
        result = getParticipantAt(eventHandle, index, resultRef);

        while (resultRef.getValue() > 0) {
            Participant participant = new Participant(resultRef.getValue());
            participants.add(participant);

            resultRef = new IntRef(0);
            index++;
            result = getParticipantAt(eventHandle, index, resultRef);
        }

        if (disposeNativeResources == true) {
            super.close();
        }
    }

    private List<Participant> participants;
    private ParticipantChangedReason reason;

    private final native long getParticipantAt(
            SafeHandle eventHandle, int index, IntRef participantRef);

    private final native long getReason(SafeHandle eventHandle, IntRef reasonVal);
}
