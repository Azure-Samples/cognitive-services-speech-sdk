package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/**
  * Defines audio input stream.
  */
public class AudioInputStream extends com.microsoft.cognitiveservices.speech.internal.AudioInputStream
{
    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @param dataBuffer The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    @Override
    public long read(byte[] dataBuffer)
    {
        throw new UnsupportedOperationException();
    }

    /**
     * Returns the format of this audio stream.
     * @return The format of the audio stream.
     */
    public AudioInputStreamFormat getFormat() {
        throw new UnsupportedOperationException();
    }
 
    /**
      * Closes the audio input stream.
      */
    @Override
    public void close()
    {
        throw new UnsupportedOperationException();
    }

    /**
     * The Function being called to get the data from the audio stream.
     * @param pformat The pointer to the AudioInputStreamFormat buffer, or null if querying the size of the structure.
     * @param cbFormat The size of the AudioInputStreamFormat buffer being passed, or 0 if querying the size of the structure.
     * @return The size of the AudioInputStreamFormat buffer required to hold the format information.
     */
    @Override
    public int getFormat(com.microsoft.cognitiveservices.speech.internal.AudioInputStreamFormat pformat, int cbFormat) {
        // Note: 44 is the size of a standard WAVEFORMEX structure.
        if(pformat == null || cbFormat < 44) {
            return 44;
        }
        
        AudioInputStreamFormat format = getFormat();
        pformat.setCbSize(44);
        pformat.setNAvgBytesPerSec(format.nAvgBytesPerSec);
        pformat.setNBlockAlign(format.nBlockAlign);
        pformat.setNChannels(format.nChannels);
        pformat.setNSamplesPerSec(format.nSamplesPerSec);
        pformat.setWBitsPerSample(format.wBitsPerSample);
        pformat.setWFormatTag(format.wFormatTag);

        return 44;
    }
}
