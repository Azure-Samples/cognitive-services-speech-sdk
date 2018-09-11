//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

 /**
  * An abstract base class that defines callback methods (read() and close()) for custom audio input streams).
  * @class
  */
export abstract class PullAudioInputStreamCallback {

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @member
     * @param dataBuffer The byte array to store the read data.
     * @returns the number of bytes have been read.
     */
    public abstract read(dataBuffer: number[]): number;

    /**
     * Closes the audio input stream.
     * @member
     */
    public abstract close(): void;
}
