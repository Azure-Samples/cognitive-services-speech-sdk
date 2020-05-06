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

/** Defines a payload for the ConversationExpiration event. Added in version 1.12.0. */
public class ConversationExpirationEventArgs extends SessionEventArgs {

    ConversationExpirationEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);
    }

    ConversationExpirationEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /**
     * Get how much longer until the conversation expires.
     * @return How much longer until the conversation expires.
     */
    public long getExpirationTime() {
        return minutesLeft;
    }

    /**
     * Returns a String that represents the recognition event payload.
     *
     * @return A String that represents the recognition event payload.
     */
    @Override
    public String toString() {
        return "SessionId: " + this.getSessionId() + " Expiration: " + minutesLeft + ".";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef errorHandle = new IntRef(0);
        minutesLeft = getExpiration(eventHandle, errorHandle);
        Contracts.throwIfFail(errorHandle.getValue());
        if (disposeNativeResources == true) {
            super.close();
        }
    }

    private long minutesLeft;

    private final native long getExpiration(SafeHandle eventHandle, IntRef resultHandle);
}
