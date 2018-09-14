//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { CreateNoDashGuid } from "../../../src/common/Guid";
import {
    AudioSourceEvent,
    AudioSourceInitializingEvent,
    AudioSourceReadyEvent,
    AudioStreamNodeAttachedEvent,
    AudioStreamNodeAttachingEvent,
    AudioStreamNodeDetachedEvent,
    Events,
    EventSource,
    IAudioSource,
    IAudioStreamNode,
    IStreamChunk,
    Promise,
    PromiseHelper,
    Stream,
    StreamReader,
} from "../../common/Exports";
import { AudioStreamFormat, PullAudioInputStreamCallback } from "../Exports";
import { AudioStreamFormatImpl } from "./AudioStreamFormat";

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
    public static createPullStream(callback: PullAudioInputStreamCallback, format?: AudioStreamFormat): PullAudioInputStream {
        return PullAudioInputStream.create(callback, format);
        // throw new Error("Oops");
    }

    /**
     * Explicitly frees any external resource attached to the object
     * @member
     */
    public abstract close(): void;
}

/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 * @class
 */
// tslint:disable-next-line:max-classes-per-file
export abstract class PushAudioInputStream extends AudioInputStream {

    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @member
     * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16KHz 16bit mono PCM).
     * @returns The push audio input stream being created.
     */
    public static create(format?: AudioStreamFormat): PushAudioInputStream {
        return new PushAudioInputStreamImpl(format);
    }

    /**
     * Writes the audio data specified by making an internal copy of the data.
     * @member
     * @param dataBuffer The audio buffer of which this function will make a copy.
     */
    public abstract write(dataBuffer: ArrayBuffer): void;

    /**
     * Closes the stream.
     * @member
     */
    public abstract close(): void;
}

/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 * @class
 */
// tslint:disable-next-line:max-classes-per-file
export class PushAudioInputStreamImpl extends PushAudioInputStream implements IAudioSource {

    private format: AudioStreamFormat;
    private id: string;
    private events: EventSource<AudioSourceEvent>;
    private stream: Stream<ArrayBuffer> = new Stream<ArrayBuffer>();

    /**
     * Creates and initalizes an instance with the given values.
     * @constructor
     * @param format - The audio stream format.
     */
    public constructor(format?: AudioStreamFormat) {
        super();
        if (format === undefined) {
            this.format = AudioStreamFormatImpl.getDefaultInputFormat();
        } else {
            this.format = format;
        }
        this.events = new EventSource<AudioSourceEvent>();
        this.id = CreateNoDashGuid();
    }

    /**
     * Writes the audio data specified by making an internal copy of the data.
     * @member
     * @param dataBuffer The audio buffer of which this function will make a copy.
     */
    public write(dataBuffer: ArrayBuffer): void {

        // Store data in buffer....
        this.stream.Write(dataBuffer);
    }

    /**
     * Closes the stream.
     * @member
     */
    public close(): void {
        this.stream.Close();
    }

    public Id(): string {
        return this.id;
    }

    public TurnOn(): Promise<boolean> {
        this.OnEvent(new AudioSourceInitializingEvent(this.id)); // no stream id
        this.OnEvent(new AudioSourceReadyEvent(this.id));
        return PromiseHelper.FromResult(true);
    }

    public Attach(audioNodeId: string): Promise<IAudioStreamNode> {
        this.OnEvent(new AudioStreamNodeAttachingEvent(this.id, audioNodeId));

        return this.TurnOn()
            .OnSuccessContinueWith<StreamReader<ArrayBuffer>>((_: boolean) => {
                // For now we support a single parallel reader of the pushed stream.
                // So we can simiply hand the stream to the recognizer and let it recognize.

                return this.stream.GetReader();
            })
            .OnSuccessContinueWith((streamReader: StreamReader<ArrayBuffer>) => {
                this.OnEvent(new AudioStreamNodeAttachedEvent(this.id, audioNodeId));

                return {
                    Detach: () => {
                        streamReader.Close();
                        this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
                        this.TurnOff();
                    },
                    Id: () => {
                        return audioNodeId;
                    },
                    Read: () => {
                        return streamReader.Read();
                    },
                };
            });
    }

    public Detach(audioNodeId: string): void {
        this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
    }

    public TurnOff(): Promise<boolean> {
        return PromiseHelper.FromResult(false);
    }

    public get Events(): EventSource<AudioSourceEvent> {
        return this.events;
    }

    private OnEvent = (event: AudioSourceEvent): void => {
        this.events.OnEvent(event);
        Events.Instance.OnEvent(event);
    }
}

/*
 * Represents audio input stream used for custom audio input configurations.
 * @class
 */
// tslint:disable-next-line:max-classes-per-file
export abstract class PullAudioInputStream extends AudioInputStream {
    /**
     * Creates and initializes and instance.
     * @constructor
     */
    protected constructor() { super(); }

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16KHz 16bit mono PCM).
     * @member
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method (currently only support 16KHz 16bit mono PCM).
     * @returns The push audio input stream being created.
     */
    public static create(callback: PullAudioInputStreamCallback, format?: AudioStreamFormat): PullAudioInputStream {
        return new PullAudioInputStreamImpl(callback, format);
    }

    /**
     * Explicitly frees any external resource attached to the object
     * @member
     */
    public abstract close(): void;

}

/**
 * Represents audio input stream used for custom audio input configurations.
 * @class
 */
// tslint:disable-next-line:max-classes-per-file
export class PullAudioInputStreamImpl extends PullAudioInputStream implements IAudioSource {

    private callback: PullAudioInputStreamCallback;
    private format: AudioStreamFormat;
    private id: string;
    private events: EventSource<AudioSourceEvent>;

    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16KHz 16bit mono PCM).
     * @member
     * @param callback The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param format The audio data format in which audio will be returned from the callback's read() method (currently only support 16KHz 16bit mono PCM).
     * @returns The push audio input stream being created.
     */
    public constructor(callback: PullAudioInputStreamCallback, format?: AudioStreamFormat) {
        super();
        if (undefined === format) {
            this.format = AudioStreamFormat.getDefaultInputFormat();
        } else {
            this.format = format;
        }
        this.events = new EventSource<AudioSourceEvent>();
        this.id = CreateNoDashGuid();
        this.callback = callback;
    }

    /**
     * Closes the stream.
     * @member
     */
    public close(): void {
        this.callback.close();
    }

    public Id(): string {
        return this.id;
    }

    public TurnOn(): Promise<boolean> {
        this.OnEvent(new AudioSourceInitializingEvent(this.id)); // no stream id
        this.OnEvent(new AudioSourceReadyEvent(this.id));
        return PromiseHelper.FromResult(true);
    }

    public Attach(audioNodeId: string): Promise<IAudioStreamNode> {
        this.OnEvent(new AudioStreamNodeAttachingEvent(this.id, audioNodeId));

        return this.TurnOn()
            .OnSuccessContinueWith((result: boolean) => {
                this.OnEvent(new AudioStreamNodeAttachedEvent(this.id, audioNodeId));

                return {
                    Detach: () => {
                        this.callback.close();
                        this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
                        this.TurnOff();
                    },
                    Id: () => {
                        return audioNodeId;
                    },
                    Read: (): Promise<IStreamChunk<ArrayBuffer>> => {
                        const readBuff: ArrayBuffer = new ArrayBuffer(4096);
                        this.callback.read(readBuff);

                        return PromiseHelper.FromResult<IStreamChunk<ArrayBuffer>>({
                            Buffer: readBuff,
                            IsEnd: false,
                        });
                    },
                };
            });

    }

    public Detach(audioNodeId: string): void {
        this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
    }

    public TurnOff(): Promise<boolean> {
        return PromiseHelper.FromResult(false);
    }

    public get Events(): EventSource<AudioSourceEvent> {
        return this.events;
    }

    private OnEvent = (event: AudioSourceEvent): void => {
        this.events.OnEvent(event);
        Events.Instance.OnEvent(event);
    }
}
