//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.audio;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import java.io.Closeable;

/**
 * Represents audio output stream used for custom audio output configurations.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 * Updated in version 1.7.0
 */
public class AudioOutputStream implements Closeable
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
     * Creates a memory backed PullAudioOutputStream.
     * @return The audio output stream being created.
     */
    public static PullAudioOutputStream createPullStream() {
        return PullAudioOutputStream.create();
    }

    /**
     * Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods.
     * Added in version 1.7.0
     * @param callback The custom audio output object, derived from PushAudioOutputStreamCallback
     * @return The audio output stream being created.
     */
    public static PushAudioOutputStream createPushStream(PushAudioOutputStreamCallback callback) {
        return PushAudioOutputStream.create(callback);
    }

    /**
     * Explicitly frees any external resource attached to the object
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        if (this.streamHandle != null) {
            this.streamHandle.close();
        }
        this.streamHandle = null;
    }

    /*! \cond PROTECTED */

    protected AudioOutputStream(IntRef stream) {
        Contracts.throwIfNull(stream, "stream");
        this.streamHandle = new SafeHandle(stream.getValue(), SafeHandleType.AudioOutputStream);
    }

    protected SafeHandle streamHandle;

    /*! \endcond */

    /*! \cond INTERNAL */

    /**
     * Returns the audio output configuration.
     * @return The implementation of the stream.
     */
    public SafeHandle getImpl() {
        return this.streamHandle;
    }

    /*! \endcond */
}
