//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.StringRef;
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

        IntRef userRef = new IntRef(0);
        Contracts.throwIfFail(createFromUserId(userId, userRef));
        return new User(userRef.getValue());
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this.userHandle != null) {
            this.userHandle.close();
        }
        this.userHandle = null;
    }

    User(long handle) {
        this.userHandle = new SafeHandle(handle, SafeHandleType.User);
    }

    /**
     * Gets the user ID of a conversation transcriber session.
     * @return the user ID of a conversation transcribing session.
     */
    public String getId() {
        StringRef id = new StringRef("");
        Contracts.throwIfFail(getId(userHandle, id));
        return id.getValue();
    }

    private SafeHandle userHandle;

    /*! \cond INTERNAL */

    /**
     * Returns the user configuration.
     * @return The implementation of the user.
     */
    public SafeHandle getImpl() {
        return this.userHandle;
    }

    /*! \endcond */

    private final static native long createFromUserId(String userId, IntRef userRef);
    private final native long getId(SafeHandle userHandle, StringRef idRef);
}
