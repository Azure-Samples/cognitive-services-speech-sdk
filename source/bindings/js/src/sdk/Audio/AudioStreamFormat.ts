// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

/**
 * Represents audio stream format used for custom audio input configurations.
 * @class AudioStreamFormat
 */
export abstract class AudioStreamFormat {
    /**
     * Creates an audio stream format object representing the default audio stream
     * format (16KHz 16bit mono PCM).
     * @member AudioStreamFormat.getDefaultInputFormat
     * @function
     * @public
     * @returns {AudioStreamFormat} The audio stream format being created.
     */
    public static getDefaultInputFormat(): AudioStreamFormat {
        return AudioStreamFormatImpl.getDefaultInputFormat();
    }

    /**
     * Creates an audio stream format object with the specified pcm waveformat characteristics.
     * @member AudioStreamFormat.getWaveFormatPCM
     * @function
     * @public
     * @param {number} samplesPerSecond - Sample rate, in samples per second (Hertz).
     * @param {number} bitsPerSample - Bits per sample, typically 16.
     * @param {number} channels - Number of channels in the waveform-audio data. Monaural data
     *        uses one channel and stereo data uses two channels.
     * @returns {AudioStreamFormat} The audio stream format being created.
     */
    public static getWaveFormatPCM(samplesPerSecond: number, bitsPerSample: number, channels: number): AudioStreamFormat {
        return new AudioStreamFormatImpl(samplesPerSecond, bitsPerSample, channels);
    }

    /**
     * Explicitly frees any external resource attached to the object
     * @member AudioStreamFormat.prototype.close
     * @function
     * @public
     */
    public abstract close(): void;
}

/**
 * @private
 * @class AudioStreamFormatImpl
 */
// tslint:disable-next-line:max-classes-per-file
export class AudioStreamFormatImpl extends AudioStreamFormat {
    /**
     * Creates an instance with the given values.
     * @constructor
     * @param {number} samplesPerSec - Samples per second.
     * @param {number} bitsPerSample - Bits per sample.
     * @param {number} channels - Number of channels.
     */
    public constructor(samplesPerSec: number = 16000, bitsPerSample: number = 16, channels: number = 1) {
        super();
        this.formatTag = 1;
        this.bitsPerSample = bitsPerSample;
        this.samplesPerSec = samplesPerSec;
        this.channels = channels;
        this.avgBytesPerSec = this.samplesPerSec * this.channels * (this.bitsPerSample / 8);
        this.blockAlign = this.channels * Math.max(this.bitsPerSample, 8);
    }

    /**
     * Retrieves the default input format.
     * @member AudioStreamFormatImpl.getDefaultInputFormat
     * @function
     * @public
     * @returns {AudioStreamFormatImpl} The default input format.
     */
    public static getDefaultInputFormat(): AudioStreamFormatImpl {
        return new AudioStreamFormatImpl();
    }

    /**
     * Closes the configuration object.
     * @member AudioStreamFormatImpl.prototype.close
     * @function
     * @public
     */
    public close(): void { return; }

    /**
     * The format of the audio, valid values: 1 (PCM)
     * @member AudioStreamFormatImpl.prototype.formatTag
     * @function
     * @public
     */
    public formatTag: number;

    /**
     * The number of channels, valid values: 1 (Mono).
     * @member AudioStreamFormatImpl.prototype.channels
     * @function
     * @public
     */
    public channels: number;

    /**
     * The sample rate, valid values: 16000.
     * @member AudioStreamFormatImpl.prototype.samplesPerSec
     * @function
     * @public
     */
    public samplesPerSec: number;

    /**
     * The bits per sample, valid values: 16
     * @member AudioStreamFormatImpl.prototype.b
     * @function
     * @public
     */
    public bitsPerSample: number;

    /**
     * Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
     * @member AudioStreamFormatImpl.prototype.avgBytesPerSec
     * @function
     * @public
     */
    public avgBytesPerSec: number;

    /**
     * The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
     * @member AudioStreamFormatImpl.prototype.blockAlign
     * @function
     * @public
     */
    public blockAlign: number;
}
