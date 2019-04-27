package tests.unit;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.FileInputStream;
import java.io.IOException;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.audio.PullAudioInputStreamCallback;

public class WavFileAudioInputStream extends PullAudioInputStreamCallback {
    private FileInputStream audioInputStream;
    private int audioInputStreamAvailableLength;

    public WavFileAudioInputStream(String filename) {
        try {
            this.audioInputStream = new FileInputStream(filename);

            byte header[] = new byte[4];
            int numRead = this.audioInputStream.read(header);
            if (numRead != header.length || header[0] != 'R' || header[1] != 'I' || header[2] != 'F' || header[3] != 'F') {
                throw new IllegalArgumentException("not a wave file");
            }

            if (4 != this.audioInputStream.read(header)) {
                throw new IllegalArgumentException("could not read length");
            }

            numRead = this.audioInputStream.read(header);
            if (numRead != header.length || header[0] != 'W' || header[1] != 'A' || header[2] != 'V' || header[3] != 'E') {
                throw new IllegalArgumentException("not a wave riff in file");
            }

            byte size[] = new byte[4];
            while(true) {
                if (header.length != this.audioInputStream.read(header)) {
                    throw new IllegalArgumentException("could not read chunk id");
                }

                if (size.length != this.audioInputStream.read(size)) {
                    throw new IllegalArgumentException("could not read chunk size");
                }

                int chunkSize = ((size[0] & 0xff) | ((size[1] & 0xff) << 8) | ((size[2] & 0xff) << 16) | ((size[3] & 0xff) << 24));
                if (chunkSize <= 0) {
                    throw new IllegalArgumentException("could not decode chunk size: " + chunkSize);
                }

                // found the data chunk, so save its length
                // and break out the search
                if (header[0] == 'd' && header[1] == 'a' && header[2] == 't' && header[3] == 'a') {
                    this.audioInputStreamAvailableLength = chunkSize;
                    break;
                }

                if (chunkSize != this.audioInputStream.skip(chunkSize)) {
                    throw new IllegalArgumentException("could not skip chunk size: " + chunkSize);
                }
            }
        } catch (IOException ex) {
            if (this.audioInputStream != null) {
                try {
                    this.audioInputStream.close();
                } catch (IOException e) {
                    // ignore
                }
                this.audioInputStream = null;
            }

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
        if (dataBuffer == null) throw new NullPointerException("dataBuffer");

        try {
            // only read the data chunk, ignore any
            // trailing data
            int wantRead = Math.min(this.audioInputStreamAvailableLength, dataBuffer.length);

            int numRead = audioInputStream.read(dataBuffer, 0, wantRead);
            int retLength = (numRead > 0) ? numRead : 0;

            this.audioInputStreamAvailableLength -= retLength;
            return retLength;
        } catch (Exception e) {
            throw new IllegalAccessError(e.toString());
        }
    }

    /**
      * Get string value for DataBuffer_TimeStamp or DataBuffer_UserId Property ids.
      * @param id The Property id.
      * @return The String value associated to Property id.
      */
    @Override
    public String getProperty(PropertyId id) {
        String propertyIdStr = "";
        if (PropertyId.DataBuffer_UserId == id) {
            propertyIdStr = "speaker123";
        }
        else if(PropertyId.DataBuffer_TimeStamp == id) {
            propertyIdStr = "somefaketimestamp";
        }
        return propertyIdStr;
    }

    /**
     * Closes the audio input stream.
     */
    @Override
    public void close() {
        try {
            this.audioInputStreamAvailableLength = 0;
            this.audioInputStream.close();
            this.audioInputStream = null;
        } catch (IOException | NullPointerException e) {
            throw new IllegalAccessError(e.toString());
        }
    }
}
