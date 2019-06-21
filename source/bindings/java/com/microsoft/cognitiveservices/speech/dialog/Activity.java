//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines an Activity.
 */
public class Activity {

    /*! \cond PROTECTED */
    static Class<?> activity = null;


    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
        activity = Activity.class;
    }

    protected Activity(com.microsoft.cognitiveservices.speech.internal.Activity activity) {
        Contracts.throwIfNull(activity, "activity");
        this.activityImpl = activity;
    }
    /*! \endcond */

    /**
     * Create an activity from a serialized activity.
     * @param activity Serialized activity
     * @return The activity created.
     */
    public static Activity fromSerializedActivity(String activity) {
        Contracts.throwIfNull(activity, "activity");
        return new Activity(com.microsoft.cognitiveservices.speech.internal.Activity.FromSerializedActivity(activity));
    }

    /**
     * Returns the activity serialized as a string.
     * @return The serialized activity.
     */
    public String serialize() {
        return this.activityImpl.Serialize();
    }

    /*! \cond PROTECTED */
    private com.microsoft.cognitiveservices.speech.internal.Activity activityImpl;
    /*! \endcond */

    /**
      * Returns the implementation of the activity.
      * @return The implementation of the activity.
      */
    public com.microsoft.cognitiveservices.speech.internal.Activity getImpl() {
        return this.activityImpl;
    }
}
