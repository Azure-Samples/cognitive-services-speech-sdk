//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.audio;

import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
  * An abstract base class that defines callback methods (write() and close()) for custom audio output streams).
  * Added in version 1.7.0
  */
public abstract class PushAudioOutputStreamCallback
{
    private PushAudioOutputStreamCallbackAdapter  _adapter;

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

    public PushAudioOutputStreamCallback() {
        this._adapter = new PushAudioOutputStreamCallbackAdapter(this);
    }

    static class PushAudioOutputStreamCallbackAdapter extends com.microsoft.cognitiveservices.speech.internal.PushAudioOutputStreamCallback {
        private com.microsoft.cognitiveservices.speech.audio.PushAudioOutputStreamCallback _target;

        PushAudioOutputStreamCallbackAdapter(com.microsoft.cognitiveservices.speech.audio.PushAudioOutputStreamCallback target) {
            this._target = target;
        }

        /**
         * Writes data to audio output stream from the data buffer.
         * @param dataBuffer The byte array to store the data to be written.
         * @return The number of written bytes.
         */
        @Override
        public int Write(byte[] dataBuffer) {
            return this._target.write(dataBuffer);
        }

        /**
          * Closes the audio output stream.
          */
        @Override
        public void Close() {
            this._target.close();
        }
    }

    /**
     * Writes data to audio output stream from the data buffer.
     * @param dataBuffer The byte array to store the data to be written.
     * @return The number of written bytes.
     */
    public abstract int write(byte[] dataBuffer);

    /**
      * Closes the audio output stream.
      * @return
      */
    public abstract void close();

    /*! \cond INTERNAL */

    /**
      * Returns the audio stream format.
      * @return The implementation of the format.
      */
    public com.microsoft.cognitiveservices.speech.internal.PushAudioOutputStreamCallback getAdapter() {
          return this._adapter;
    }

    /*! \endcond */
}
