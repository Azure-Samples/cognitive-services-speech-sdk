// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/*
import java.io.File;
import java.io.IOException;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;

import com.microsoft.cognitiveservices.speech.AudioInputStream;
import com.microsoft.cognitiveservices.speech.AudioInputStreamFormat;
*/

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import * as fs from "fs";
import { AudioInputStream } from "../../../../../source/bindings/js/distrib/src/sdk/speech.browser/Exports";

export class WaveFileAudioInputStream extends  AudioInputStream {

    public constructor(filename: string) {
        // obtain and open the line.
        /*
        try {
            audioInputStream = AudioSystem.getAudioInputStream(new File(filename));

            AudioFormat audioFormat = audioInputStream.getFormat();
            if(audioFormat.getChannels() != audioFormat.getChannels()) throw new IllegalArgumentException("channels");
            if(audioFormat.getSampleRate() != audioFormat.getSampleRate()) throw new IllegalArgumentException("samplerate");
            if(audioFormat.getEncoding() != audioFormat.getEncoding()) throw new IllegalArgumentException("encoding");
            if(audioFormat.getSampleSizeInBits() != audioFormat.getSampleSizeInBits()) throw new IllegalArgumentException("bitspersample");
        } catch (Exception ex) {
            // Handle the error ...
            throw new IllegalArgumentException(ex);
        }
        */
        const fileContents: Buffer = fs.readFileSync(filename);

        const arrayBuffer: ArrayBuffer = Uint8Array.from(fileContents).buffer;
        const parts: ArrayBuffer[] = [arrayBuffer];
        const file: File = new File(parts, filename);

        super(file);
    }

    /**
     * Reads data from audio input stream into the data buffer. The maximal number
     * of bytes to be read is determined by the size of dataBuffer.
     *
     * @param dataBuffer
     *            The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    public read(dataBuffer: number[]): number {
        /*
        if(dataBuffer == null) throw new NullPointerException("dataBuffer");

        try {
            int numRead = audioInputStream.read(dataBuffer, 0, dataBuffer.length);
            return numRead > 0 ? numRead : 0;
        } catch (Exception e) {
            throw new IllegalAccessError(e.toString());
        }
        */


        return 0;
    }

    /**
     * Returns the audioFormat of this audio stream.
     *
     * @return The audioFormat of the audio stream.
     */
    public getFormat(): sdk.AudioInputStreamFormat {
        const f: sdk.AudioInputStreamFormat = new sdk.AudioInputStreamFormat();
        /*
        f.BlockAlign = (short)(audioFormat.getChannels() * (audioFormat.getSampleSizeInBits() + 7) / 8);
        f.AvgBytesPerSec = f.BlockAlign * (int)audioFormat.getSampleRate();
        f.Channels = (short) audioFormat.getChannels();
        f.SamplesPerSec = (int)audioFormat.getSampleRate();
        f.BitsPerSample = (short) audioFormat.getSampleSizeInBits();
        f.FormatTag = 1; // PCM signed (we selected this in the constructor!).
        */
        return f;
    }

    /**
     * Closes the audio input stream.
     */
    public close(): void {
        /*
            try {
                javax.sound.sampled.AudioInputStream a = audioInputStream;
                audioInputStream = null;
                a.close();
            } catch (IOException | NullPointerException e) {
                throw new IllegalAccessError(e.toString());
            }
            */
    }
}
