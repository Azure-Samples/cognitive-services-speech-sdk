//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents a user in a conversation.
 * Added in version 1.5.0
 */
public final class User
{
    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

    /**
     * Creates a User.
     * @param userId user's Id.
     * @return
     */
    public static com.microsoft.cognitiveservices.speech.transcription.User fromUserId(String userId) {
        return new User(com.microsoft.cognitiveservices.speech.internal.User.FromUserId(userId));
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this._userImpl != null) {
            this._userImpl.delete();
        }
        this._userImpl = null;
    }

    User(com.microsoft.cognitiveservices.speech.internal.User user) {
        Contracts.throwIfNull(user, "user");
        this._userImpl = user;
    }

    /**
     * Gets the user ID of a conversation transcriber session.
     * @return the user ID of a conversation transcribing session.
     */
    public String getId() {
        return _userImpl.GetId();
    }

    private com.microsoft.cognitiveservices.speech.internal.User _userImpl;

    /*! \cond INTERNAL */

    /**
     * Returns the user configuration.
     * @return The implementation of the user.
     */
    public com.microsoft.cognitiveservices.speech.internal.User getUserImpl() {
        return this._userImpl;
    }

    /*! \endcond */
}
