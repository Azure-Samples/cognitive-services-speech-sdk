//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { AudioInputStream, AudioStreamFormat, PullAudioInputStreamCallback } from "../Exports";

/*
 * Represents audio input stream used for custom audio input configurations.
 */
export abstract class PullAudioInputStream extends AudioInputStream {
    protected constructor() { super(); }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16Khz 16bit mono PCM).
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method (currently only support 16Khz 16bit mono PCM).
     * @return The push audio input stream being created.
     */
    public static create(callback: PullAudioInputStreamCallback, format?: AudioStreamFormat): PullAudioInputStream {
        // return new PullAudioInputStream(com.microsoft.cognitiveservices.speech.internal.AudioInputStream.CreatePullStream(callback.getAdapter()), callback);
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
export class PullAudioInputStreamImpl extends PullAudioInputStream {
    private callback: PullAudioInputStreamCallback;
    private format: AudioStreamFormat;

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16Khz 16bit mono PCM).
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method (currently only support 16Khz 16bit mono PCM).
     * @return The push audio input stream being created.
     */
    public constructor(callback: PullAudioInputStreamCallback, format?: AudioStreamFormat) {
        super();
        if (undefined === format) {
            this.format = AudioStreamFormat.getDefaultInputFormat();
        } else {
            this.format = format;
        }

        this.callback = callback;
    }

    public Read(buffer: number[]): number {
        return this.callback.read(buffer);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public close(): void {
        this.callback.close();
    }
}
