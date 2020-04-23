//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

import java.io.Closeable;

/**
 * Defines payload for SessionStarted/Stopped events.
 */
public class SessionEventArgs // extends EventArgs
{
    /*! \cond INTERNAL */

    /**
     * Constructs a SessionEventArgs object.
     * @param eventArgs The native SessionEventArgs
     */
    public SessionEventArgs(long eventArgs) {
        Contracts.throwIfNull(eventArgs, "eventArgs");
        eventHandle = new SafeHandle(eventArgs, SafeHandleType.Event);
        storeEventData(false);
    }

    public SessionEventArgs(long eventArgs, boolean dispose) {
        Contracts.throwIfNull(eventArgs, "eventArgs");
        eventHandle = new SafeHandle(eventArgs, SafeHandleType.Event);
        storeEventData(dispose);
    }
    
    /*! \endcond */

    /**
     * Represents the session identifier.
     * @return Represents the session identifier.
     */
    public String getSessionId() {
        return sessionId;
    }

    /**
     * Returns a String that represents the session event.
     * @return a String that represents the session event.
     */
    @Override
    public String toString() {
        return "SessionId: " + sessionId + ".";
    }

    /*! \cond PROTECTED */
    /**
     * Explicitly frees any external resource attached to the object
     */
    protected void close() {
        if(eventHandle != null) {
            eventHandle.close();
            eventHandle = null;    
        }
    }
    /*! \endcond */

    private void storeEventData(boolean disposeNativeResources) {
        StringRef sessionIdRef = new StringRef("");       
        Contracts.throwIfFail(getSessionId(eventHandle, sessionIdRef, maxSessionID));
        sessionId = sessionIdRef.getValue();
        Contracts.throwIfNull(this.sessionId, "SessionId");
        if (disposeNativeResources == true)
        {
            close();
        }
    }
    
    /*! \cond PROTECTED */
    protected SafeHandle eventHandle = null;
    /*! \endcond */

    private String sessionId;
    private final int maxUUID = 36;
    private final int maxSessionID = maxUUID + 1;

    private final native long getSessionId(SafeHandle eventHandle, StringRef sessionIdStr, int maxSessionIDValue);
}
