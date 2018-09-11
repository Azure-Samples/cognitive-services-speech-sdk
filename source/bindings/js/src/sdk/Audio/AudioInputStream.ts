//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { AudioStreamFormat, PullAudioInputStream, PullAudioInputStreamCallback, PushAudioInputStream } from "../Exports";

/**
 * Represents audio input stream used for custom audio input configurations.
 * @class
 */
export abstract class AudioInputStream {

    /**
     * Creates and initializes an instance.
     * @constructor
     */
    protected constructor() { }

    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @member
     * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16KHz 16bit mono PCM).
     * @returns The audio input stream being created.
     */
    public static createPushStream(format?: AudioStreamFormat): PushAudioInputStream {
        return PushAudioInputStream.create(format);
    }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
     * @member
     * @param callback The custom audio input object, derived from PullAudioInputStreamCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method (currently only support 16KHz 16bit mono PCM).
     * @returns The audio input stream being created.
     */
    public static createPullStream(callback: PullAudioInputStreamCallback, format: AudioStreamFormat): PullAudioInputStream {
        return PullAudioInputStream.create(callback, format);
    }

    /**
     * Explicitly frees any external resource attached to the object
     * @member
     */
    public abstract close(): void;
}
