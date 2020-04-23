package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

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
        IntRef audioStreamHandle = new IntRef(0);
        Contracts.throwIfFail(createPullAudioOutputStream(audioStreamHandle));
        return new PullAudioOutputStream(audioStreamHandle);
    }

    /**
     * Reads the audio data specified by making an internal copy of the data. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * @param dataBuffer The audio buffer of which this function will make a copy to.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public long read(byte[] dataBuffer) {
        IntRef filledSize = new IntRef(0);
        Contracts.throwIfFail(pullAudioOutputStreamRead(streamHandle, dataBuffer, filledSize));
        return filledSize.getValue();
    }

    /*! \cond PROTECTED */
    public PullAudioOutputStream(IntRef stream) {
        super(stream);
    }    
     /*! \endcond */
     
     private final static native long createPullAudioOutputStream(IntRef audioStreamHandle);
     private final native long pullAudioOutputStreamRead(SafeHandle streamHandle, byte[] dataBuffer, IntRef filledSize);
}
