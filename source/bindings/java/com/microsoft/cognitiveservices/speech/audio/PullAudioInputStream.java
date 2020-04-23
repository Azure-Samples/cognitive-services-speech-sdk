package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.audio.AudioStreamFormat;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents audio input stream used for custom audio input configurations.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
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
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz, 16 bit, mono PCM).
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @return The push audio input stream being created.
     */
    public static PullAudioInputStream create(PullAudioInputStreamCallback callback) {
        SafeHandle audioStreamHandle = new SafeHandle(0, SafeHandleType.AudioInputStream);
        Contracts.throwIfFail(createPullAudioInputStream(audioStreamHandle, null));
        return new PullAudioInputStream(audioStreamHandle, callback);
    }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method.
     * @return The push audio input stream being created.
     */
    public static PullAudioInputStream create(PullAudioInputStreamCallback callback, AudioStreamFormat format) {
        SafeHandle audioStreamHandle = new SafeHandle(0, SafeHandleType.AudioInputStream);
        Contracts.throwIfFail(createPullAudioInputStream(audioStreamHandle, format.getImpl()));
        return new PullAudioInputStream(audioStreamHandle, callback);
    }

    /**
     * Explicitly frees any external resource attached to the object
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        callbackHandle = null;
        super.close();
    }

    /*! \cond PROTECTED */

    protected PullAudioInputStream(SafeHandle stream, PullAudioInputStreamCallback callback) {
        super(stream);
        Contracts.throwIfNull(streamHandle, "streamHandle");
        callbackHandle = callback;
        Contracts.throwIfFail(setStreamCallbacks(streamHandle));
    }

    /*! \endcond */

    private PullAudioInputStreamCallback getCallbackHandle() {
        return callbackHandle;
    }

    private final static native long createPullAudioInputStream(SafeHandle audioStreamHandle, SafeHandle formatHandle);
    private final native long setStreamCallbacks(SafeHandle streamHandle);

    private PullAudioInputStreamCallback callbackHandle;
}
