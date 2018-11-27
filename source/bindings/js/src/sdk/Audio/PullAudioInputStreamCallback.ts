// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
 /**
  * An abstract base class that defines callback methods (read() and close()) for
  * custom audio input streams).
  * @class PullAudioInputStreamCallback
  */
export abstract class PullAudioInputStreamCallback {

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes
     * to be read is determined by the size of dataBuffer.
     * @member PullAudioInputStreamCallback.prototype.read
     * @function
     * @public
     * @param {ArrayBuffer} dataBuffer - The byte array to store the read data.
     * @returns {number} the number of bytes have been read.
     */
    public abstract read(dataBuffer: ArrayBuffer): number;

    /**
     * Closes the audio input stream.
     * @member PullAudioInputStreamCallback.prototype.close
     * @function
     * @public
     */
    public abstract close(): void;
}
