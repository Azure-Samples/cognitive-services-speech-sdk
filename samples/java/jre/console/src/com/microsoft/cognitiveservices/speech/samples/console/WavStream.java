package com.microsoft.cognitiveservices.speech.samples.console;


import com.microsoft.cognitiveservices.speech.AudioInputStream;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;

import java.io.IOException;
import java.io.InputStream;

public class WavStream extends AudioInputStream {
    private final AudioInputStreamFormat format;
    private final InputStream stream;

    public WavStream(InputStream wavStream) {
        try {
            this.format = new AudioInputStreamFormat();
            this.stream = parseWavHeader(wavStream);
        } catch (Exception ex) {
            throw new IllegalArgumentException(ex.getMessage());
        }
    }

    //region AudioInputStream overrides
    @Override
    public AudioInputStreamFormat getFormat() {
        return this.format;
    }

    @Override
    public long read(byte[] bytes) {
        long ret = 0;

        try {
            ret = this.stream.read(bytes, 0, bytes.length);
        } catch (Exception ex) {
            System.out.println("Read " + ex);
        }

        return Math.max(0, ret);
    }

    @Override
    public void close() {
        try {
            this.stream.close();
        } catch (IOException ex) {
            // ignored
        }
    }
    // endregion

    // region Wav File helper functions
    private int ReadInt32(InputStream inputStream) throws IOException {
        int n = 0;
        for (int i = 0; i < 4; i++) {
            n |= inputStream.read() << (i * 8);
        }
        return n;
    }

    private long ReadUInt32(InputStream inputStream) throws IOException {
        long n = 0;
        for (int i = 0; i < 4; i++) {
            n |= inputStream.read() << (i * 8);
        }
        return n;
    }

    private int ReadUInt16(InputStream inputStream) throws IOException {
        int n = 0;
        for (int i = 0; i < 2; i++) {
            n |= inputStream.read() << (i * 8);
        }
        return n;
    }

    public InputStream parseWavHeader(InputStream reader) throws IOException {
        // Note: assumption about order of chunks
        // Tag "RIFF"
        byte data[] = new byte[4];
        reader.read(data, 0, 4);
        ThrowIfFalse((data[0] == 'R') && (data[1] == 'I') && (data[2] == 'F') && (data[3] == 'F'), "RIFF");

        // Chunk size
        long fileSize = ReadInt32(reader);

        // Subchunk, Wave Header
        // Subchunk, Format
        // Tag: "WAVE"
        reader.read(data, 0, 4);
        ThrowIfFalse ((data[0] == 'W') && (data[1] == 'A') && (data[2] == 'V') && (data[3] == 'E'), "WAVE");

        // Tag: "fmt"
        reader.read(data, 0, 4);
        ThrowIfFalse((data[0] == 'f') && (data[1] == 'm') && (data[2] == 't') && (data[3] == ' '), "fmt ");

        // chunk format size
        long formatSize = ReadInt32(reader);
        ThrowIfFalse(formatSize >= 16, "formatSize");

        format.FormatTag = ReadUInt16(reader);
        format.Channels = ReadUInt16(reader);
        format.SamplesPerSec = (int) ReadUInt32(reader);
        format.AvgBytesPerSec = (int) ReadUInt32(reader);
        format.BlockAlign = ReadUInt16(reader);
        format.BitsPerSample = ReadUInt16(reader);
        ThrowIfFalse(format.FormatTag == 1, "PCM"); // PCM

        // Until now we have read 16 bytes in format, the rest is cbSize and is ignored for now.
        if (formatSize > 16) {
            reader.read(new byte[(int) (formatSize - 16)]);
        }

        // Second Chunk, data
        // tag: data.
        reader.read(data, 0, 4);
        ThrowIfFalse ((data[0] == 'd') && (data[1] == 'a') && (data[2] == 't') && (data[3] == 'a'), "data");

        // data chunk size
        // Note: assumption is that only a single data chunk
        int dataSize = ReadInt32(reader);
        return reader;
    }

    private static void ThrowIfFalse(Boolean condition, String message) {
        if(!condition) {
        throw new IllegalArgumentException(message);
        }
    }
    // endregion
}
