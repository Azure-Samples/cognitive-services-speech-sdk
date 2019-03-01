package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
  * An abstract base class that defines callback methods (read() and close()) for custom audio input streams).
  */
public abstract class PullAudioInputStreamCallback
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

    public PullAudioInputStreamCallback() {
        this._adapter = new PullAudioInputStreamCallbackAdapter(this);
    }

    static class PullAudioInputStreamCallbackAdapter extends com.microsoft.cognitiveservices.speech.internal.PullAudioInputStreamCallback {
        private com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback _target;

        PullAudioInputStreamCallbackAdapter(com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback target) {
            this._target = target;
        }

        /**
         * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
         * If there is no data immediately available, read() blocks until the next data becomes available.
         * @param dataBuffer The byte array to store the read data.
         * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
         */
        @Override
        public int Read(byte[] dataBuffer) {
            return this._target.read(dataBuffer);
        }

        /**
          * Closes the audio input stream.
          */
        @Override
        public void Close() {
            this._target.close();
        }
    }

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * Note: The dataBuffer returned by read() should not contain any audio header.
     * @param dataBuffer The byte array to store the read data.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public abstract int read(byte[] dataBuffer);

    /**
      * Closes the audio input stream.
      */
    public abstract void close();

    private PullAudioInputStreamCallbackAdapter  _adapter;

    /*! \cond INTERNAL */

    /**
      * Returns the audio stream format.
      * @return The implementation of the format.
      */
    public com.microsoft.cognitiveservices.speech.internal.PullAudioInputStreamCallback getAdapter() {
          return this._adapter;
    }

    /*! \endcond */
}
