//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { FileAudioSource, MicAudioSource, PcmRecorder } from "../../../common.browser/Exports";
import { AudioSourceEvent, EventSource, IAudioSource, IAudioStreamNode, Promise } from "../../../common/exports";
import { AudioInputStream, PullAudioInputStreamCallback } from "../Exports";

/**
 * Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
 */

export abstract class AudioConfig {
    /**
     * Creates an AudioConfig object representing the default microphone on the system.
     * @return The audio input configuration being created.
     */
    public static fromDefaultMicrophoneInput(): AudioConfig {
        const pcmRecorder = new PcmRecorder();
        return new AudioConfigImpl(new MicAudioSource(pcmRecorder));
    }

    /**
     * Creates an AudioConfig object representing the specified file.
     * @param fileName Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @return The audio input configuration being created.
     */
    public static fromWavFileInput(file: File): AudioConfig {
        return new AudioConfigImpl(new FileAudioSource(file));
    }

    /**
     * Creates an AudioConfig object representing the specified stream.
     * @param audioStream Specifies the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @param callback Specifies the pull audio input stream callback. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @return The audio input configuration being created.
     */
    public static fromStreamInput(audioStream: AudioInputStream | PullAudioInputStreamCallback): AudioConfig {
        throw new Error("NYI");
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public abstract close(): void;
}

/**
 * Represents audio input stream used for custom audio input configurations.
 */
// tslint:disable-next-line:max-classes-per-file
export class AudioConfigImpl extends AudioConfig implements IAudioSource {
    private source: IAudioSource;

    public constructor(source: IAudioSource) {
        super();
        this.source = source;
    }

    public close(): void {
        this.source.TurnOff();
    }

    public Id(): string {
        return this.source.Id();
    }

    public TurnOn(): Promise<boolean> {
        return this.source.TurnOn();
    }

    public Attach(audioNodeId: string): Promise<IAudioStreamNode> {
        return this.source.Attach(audioNodeId);
    }

    public Detach(audioNodeId: string): void {
        return this.Detach(audioNodeId);
    }

    public TurnOff(): Promise<boolean> {
        return this.source.TurnOff();
    }

    public get Events(): EventSource<AudioSourceEvent> {
        return this.source.Events;
    }
}
