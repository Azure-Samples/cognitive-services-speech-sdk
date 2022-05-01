//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// Adapted from code in:
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/java/jre/console/src/com/microsoft/cognitiveservices/speech/samples/console/WavStream.java

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;

public class BinaryFileReader extends PullAudioInputStreamCallback {
    private InputStream m_stream;

    public BinaryFileReader(String audioFileName) {
        try {
            m_stream = new FileInputStream(audioFileName);
        } catch (Exception ex) {
            throw new IllegalArgumentException(ex.getMessage());
        }
    }

    @Override
    public int read(byte[] dataBuffer) {
        long ret = 0;

        try {
            ret = m_stream.read(dataBuffer, 0, dataBuffer.length);
        } catch (Exception ex) {
            System.out.println("Read " + ex);
        }

        return (int)Math.max(0, ret);
    }

    @Override
    public void close() {
        try {
            m_stream.close();
        } catch (IOException ex) {
            // ignored
        }
    }
}
