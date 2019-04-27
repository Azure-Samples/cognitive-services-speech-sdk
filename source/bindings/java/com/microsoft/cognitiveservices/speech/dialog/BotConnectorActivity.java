//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines a BotConnectorActivity.
 */
public class BotConnectorActivity {

    /*! \cond PROTECTED */
    static Class<?> botConnectorActivity = null;


    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
        botConnectorActivity = BotConnectorActivity.class;
    }

    protected BotConnectorActivity(com.microsoft.cognitiveservices.speech.internal.BotConnectorActivity botConnectorActivity) {
        Contracts.throwIfNull(botConnectorActivity, "botConnectorActivity");
        this.botConnectorActivityImpl = botConnectorActivity;
    }
    /*! \endcond */

    /**
     * Create a bot activity from a serialized activity.
     * @param activity Serialized activity
     * @return The activity created.
     */
    public static BotConnectorActivity fromSerializedActivity(String activity) {
        Contracts.throwIfNull(activity, "activity");
        return new BotConnectorActivity(com.microsoft.cognitiveservices.speech.internal.BotConnectorActivity.FromSerializedActivity(activity));
    }

    /**
     * Returns the activity serialized as a string.
     * @return The serialized activity.
     */
    public String serialize() {
        return this.botConnectorActivityImpl.Serialize();
    }

    /*! \cond PROTECTED */
    private com.microsoft.cognitiveservices.speech.internal.BotConnectorActivity botConnectorActivityImpl;
    /*! \endcond */

    /**
      * Returns the implementation of the activity.
      * @return The implementation of the activity.
      */
    public com.microsoft.cognitiveservices.speech.internal.BotConnectorActivity getImpl() {
        return this.botConnectorActivityImpl;
    }
}
