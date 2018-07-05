//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.quickstart;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

import com.microsoft.cognitiveservices.speech.AudioInputStream;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;

/**
 * MicrophoneStream exposes the Android Microphone as a AudioInputStream
 * to be consumed by the Speech SDK.
 * It configures the microphone with 16000khz, 16bits, mono.
 */
public class MicrophoneStream extends AudioInputStream {
    private final static int SAMPLE_RATE = 16000;
    private final AudioInputStreamFormat format;
    private AudioRecord recorder;

    public MicrophoneStream() {
        this.format = new AudioInputStreamFormat();
        this.initMic();
    }

    @Override
    public AudioInputStreamFormat getFormat() {
        return this.format;
    }

    @Override
    public long read(byte[] bytes) {
        long ret = this.recorder.read(bytes, 0, bytes.length);
        return ret;
    }

    @Override
    public void close() {
        this.recorder.release();
        this.recorder = null;
    }

    private void initMic() {
        // Note: currently, the SpeechSDK supports 16000, 16bit, mono streams only.
        AudioFormat af = new AudioFormat.Builder()
                .setSampleRate(SAMPLE_RATE)
                .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                .setChannelMask(AudioFormat.CHANNEL_IN_MONO)
                .build();
        this.recorder = new AudioRecord.Builder()
                .setAudioSource(MediaRecorder.AudioSource.VOICE_RECOGNITION)
                .setAudioFormat(af)
                .build();

        this.format.Channels = (short) 1; // MONO
        this.format.SamplesPerSec = SAMPLE_RATE;
        this.format.BitsPerSample = 16;
        this.format.AvgBytesPerSec = 2 * SAMPLE_RATE;
        this.format.FormatTag = 1; // PCM
        this.format.BlockAlign = (this.format.BitsPerSample + 7) / 8;

        this.recorder.startRecording();
    }
}
