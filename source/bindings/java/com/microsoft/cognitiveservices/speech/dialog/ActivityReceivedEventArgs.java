//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.audio.PullAudioOutputStream;
import com.microsoft.cognitiveservices.speech.internal.PullAudioInputStream;

/**
 * Class that describes the events of a activity received event.
 */
public class ActivityReceivedEventArgs {

    /**
     * Constructs an ActivityReceivedEventArgs object.
     * @param args The native ActivityReceivedEventArgs
     */
    public ActivityReceivedEventArgs(com.microsoft.cognitiveservices.speech.internal.ActivityReceivedEventArgs args) {
        Contracts.throwIfNull(args, "args");
        activityReceivedEventArgs = args;
    }

    /**
     * Gets the activity received.
     * @return The activity associated with the event.
     */
    public BotConnectorActivity getActivity() {
        return new BotConnectorActivity(activityReceivedEventArgs.GetActivity());
    }

    /**
     * Checks whether the event has associated audio.
     * @return True if the event has audio associated, false otherwise.
     */
    public boolean hasAudio() {
        return activityReceivedEventArgs.HasAudio();
    }

    /**
     * Gets the associated audio stream if present.
     * @return The associated audio stream.
     */
    public PullAudioOutputStream getAudio() {
        return new PullAudioOutputStream(activityReceivedEventArgs.GetAudio());
    }

    /*! \cond PROTECTED */
    private com.microsoft.cognitiveservices.speech.internal.ActivityReceivedEventArgs activityReceivedEventArgs;
    /*! \endcond */
}