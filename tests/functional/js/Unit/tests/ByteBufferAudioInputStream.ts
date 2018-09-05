// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";

export class ByteBufferAudioInputStream extends sdk.AudioInputStream {

    public constructor(buffer: Uint8Array) {

        const parts: ArrayBuffer[] = [buffer.buffer];
        const file: File = new File(parts, "file.wav");

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
        return 0;
    }

    /**
     * Returns the audioFormat of this audio stream.
     *
     * @return The audioFormat of the audio stream.
     */
    public getFormat(): sdk.AudioInputStreamFormat {
        const f: sdk.AudioInputStreamFormat = new sdk.AudioInputStreamFormat();
        return f;
    }

    /**
     * Closes the audio input stream.
     */
    public close(): void {
    }
}
