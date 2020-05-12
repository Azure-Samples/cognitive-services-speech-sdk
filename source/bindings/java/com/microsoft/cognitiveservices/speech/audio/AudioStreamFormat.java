package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents audio stream format used for custom audio input configurations.
 * Updated in version 1.5.0.
 */
public final class AudioStreamFormat
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
     * Creates an audio stream format object representing the default audio stream format (16 kHz, 16 bit, mono PCM).
     * @return The audio stream format being created.
     */
    public static AudioStreamFormat getDefaultInputFormat() {
        SafeHandle streamFormatHandle = new SafeHandle(0, SafeHandleType.AudioStreamFormat);
        Contracts.throwIfFail(createFromDefaultInput(streamFormatHandle));
        return new AudioStreamFormat(streamFormatHandle);
    }

    /**
     * Creates an audio stream format object with the specified pcm waveformat characteristics.
     * @param samplesPerSecond Sample rate, in samples per second (Hertz).
     * @param bitsPerSample Bits per sample.
     * @param channels Number of channels in the waveform-audio data.
     * @return The audio stream format being created.
     */
    public static AudioStreamFormat getWaveFormatPCM(long samplesPerSecond, short bitsPerSample, short channels) {
        SafeHandle streamFormatHandle = new SafeHandle(0, SafeHandleType.AudioStreamFormat);
        Contracts.throwIfFail(createFromWaveFormatPCM(streamFormatHandle, samplesPerSecond, bitsPerSample, channels));        
        return new AudioStreamFormat(streamFormatHandle);
    }

    /**
     * Creates an audio stream format object with the specified compressed audio container format, to be used as input format.
     * Added in version 1.4.0
     * @param compressedFormat are defined in AudioStreamContainerFormat enum
     * @return The audio stream format being created.
     */
    public static AudioStreamFormat getCompressedFormat(AudioStreamContainerFormat compressedFormat) {
        SafeHandle streamFormatHandle = new SafeHandle(0, SafeHandleType.AudioStreamFormat);
        int value = AudioStreamContainerFormat.values()[(int)compressedFormat.ordinal()].getValue();
        Contracts.throwIfFail(createFromCompressedFormat(streamFormatHandle, value));
        return new AudioStreamFormat(streamFormatHandle);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this.formatHandle != null) {
            this.formatHandle.close();
            this.formatHandle = null;
        }
    }

    AudioStreamFormat(SafeHandle format)
    {
        Contracts.throwIfNull(format, "format");
        this.formatHandle = format;
    }

    private SafeHandle formatHandle = null;

    /*! \cond INTERNAL */

    /**
     * Returns the audio stream format.
     * @return The implementation of the format.
     */
    public SafeHandle getImpl() {
        return this.formatHandle;
    }
    
    /*! \endcond */

    private final static native long createFromDefaultInput(SafeHandle streamFormatHandle);
    private final static native long createFromWaveFormatPCM(SafeHandle streamFormatHandle, long samplesPerSecond, short bitsPerSample, short channels);
    private final static native long createFromCompressedFormat(SafeHandle streamFormatHandle, int compressedFormat);

}
