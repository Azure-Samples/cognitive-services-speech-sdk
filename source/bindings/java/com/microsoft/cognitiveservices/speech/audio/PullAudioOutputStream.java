package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents memory backed push audio output stream used for custom audio output configurations.
 */
public final class PullAudioOutputStream extends com.microsoft.cognitiveservices.speech.audio.AudioOutputStream
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
     * @return The pull audio output stream being created.
     */
    public static PullAudioOutputStream create() {
        return new PullAudioOutputStream(com.microsoft.cognitiveservices.speech.internal.AudioOutputStream.CreatePullStream());
    }

    /**
     * Reads the audio data specified by making an internal copy of the data. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * @param dataBuffer The audio buffer of which this function will make a copy to.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public long read(byte[] dataBuffer) {
        return this._pullStreamImpl.Read(dataBuffer);
    }

    /*! \cond PROTECTED */
    public PullAudioOutputStream(com.microsoft.cognitiveservices.speech.internal.PullAudioOutputStream stream) {
        super(stream);
        this._pullStreamImpl = stream;
    }
     /*! \endcond */

    private com.microsoft.cognitiveservices.speech.internal.PullAudioOutputStream _pullStreamImpl;
}
