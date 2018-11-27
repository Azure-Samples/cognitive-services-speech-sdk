// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { AudioStreamFormat } from "../../../src/sdk/Exports";
import { FileAudioSource, MicAudioSource, PcmRecorder } from "../../common.browser/Exports";
import { AudioSourceEvent, EventSource, IAudioSource, IAudioStreamNode, Promise } from "../../common/exports";
import { AudioInputStream, PullAudioInputStreamCallback } from "../Exports";
import { PullAudioInputStreamImpl, PushAudioInputStreamImpl } from "./AudioInputStream";

/**
 * Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
 * @class AudioConfig
 */
export abstract class AudioConfig {
    /**
     * Creates an AudioConfig object representing the default microphone on the system.
     * @member AudioConfig.fromDefaultMicrophoneInput
     * @function
     * @public
     * @returns {AudioConfig} The audio input configuration being created.
     */
    public static fromDefaultMicrophoneInput(): AudioConfig {
        const pcmRecorder = new PcmRecorder();
        return new AudioConfigImpl(new MicAudioSource(pcmRecorder));
    }

    /**
     * Creates an AudioConfig object representing the specified file.
     * @member AudioConfig.fromWavFileInput
     * @function
     * @public
     * @param {File} fileName - Specifies the audio input file. Currently, only WAV / PCM with 16-bit
     *        samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @returns {AudioConfig} The audio input configuration being created.
     */
    public static fromWavFileInput(file: File): AudioConfig {
        return new AudioConfigImpl(new FileAudioSource(file));
    }

    /**
     * Creates an AudioConfig object representing the specified stream.
     * @member AudioConfig.fromStreamInput
     * @function
     * @public
     * @param {AudioInputStream | PullAudioInputStreamCallback} audioStream - Specifies the custom audio input
     *        stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel
     *        (Mono) is supported.
     * @returns {AudioConfig} The audio input configuration being created.
     */
    public static fromStreamInput(audioStream: AudioInputStream | PullAudioInputStreamCallback): AudioConfig {
        if (audioStream instanceof PullAudioInputStreamCallback) {
            return new AudioConfigImpl(new PullAudioInputStreamImpl(audioStream as PullAudioInputStreamCallback));
        }

        if (audioStream instanceof AudioInputStream) {
            return new AudioConfigImpl(audioStream as PushAudioInputStreamImpl);
        }

        throw new Error("Not Supported Type");
    }

    /**
     * Explicitly frees any external resource attached to the object
     * @member AudioConfig.prototype.close
     * @function
     * @public
     */
    public abstract close(): void;
}

/**
 * Represents audio input stream used for custom audio input configurations.
 * @private
 * @class AudioConfigImpl
 */
// tslint:disable-next-line:max-classes-per-file
export class AudioConfigImpl extends AudioConfig implements IAudioSource {
    private privSource: IAudioSource;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {IAudioSource} source - An audio source.
     */
    public constructor(source: IAudioSource) {
        super();
        this.privSource = source;
    }

    /**
     * Format information for the audio
     */
    public get format(): AudioStreamFormat {
        return this.privSource.format;
    }

    /**
     * @member AudioConfigImpl.prototype.close
     * @function
     * @public
     */
    public close(): void {
        this.privSource.turnOff();
    }

    /**
     * @member AudioConfigImpl.prototype.id
     * @function
     * @public
     */
    public id(): string {
        return this.privSource.id();
    }

    /**
     * @member AudioConfigImpl.prototype.turnOn
     * @function
     * @public
     * @returns {Promise<boolean>} A promise.
     */
    public turnOn(): Promise<boolean> {
        return this.privSource.turnOn();
    }

    /**
     * @member AudioConfigImpl.prototype.attach
     * @function
     * @public
     * @param {string} audioNodeId - The audio node id.
     * @returns {Promise<IAudioStreamNode>} A promise.
     */
    public attach(audioNodeId: string): Promise<IAudioStreamNode> {
        return this.privSource.attach(audioNodeId);
    }

    /**
     * @member AudioConfigImpl.prototype.detach
     * @function
     * @public
     * @param {string} audioNodeId - The audio node id.
     */
    public detach(audioNodeId: string): void {
        return this.detach(audioNodeId);
    }

    /**
     * @member AudioConfigImpl.prototype.turnOff
     * @function
     * @public
     * @returns {Promise<boolean>} A promise.
     */
    public turnOff(): Promise<boolean> {
        return this.privSource.turnOff();
    }

    /**
     * @member AudioConfigImpl.prototype.events
     * @function
     * @public
     * @returns {EventSource<AudioSourceEvent>} An event source for audio events.
     */
    public get events(): EventSource<AudioSourceEvent> {
        return this.privSource.events;
    }
}
