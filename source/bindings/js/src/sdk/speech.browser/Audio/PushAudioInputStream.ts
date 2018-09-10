//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { AudioInputStream, AudioStreamFormat } from "../Exports";
import { AudioStreamFormatImpl } from "./AudioStreamFormat";
/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 */
export abstract class PushAudioInputStream extends AudioInputStream {

    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
     * @return The push audio input stream being created.
     */
    public static create(format?: AudioStreamFormat): PushAudioInputStream {
        return new PushAudioInputStreamImpl(format);
    }

    /**
     * Writes the audio data specified by making an internal copy of the data.
     * @param dataBuffer The audio buffer of which this function will make a copy.
     */
    public abstract write(dataBuffer: number[]): void;

    /**
     * Closes the stream.
     */
    public abstract close(): void;
}

//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 */
// tslint:disable-next-line:max-classes-per-file
export class PushAudioInputStreamImpl extends PushAudioInputStream {

    private format: AudioStreamFormat;

    public constructor(format?: AudioStreamFormat) {
        super();
        if (format === undefined) {
            this.format = AudioStreamFormatImpl.getDefaultInputFormat();
        } else {
            this.format = format;
        }
    }

    /**
     * Writes the audio data specified by making an internal copy of the data.
     * @param dataBuffer The audio buffer of which this function will make a copy.
     */
    public write(dataBuffer: number[]): void {

        // Store data in buffer....
        throw new Error("NYI");
    }

    public Read(buffer: number[]): number {
        // Read from the mythical buffers write wrote into.
        return 0;
    }

    /**
     * Closes the stream.
     */
    public close(): void {
        return;
    }
}
