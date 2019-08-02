//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package com.microsoft.cognitiveservices.speech.samples.quickstart;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

import com.microsoft.cognitiveservices.speech.audio.PushAudioOutputStreamCallback;

public class SpeakerStream extends PushAudioOutputStreamCallback {

    private AudioTrack audioTrack;

    private boolean blockAligned = true;

    private byte remainingByte = 0;

    public SpeakerStream()
    {
        audioTrack = new AudioTrack(
            new AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_MEDIA)
                .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                .build(),
            new AudioFormat.Builder()
                .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                .setSampleRate(16000)
                .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                .build(),
            AudioTrack.getMinBufferSize(
                16000,
                AudioFormat.CHANNEL_OUT_MONO,
                AudioFormat.ENCODING_PCM_16BIT) * 2,
            AudioTrack.MODE_STREAM,
            AudioManager.AUDIO_SESSION_ID_GENERATE);
        audioTrack.play();
    }

    @Override
    public int write(byte[] bytes) {
        byte[] bytesForPlaying = bytes;
        if (!blockAligned) {
            bytesForPlaying = new byte[bytes.length + 1];
            bytesForPlaying[0] = remainingByte;
            System.arraycopy(bytes, 0, bytesForPlaying, 1, bytes.length);
        }

        if (bytesForPlaying.length % 2 == 0) {
            blockAligned = true;
            audioTrack.write(bytesForPlaying, 0, bytesForPlaying.length);
        } else {
            blockAligned = false;
            remainingByte = bytesForPlaying[bytesForPlaying.length - 1];
            audioTrack.write(bytesForPlaying, 0, bytesForPlaying.length - 1);
        }

        return bytes.length;
    }

    @Override
    public void close() {
        audioTrack.flush();
        audioTrack.stop();
    }
}
// </code>
