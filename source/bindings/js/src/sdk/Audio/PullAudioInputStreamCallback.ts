//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

 /**
  * An abstract base class that defines callback methods (read() and close()) for custom audio input streams).
  * @class PullAudioInputStreamCallback
  */
export abstract class PullAudioInputStreamCallback {

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @member PullAudioInputStreamCallback.prototype.read
     * @param dataBuffer The byte array to store the read data.
     * @returns the number of bytes have been read.
     */
    public abstract read(dataBuffer: ArrayBuffer): number;

    /**
     * Closes the audio input stream.
     * @member PullAudioInputStreamCallback.prototype.close
     */
    public abstract close(): void;
}
