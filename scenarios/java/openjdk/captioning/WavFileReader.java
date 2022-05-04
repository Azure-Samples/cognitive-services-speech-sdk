//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// Adapted from code in:
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/WavStream.java

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import com.microsoft.cognitiveservices.speech.audio.AudioStreamFormat;

public class WavFileReader {
    private AudioStreamFormat format;
    private InputStream m_stream;

    public AudioStreamFormat getFormat() {
        return format;
    }

    public WavFileReader(String audioFileName) {
        try {
            m_stream = new FileInputStream(audioFileName);
            parseWavHeader();
        } catch (Exception ex) {
            throw new IllegalArgumentException(ex.getMessage());
        }
    }

    public void Close() {
        try {
            m_stream.close();
        } catch (IOException ex) {
            // ignored
        }
    }

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

    private void parseWavHeader() throws IOException {
        // Note: assumption about order of chunks
        // Tag "RIFF"
        byte data[] = new byte[4];
        int numRead = m_stream.read(data, 0, 4);
        ThrowIfFalse((numRead == 4) && (data[0] == 'R') && (data[1] == 'I') && (data[2] == 'F') && (data[3] == 'F'), "Invalid WAV file header, tag 'RIFF' is expected.");

        // Chunk size
        ReadInt32(m_stream);

        // Subchunk, Wave Header
        // Subchunk, Format
        // Tag: "WAVE"
        numRead = m_stream.read(data, 0, 4);
        ThrowIfFalse((numRead == 4) && (data[0] == 'W') && (data[1] == 'A') && (data[2] == 'V') && (data[3] == 'E'), "Invalid file header, tag 'WAVE' is expected.");

        // Tag: "fmt"
        numRead = m_stream.read(data, 0, 4);
        ThrowIfFalse((numRead == 4) && (data[0] == 'f') && (data[1] == 'm') && (data[2] == 't') && (data[3] == ' '), "Invalid file header, tag 'fmt ' is expected.");

        // chunk format size
        long formatSize = ReadInt32(m_stream);
        ThrowIfFalse(formatSize >= 16, "formatSize");

        int formatTag = ReadUInt16(m_stream);
        int channels = ReadUInt16(m_stream);
        int samplesPerSec = (int) ReadUInt32(m_stream);
        @SuppressWarnings("unused")
        int avgBytesPerSec = (int) ReadUInt32(m_stream);
        @SuppressWarnings("unused")
        int blockAlign = ReadUInt16(m_stream);
        int bitsPerSample = ReadUInt16(m_stream);

        this.format = AudioStreamFormat.getWaveFormatPCM(samplesPerSec, (short)bitsPerSample, (short)channels);
    }

    private static void ThrowIfFalse(Boolean condition, String message) {
        if (!condition) {
            throw new IllegalArgumentException(message);
        }
    }
}
