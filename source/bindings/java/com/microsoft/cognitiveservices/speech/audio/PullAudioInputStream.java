package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents audio input stream used for custom audio input configurations.
 */
public final class PullAudioInputStream extends com.microsoft.cognitiveservices.speech.audio.AudioInputStream
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
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16Khz 16bit mono PCM).
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @return The push audio input stream being created.
     */
    public static PullAudioInputStream create(PullAudioInputStreamCallback callback) {
        return new PullAudioInputStream(com.microsoft.cognitiveservices.speech.internal.AudioInputStream.CreatePullStream(callback.getAdapter()), callback);
    }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method (currently only support 16Khz 16bit mono PCM).
     * @return The push audio input stream being created.
     */
    public static PullAudioInputStream create(PullAudioInputStreamCallback callback, AudioStreamFormat format) {
        return new PullAudioInputStream(com.microsoft.cognitiveservices.speech.internal.AudioInputStream.CreatePullStream(format.getFormatImpl(), callback.getAdapter()), callback);
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

    protected PullAudioInputStream(com.microsoft.cognitiveservices.speech.internal.PullAudioInputStream stream, PullAudioInputStreamCallback callback) {
        super(stream);
        _callbackKeepAlive = callback;
    }

    /*! \endcond */

    @SuppressWarnings("unused")
    private PullAudioInputStreamCallback _callbackKeepAlive;
}
