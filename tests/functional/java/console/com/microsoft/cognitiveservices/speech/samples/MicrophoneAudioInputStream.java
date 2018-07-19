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
    private AudioFormat audioFormat = new AudioFormat(16000, 16, 1, /*isSigned*/true, /*isBigEndian*/false);
    private TargetDataLine audioLine;
    
    public MicrophoneAudioInputStream() {
        // Obtain and open the audioLine.
        try {
            DataLine.Info info = new DataLine.Info(TargetDataLine.class, audioFormat);

            if (!AudioSystem.isLineSupported(info)) {
                // Handle the error ...
                throw new RuntimeException("audio format is not supported");
            }
        
            audioLine = (TargetDataLine) AudioSystem.getLine(info);
            audioLine.open(audioFormat);
            audioLine.start();
        } catch (LineUnavailableException ex) {
            // Handle the error ...
            SampleSettings.displayException(ex);
            throw new UnsupportedOperationException("Audio format not supported: " + ex.getMessage());
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
    public long read(byte[] dataBuffer) {
        return audioLine.read(dataBuffer, 0, dataBuffer.length);
    }

    /**
     * Returns the audioFormat of this audio stream.
     * 
     * @return The audioFormat of the audio stream.
     */
    @Override
    public AudioInputStreamFormat getFormat() {
        AudioInputStreamFormat  f = new AudioInputStreamFormat();
        f.BlockAlign = (short)(audioFormat.getChannels() * (audioFormat.getSampleSizeInBits() + 7) / 8);
        f.AvgBytesPerSec = f.BlockAlign * (int)audioFormat.getSampleRate();
        f.Channels = (short) audioFormat.getChannels();
        f.SamplesPerSec = (int)audioFormat.getSampleRate();
        f.BitsPerSample = (short) audioFormat.getSampleSizeInBits();
        f.FormatTag = 1; // PCM signed (we selected this in the constructor!).
        return f;
    }

    /**
     * Closes the audio input stream.
     */
    @Override
    public void close() {
        audioLine.stop();
        audioLine.close();
    }
}
