//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

export class AudioInputStreamFormat {
    /**
     * The format of the audio, valid values: 1 (PCM)
     */
    public formatTag: number;

    /**
     * The number of channels, valid values: 1 (Mono).
     */
    public channels: number;

    /**
     * The sample rate, valid values: 16000.
     */
    public samplesPerSec: number;

    /**
     * Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
     */
    public avgBytesPerSec: number;

    /**
     * The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
     */
    public blockAlign: number;

    /**
     * The bits per sample, valid values: 16
     */
    public bitsPerSample: number;
}
