/**
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
 */
// <code>
package com.speechsdk.quickstart;

import com.microsoft.cognitiveservices.speech.audio.PullAudioOutputStream;

import java.io.IOException;
import java.io.InputStream;

/**
 * This class is an input stream with a specified audio format. This input stream provides
 * several methods to read a certain number of bytes from  the stream, or an unspecified number of bytes.
 * This stream allows skipping over a certain bytes of the audio stream to get to a later position.
 * This stream does not support mark.
 */
public final class ActivityAudioStream extends InputStream {
    /**
     * The number of samples played per second. (16 kHz)
     */
    public static final long SAMPLE_RATE = 16000;
    /**
     * The number of bits in each sample of a sound that has this format. (16 bits)
     */
    public static final int BITS_PER_SECOND = 16;
    /**
     * The number of audio channels in this format (1 for mono).
     */
    public static final int CHANNELS = 1;
    /**
     * The number of bytes in each frame of a sound that has this format (2).
     */
    public static final int FRAME_SIZE = 2;

    /**
     * Reads up to a specified maximum number of bytes of data from the audio
     * stream, putting them into the given byte array.
     *
     * @param b   the buffer into which the data is read
     * @param off the offset, from the beginning of array <code>b</code>, at which
     *            the data will be written
     * @param len the maximum number of bytes to read
     * @return the total number of bytes read into the buffer, or -1 if there
     * is no more data because the end of the stream has been reached
     */
    @Override
    public int read(byte[] b, int off, int len) {
        byte[] tempBuffer = new byte[len];
        int n = (int) this.pullStreamImpl.read(tempBuffer);
        for (int i = 0; i < n; i++) {
            if (off + i > b.length) {
                throw new ArrayIndexOutOfBoundsException(b.length);
            }
            b[off + i] = tempBuffer[i];
        }
        if (n == 0) {
            return -1;
        }
        return n;
    }

    /**
     * Reads the next byte of data from the activity audio stream if available.
     *
     * @return the next byte of data, or -1 if the end of the stream is reached
     * @see #read(byte[], int, int)
     * @see #read(byte[])
     * @see #available
     * <p>
     */
    @Override
    public int read() {
        byte[] data = new byte[1];
        int temp = read(data);
        if (temp <= 0) {
            // we have a weird situation if read(byte[]) returns 0!
            return -1;
        }
        return data[0] & 0xFF;
    }

    /**
     * Reads up to a specified maximum number of bytes of data from the activity audio stream
     * putting them into the given byte array.
     *
     * @param b the buffer into which the data is read
     * @return the total number of bytes read into the buffer, or -1 if there
     * is no more data because the end of the stream has been reached
     */
    @Override
    public int read(byte[] b) {
        int n = (int) pullStreamImpl.read(b);
        if (n == 0) {
            return -1;
        }
        return n;
    }

    /**
     * Skips over and discards a specified number of bytes from this
     * audio input stream.
     *
     * @param n the requested number of bytes to be skipped
     * @return the actual number of bytes skipped
     * @throws IOException if an input or output error occurs
     * @see #read
     * @see #available
     */
    @Override
    public long skip(long n) {
        if (n <= 0) {
            return 0;
        }
        if (n <= Integer.MAX_VALUE) {
            byte[] tempBuffer = new byte[(int) n];
            return read(tempBuffer);
        }
        long count = 0;
        for (long i = n; i > 0; i -= Integer.MAX_VALUE) {
            int size = (int) Math.min(Integer.MAX_VALUE, i);
            byte[] tempBuffer = new byte[size];
            count += read(tempBuffer);
        }
        return count;
    }

    /**
     * Closes this audio input stream and releases any system resources associated
     * with the stream.
     */
    @Override
    public void close() {
        this.pullStreamImpl.close();
    }

    /**
     * Fetch the audio format for the ActivityAudioStream. The ActivityAudioFormat defines the sample rate, bits per sample and the # channels
     *
     * @return instance of the ActivityAudioFormat associated with the stream
     */
    public ActivityAudioStream.ActivityAudioFormat getActivityAudioFormat() {
        return activityAudioFormat;
    }

    /**
     * Returns the maximum number of bytes that can be read (or skipped over) from this
     * audio input stream without blocking.
     *
     * @return the number of bytes that can be read from this audio input stream without blocking.
     * As this implementation does not buffer this will be defaulted to 0
     */
    @Override
    public int available() {
        return 0;
    }

    public ActivityAudioStream(final PullAudioOutputStream stream) {
        pullStreamImpl = stream;
        this.activityAudioFormat = new ActivityAudioStream.ActivityAudioFormat(SAMPLE_RATE, BITS_PER_SECOND, CHANNELS, FRAME_SIZE, AudioEncoding.PCM_SIGNED);
    }

    private PullAudioOutputStream pullStreamImpl;

    private ActivityAudioFormat activityAudioFormat;

    /**
     * ActivityAudioFormat is an internal format which contains metadata regarding the type of arrangement of
     * audio bits in this activity audio stream.
     */
    static class ActivityAudioFormat {

        private long samplesPerSecond;
        private int bitsPerSample;
        private int channels;
        private int frameSize;
        private AudioEncoding encoding;

        public ActivityAudioFormat(long samplesPerSecond, int bitsPerSample, int channels, int frameSize, AudioEncoding encoding) {
            this.samplesPerSecond = samplesPerSecond;
            this.bitsPerSample = bitsPerSample;
            this.channels = channels;
            this.encoding = encoding;
            this.frameSize = frameSize;
        }

        /**
         * Fetch the number of samples played per second for the associated audio stream format.
         *
         * @return the number of samples played per second
         */
        public long getSamplesPerSecond() {
            return samplesPerSecond;
        }

        /**
         * Fetch the number of bits in each sample of a sound that has this audio stream format.
         *
         * @return the number of bits per sample
         */
        public int getBitsPerSample() {
            return bitsPerSample;
        }

        /**
         * Fetch the number of audio channels used by this audio stream format.
         *
         * @return the number of channels
         */
        public int getChannels() {
            return channels;
        }

        /**
         * Fetch the default number of bytes in a frame required by this audio stream format.
         *
         * @return the number of bytes
         */
        public int getFrameSize() {
            return frameSize;
        }

        /**
         * Fetch the audio encoding type associated with this audio stream format.
         *
         * @return the encoding associated
         */
        public AudioEncoding getEncoding() {
            return encoding;
        }
    }

    /**
     * Enum defining the types of audio encoding supported by this stream
     */
    public enum AudioEncoding {
        PCM_SIGNED("PCM_SIGNED");

        String value;

        AudioEncoding(String value) {
            this.value = value;
        }
    }
}
// </code>
