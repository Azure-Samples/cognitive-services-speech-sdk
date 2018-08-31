//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { AudioInputStreamFormat } from "./Exports";

/**
 * Defines audio input stream.
 */
export class AudioInputStream {
    private fileHolder: File;

    protected constructor(file?: File) {
        this.fileHolder = file;
    }

    public get file(): File {
        return this.fileHolder;
    }

    /**
     * Reads data from audio input stream into the data buffer.
     * The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @param dataBuffer The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    public read(dataBuffer: number[]): number {
        // implement your own
        return 0;
    }

    /**
     * Returns the format of this audio stream.
     * @return The format of the audio stream.
     */
    public get format(): AudioInputStreamFormat {
        // implement your own
        return undefined;
    }

    /**
     * Closes the audio input stream.
     */
    public close(): void {
        // implement your own
    }
}

// tslint:disable-next-line:max-classes-per-file
export class FileInputStream extends AudioInputStream {

    public constructor(file: File) {
        super(file);
    }
}
