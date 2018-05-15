package com.microsoft.cognitiveservices.speech.samples;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.TargetDataLine;

import com.microsoft.cognitiveservices.speech.AudioInputStream;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;

public class MicrophoneAudioInputStream extends AudioInputStream {
    AudioFormat m_format = new AudioFormat(16000, 16, 1, /*isSigned*/true, /*isBigEndian*/false);
    TargetDataLine m_line;
    
    public MicrophoneAudioInputStream() {
        // Obtain and open the m_line.
        try {
            DataLine.Info info = new DataLine.Info(TargetDataLine.class, m_format);
            if (!AudioSystem.isLineSupported(info)) {
                // Handle the error ...
                throw new RuntimeException("m_line is not supported");
            }
        
            m_line = (TargetDataLine) AudioSystem.getLine(info);
            m_line.open(m_format);
            m_line.start();
        } catch (LineUnavailableException ex) {
            // Handle the error ...
            SampleSettings.displayException(ex);
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
        return m_line.read(dataBuffer, 0, dataBuffer.length);
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
        m_line.stop();
        m_line.close();
    }
}
