package com.microsoft.cognitiveservices.speech.samples;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.File;
import java.io.IOException;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;

import com.microsoft.cognitiveservices.speech.AudioInputStream;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;

public class WaveFileAudioInputStream extends AudioInputStream {
    AudioFormat m_format = new AudioFormat(16000, 16, 1, /*isSigned*/true, /*isBigEndian*/false);
    javax.sound.sampled.AudioInputStream _ais;
    
    public WaveFileAudioInputStream(String filename) {
        // Obtain and open the m_line.
        try {
            _ais = AudioSystem.getAudioInputStream(new File(filename));
            
            AudioFormat audioFormat = _ais.getFormat();
            if(audioFormat.getChannels() != m_format.getChannels()) throw new IllegalArgumentException("channels");
            if(audioFormat.getSampleRate() != m_format.getSampleRate()) throw new IllegalArgumentException("samplerate");
            if(audioFormat.getEncoding() != m_format.getEncoding()) throw new IllegalArgumentException("encoding");
            if(audioFormat.getSampleSizeInBits() != m_format.getSampleSizeInBits()) throw new IllegalArgumentException("bitspersample");
        } catch (Exception ex) {
            // Handle the error ...
            throw new IllegalArgumentException(ex);
        }
    }

    /**
     * Reads data from audio input stream into the data buffer. The maximal number
     * of bytes to be read is determined by the size of dataBuffer.
     * 
     * @param dataBuffer
     *            The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    @Override
    public int read(byte[] dataBuffer) {
        if(dataBuffer == null) throw new NullPointerException("dataBuffer");
        
        try {
            int numRead = _ais.read(dataBuffer, 0, dataBuffer.length);
            return numRead > 0 ? numRead : 0;
        } catch (Exception e) {
            throw new IllegalAccessError(e.toString());
        }
    }

    /**
     * Returns the m_format of this audio stream.
     * 
     * @return The m_format of the audio stream.
     */
    @Override
    public AudioInputStreamFormat getFormat() {
        AudioInputStreamFormat  f = new AudioInputStreamFormat();
        f.nBlockAlign = (short)(m_format.getChannels() * (m_format.getSampleSizeInBits() + 7) / 8);
        f.nAvgBytesPerSec = f.nBlockAlign * (int)m_format.getSampleRate();
        f.nChannels = (short) m_format.getChannels();
        f.nSamplesPerSec = (int)m_format.getSampleRate();
        f.wBitsPerSample = (short) m_format.getSampleSizeInBits();
        f.wFormatTag = 1; // PCM signed (we selected this in the constructor!).
        return f;
    }

    /**
     * Closes the audio input stream.
     */
    @Override
    public void close() {
        try {
            javax.sound.sampled.AudioInputStream a = _ais;
            _ais = null;
            a.close();
        } catch (IOException | NullPointerException e) {
            throw new IllegalAccessError(e.toString());
        }
    }
}
