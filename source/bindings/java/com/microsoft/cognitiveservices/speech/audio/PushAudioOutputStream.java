//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.audio;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Represents audio output stream used for custom audio output configurations.
 * Added in version 1.7.0
 */
public final class PushAudioOutputStream extends com.microsoft.cognitiveservices.speech.audio.AudioOutputStream
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
     * Creates a PushAudioOutputStream that delegates to the specified callback interface for write() and close() methods.
     * @param callback The custom audio output object, derived from PushAudioOutputStreamCallback
     * @return The push audio output stream being created.
     */
    public static PushAudioOutputStream create(PushAudioOutputStreamCallback callback) {
        IntRef audioStreamHandle = new IntRef(0);
        Contracts.throwIfFail(createPushAudioOutputStream(audioStreamHandle));
        return new PushAudioOutputStream(audioStreamHandle, callback);
    }

    /*! \cond PROTECTED */

    protected PushAudioOutputStream(IntRef stream, PushAudioOutputStreamCallback callback) {
        super(stream);
        callbackHandle = callback;
        Contracts.throwIfFail(setStreamCallbacks(streamHandle));
    }

     /*! \endcond */

     private PushAudioOutputStreamCallback getCallbackHandle() {
        return callbackHandle;
    }

     private final static native long createPushAudioOutputStream(IntRef audioStreamHandle);
     private final native long setStreamCallbacks(SafeHandle streamHandle);

     private PushAudioOutputStreamCallback callbackHandle;
}
