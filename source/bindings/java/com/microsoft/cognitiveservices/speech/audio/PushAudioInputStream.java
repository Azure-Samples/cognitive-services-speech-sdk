package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.CloseGuard;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 */
public final class PushAudioInputStream extends com.microsoft.cognitiveservices.speech.audio.AudioInputStream
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
     * Creates a memory backed PushAudioInputStream using the default format (16 kHz, 16 bit, mono PCM).
     * @return The push audio input stream being created.
     */
    public static PushAudioInputStream create() {
        SafeHandle audioStreamHandle = new SafeHandle(0, SafeHandleType.AudioInputStream);
        Contracts.throwIfFail(createPushAudioInputStream(audioStreamHandle, null));
        return new PushAudioInputStream(audioStreamHandle);
    }

    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @param format The audio data format in which audio will be written to the push audio stream's write() method.
     * @return The push audio input stream being created.
     */
    public static PushAudioInputStream create(AudioStreamFormat format) {
        SafeHandle audioStreamHandle = new SafeHandle(0, SafeHandleType.AudioInputStream);
        Contracts.throwIfFail(createPushAudioInputStream(audioStreamHandle, format.getImpl()));
        return new PushAudioInputStream(audioStreamHandle);
    }

    /**
     * Writes the audio data specified by making an internal copy of the data.
     * Note: The dataBuffer should not contain any audio header.
     * @param dataBuffer The audio buffer of which this function will make a copy.
     */
    public void write(byte[] dataBuffer) {
        cg.enterUseObject();
        try {
            Contracts.throwIfFail(pushAudioInputStreamWrite(streamHandle, dataBuffer, dataBuffer.length));
        } finally {
            cg.exitUseObject();
        }
    }

    /**
     * Closes the stream.
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        cg.closeObject();

        if (streamHandle != null) {
            Contracts.throwIfFail(pushAudioInputStreamClose(streamHandle));
        }
        super.close();
    }

    /*! \cond PROTECTED */

    protected PushAudioInputStream(SafeHandle stream) {
        super(stream);
    }

    /*! \endcond */

    private CloseGuard cg = new CloseGuard();

    private final static native long createPushAudioInputStream(SafeHandle audioStreamHandle, SafeHandle formatHandle);
    private final native long pushAudioInputStreamWrite(SafeHandle audioStreamHandle, byte[] dataBuffer, int length);
    private final native long pushAudioInputStreamClose(SafeHandle audioStreamHandle);
}
