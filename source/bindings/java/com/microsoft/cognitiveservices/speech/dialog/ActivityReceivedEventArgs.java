//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.audio.PullAudioOutputStream;

/**
 * Class that describes the events of a activity received event.
 */
public class ActivityReceivedEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an ActivityReceivedEventArgs object.
     * @param args The native ActivityReceivedEventArgs
     */
    public ActivityReceivedEventArgs(long eventArgs) {
        Contracts.throwIfNull(eventArgs, "eventArgs");
        activityReceivedEventHandle = new SafeHandle(eventArgs, SafeHandleType.DialogServiceConnectorEvent);
        StringRef activityRef = new StringRef("");
        Contracts.throwIfFail(getActivity(activityReceivedEventHandle, activityRef));
        activity = activityRef.getValue();
        hasAudio = hasAudio(activityReceivedEventHandle);
    }
    
    /*! \endcond */

    /**
     * Gets the activity received.
     * @return The activity associated with the event.
     */
    public String getActivity() {
        return activity;
    }

    /**
     * Checks whether the event has associated audio.
     * @return True if the event has audio associated, false otherwise.
     */
    public boolean hasAudio() {
        return hasAudio;
    }

    /**
     * Gets the associated audio stream if present.
     * @return The associated audio stream.
     */
    public PullAudioOutputStream getAudio() {
        IntRef audioRef = new IntRef(0);
        Contracts.throwIfFail(getAudio(activityReceivedEventHandle, audioRef));
        return new PullAudioOutputStream(audioRef);
    }

    private SafeHandle activityReceivedEventHandle = null;
    private String activity = "";
    private boolean hasAudio = false;

    private final native long getAudio(SafeHandle activityReceivedEventHandle, IntRef audioRef);
    private final native long getActivity(SafeHandle activityReceivedEventHandle, StringRef activityRef);
    private final native boolean hasAudio(SafeHandle activityReceivedEventHandle);
}