package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;


/**
  * Defines audio input stream.
  */
public abstract class AudioInputStream
{
    public AudioInputStream() {
        _adapter = new AudioInputStreamAdapter(this);
    }

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @param dataBuffer The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    public abstract long read(byte[] dataBuffer);

    /**
     * Returns the format of this audio stream.
     * @return The format of the audio stream.
     */
    public abstract AudioInputStreamFormat getFormat();
 
    /**
      * Closes the audio input stream.
      */
    public abstract void close();

    private AudioInputStreamAdapter  _adapter;
    com.microsoft.cognitiveservices.speech.internal.AudioInputStream getAdapter() {
        return _adapter;
    }

    static class AudioInputStreamAdapter extends com.microsoft.cognitiveservices.speech.internal.AudioInputStream {
        private AudioInputStream _target;

        AudioInputStreamAdapter(AudioInputStream target) {
            _target = target;
        }

        /**
         * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
         * @param dataBuffer The byte array to store the read data.
         * @return the number of bytes have been read.
         */
        @Override
        public long Read(byte[] dataBuffer) {
            return _target.read(dataBuffer);
        }

        /**
          * Closes the audio input stream.
          */
        @Override
        public void Close() {
            _target.close();
        }

        /**
         * The Function being called to get the data from the audio stream.
         * @param pformat The pointer to the AudioInputStreamFormat buffer, or null if querying the size of the structure.
         * @param cbFormat The size of the AudioInputStreamFormat buffer being passed, or 0 if querying the size of the structure.
         * @return The size of the AudioInputStreamFormat buffer required to hold the format information.
         */
        @Override
        public long GetFormat(com.microsoft.cognitiveservices.speech.internal.AudioInputStreamFormat pformat, long cbFormat) {
         // Note: 24 Bytes is the size of AudioInputStreamFormat
            if (pformat == null || cbFormat < 24) {
                return 24;
            }

            AudioInputStreamFormat format = _target.getFormat();
            Contracts.throwIfNull(format, "format");

            pformat.setAvgBytesPerSec(format.AvgBytesPerSec);
            pformat.setBlockAlign(format.BlockAlign);
            pformat.setChannels(format.Channels);
            pformat.setSamplesPerSec(format.SamplesPerSec);
            pformat.setBitsPerSample(format.BitsPerSample);
            pformat.setFormatTag(format.FormatTag);

            return 24;
        }
    }
}
