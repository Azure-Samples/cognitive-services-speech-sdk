//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.audio;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents audio output stream used for custom audio output configurations.
 */
public class AudioOutputStream
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
     * Creates a memory backed PullAudioOutputStream using the default format (16 kHz, 16 bit, mono PCM).
     * @return The audio output stream being created.
     */
    public static PullAudioOutputStream createPullStream() {
        return PullAudioOutputStream.create();
    }

    /**
     * Creates a memory backed PullAudioOutputStream with the specified audio format.
     * @param format The audio data format in which audio will be read from the pull audio stream's read() method.
     * @return The audio output stream being created.
     */
    public static PullAudioOutputStream createPullStream(AudioStreamFormat format) {
        return PullAudioOutputStream.create(format);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this._streamImpl != null) {
            this._streamImpl.delete();
        }
        this._streamImpl = null;
    }

    /*! \cond PROTECTED */

    protected AudioOutputStream(com.microsoft.cognitiveservices.speech.internal.AudioOutputStream stream) {
        Contracts.throwIfNull(stream, "stream");
        this._streamImpl = stream;
    }

    protected com.microsoft.cognitiveservices.speech.internal.AudioOutputStream _streamImpl;

    /*! \endcond */

    /*! \cond INTERNAL */

    /**
     * Returns the audio output configuration.
     * @return The implementation of the stream.
     */
    public com.microsoft.cognitiveservices.speech.internal.AudioOutputStream getStreamImpl() {
        return this._streamImpl;
    }

    /*! \endcond */
}
