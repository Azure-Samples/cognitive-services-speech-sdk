package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/**
  * Defines audio input stream.
  */
public class AudioInputStream
{
    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @param dataBuffer The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    public int Read(byte[] dataBuffer)
    {
        throw new UnsupportedOperationException();
    }

    /**
      * Closes the audio input stream.
      */
    public void Close()
    {
        throw new UnsupportedOperationException();
    }
}
