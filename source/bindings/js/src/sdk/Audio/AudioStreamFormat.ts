//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Represents audio stream format used for custom audio input configurations.
 */
export abstract class AudioStreamFormat {
    /**
     * Creates an audio stream format object representing the default audio stream format (16Khz 16bit mono PCM).
     * @return The audio stream format being created.
     */
    public static getDefaultInputFormat(): AudioStreamFormat {
        return AudioStreamFormatImpl.getDefaultInputFormat();
    }

    /**
     * Creates an audio stream format object with the specified pcm waveformat characteristics.
     * @param samplesPerSecond Sample rate, in samples per second (hertz).
     * @param bitsPerSample Bits per sample, typically 16.
     * @param channels Number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels.
     * @return The audio stream format being created.
     */
    public static getWaveFormatPCM(samplesPerSecond: number, bitsPerSample: number, channels: number): AudioStreamFormat {
        return new AudioStreamFormatImpl(samplesPerSecond, bitsPerSample, channels);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public abstract close(): void;
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamFormatImpl extends AudioStreamFormat {
    public constructor(samplesPerSec: number = 16000, bitsPerSample: number = 16, channels: number = 1) {
        super();
        this.formatTag = 1;
        this.bitsPerSample = bitsPerSample;
        this.samplesPerSec = samplesPerSec;
        this.channels = channels;
        this.avgBytesPerSec = this.samplesPerSec * this.channels * this.bitsPerSample;
        this.blockAlign = this.channels * Math.max(this.bitsPerSample, 8);
    }

    public static getDefaultInputFormat(): AudioStreamFormatImpl {
        return new AudioStreamFormatImpl();
    }

    public close(): void { return; }

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
     * The bits per sample, valid values: 16
     */
    public bitsPerSample: number;

    /**
     * Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
     */
    public avgBytesPerSec: number;

    /**
     * The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
     */
    public blockAlign: number;
}
