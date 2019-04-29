package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
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
        return new PushAudioInputStream(com.microsoft.cognitiveservices.speech.internal.AudioInputStream.CreatePushStream());
    }

    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported. When used with the Conversation Transcription Service, eight channels are supported.
     * @param format The audio data format in which audio will be written to the push audio stream's write() method.
     * @return The push audio input stream being created.
     */
    public static PushAudioInputStream create(AudioStreamFormat format) {
        return new PushAudioInputStream(com.microsoft.cognitiveservices.speech.internal.AudioInputStream.CreatePushStream(format.getFormatImpl()));
    }

    /**
     * Writes the audio data specified by making an internal copy of the data.
     * Note: The dataBuffer should not contain any audio header.
     * @param dataBuffer The audio buffer of which this function will make a copy.
     */
    public void write(byte[] dataBuffer) {
        this._pushStreamImpl.Write(dataBuffer);
    }

    /**
     * Closes the stream.
     */
    @Override
    public void close() {
        if (this._pushStreamImpl != null) {
            this._pushStreamImpl.Close();
        }
        this._pushStreamImpl = null;
    }

    /*! \cond PROTECTED */

    protected PushAudioInputStream(com.microsoft.cognitiveservices.speech.internal.PushAudioInputStream stream) {
        super(stream);
        this._pushStreamImpl = stream;
    }

    /*! \endcond */

    private com.microsoft.cognitiveservices.speech.internal.PushAudioInputStream _pushStreamImpl;
}
