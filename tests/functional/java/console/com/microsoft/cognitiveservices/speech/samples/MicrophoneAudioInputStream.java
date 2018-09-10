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

import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;

public class MicrophoneAudioInputStream extends PullAudioInputStreamCallback {
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
    public int read(byte[] dataBuffer) {
        return audioLine.read(dataBuffer, 0, dataBuffer.length);
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
