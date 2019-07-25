//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.audio;

import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents audio output stream used for custom audio output configurations.
 * Added in version 1.7.0
 */
public final class PushAudioOutputStream extends com.microsoft.cognitiveservices.speech.audio.AudioOutputStream
{
    @SuppressWarnings("unused")
    private PushAudioOutputStreamCallback _callbackKeepAlive;

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
        return new PushAudioOutputStream(com.microsoft.cognitiveservices.speech.internal.AudioOutputStream.CreatePushStream(callback.getAdapter()), callback);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        if (this._streamImpl != null) {
            this._streamImpl.delete();
        }
        this._streamImpl = null;
    }

    /*! \cond PROTECTED */

    protected PushAudioOutputStream(com.microsoft.cognitiveservices.speech.internal.PushAudioOutputStream stream, PushAudioOutputStreamCallback callback) {
        super(stream);
        _callbackKeepAlive = callback;
    }

     /*! \endcond */
}
