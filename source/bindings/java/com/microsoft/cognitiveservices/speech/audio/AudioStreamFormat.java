package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.internal.AudioStreamContainerFormat;


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
        return new AudioStreamFormat(com.microsoft.cognitiveservices.speech.internal.AudioStreamFormat.GetDefaultInputFormat());
    }

    /**
     * Creates an audio stream format object with the specified pcm waveformat characteristics.
     * Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported. When used with Conversation Transcription, eight channels are supported.
     * @param samplesPerSecond Sample rate, in samples per second (Hertz).
     * @param bitsPerSample Bits per sample.
     * @param channels Number of channels in the waveform-audio data.
     * @return The audio stream format being created.
     */
    public static AudioStreamFormat getWaveFormatPCM(long samplesPerSecond, short bitsPerSample, short channels) {
        return new AudioStreamFormat(com.microsoft.cognitiveservices.speech.internal.AudioStreamFormat.GetWaveFormatPCM(samplesPerSecond, bitsPerSample, channels));
    }

    /**
     * Creates an audio stream format object with the specified compressed audio container format, to be used as input format.
     * Added in version 1.4.0
     * @param compressedFormat are defined in AudioStreamContainerFormat enum
     * @return The audio stream format being created.
     */
    public static AudioStreamFormat getCompressedFormat(AudioStreamContainerFormat compressedFormat) {
        return new AudioStreamFormat(com.microsoft.cognitiveservices.speech.internal.AudioStreamFormat.GetCompressedFormat(compressedFormat));
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this._formatImpl != null) {
            this._formatImpl.delete();
        }
        this._formatImpl = null;
    }

    AudioStreamFormat(com.microsoft.cognitiveservices.speech.internal.AudioStreamFormat format)
    {
        Contracts.throwIfNull(format, "format");
        this._formatImpl = format;
    }

    private com.microsoft.cognitiveservices.speech.internal.AudioStreamFormat _formatImpl;

    /*! \cond INTERNAL */

    /**
     * Returns the audio stream format.
     * @return The implementation of the format.
     */
    public com.microsoft.cognitiveservices.speech.internal.AudioStreamFormat getFormatImpl() {
        return this._formatImpl;
    }

    /*! \endcond */
}
