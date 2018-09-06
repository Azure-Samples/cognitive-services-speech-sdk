//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as fs from "fs";
import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";

export class WaveFileAudioInputStream extends  sdk.AudioInputStream {

    public constructor(filename: string) {
        // obtain and open the line.
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
}
