// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { AudioStreamFormatImpl } from "../../src/sdk/Audio/AudioStreamFormat";
import { AudioStreamFormat } from "../../src/sdk/Exports";
import { Timer } from "../common.browser/Exports";
import {
    AudioSourceErrorEvent,
    AudioSourceEvent,
    AudioSourceInitializingEvent,
    AudioSourceOffEvent,
    AudioSourceReadyEvent,
    AudioStreamNodeAttachedEvent,
    AudioStreamNodeAttachingEvent,
    AudioStreamNodeDetachedEvent,
    AudioStreamNodeErrorEvent,
    createNoDashGuid,
    Events,
    EventSource,
    IAudioSource,
    IAudioStreamNode,
    IStringDictionary,
    Promise,
    PromiseHelper,
    Stream,
    StreamReader,
} from "../common/Exports";

export class FileAudioSource implements IAudioSource {

    // Recommended sample rate (bytes/second).
    private static readonly SAMPLE_RATE: number = 16000 * 2; // 16 kHz * 16 bits

    // We should stream audio at no faster than 2x real-time (i.e., send five chunks
    // per second, with the chunk size == sample rate in bytes per second * 2 / 5).
    private static readonly CHUNK_SIZE: number = FileAudioSource.SAMPLE_RATE * 2 / 5;

    private static readonly UPLOAD_INTERVAL: number = 200; // milliseconds

    // 10 seconds of audio in bytes =
    // sample rate (bytes/second) * 600 (seconds) + 44 (size of the wave header).
    private static readonly MAX_SIZE: number = FileAudioSource.SAMPLE_RATE * 600 + 44;

    private static readonly FILEFORMAT: AudioStreamFormatImpl = AudioStreamFormat.getWaveFormatPCM(16000, 16, 1) as AudioStreamFormatImpl;

    private privStreams: IStringDictionary<Stream<ArrayBuffer>> = {};

    private privId: string;

    private privEvents: EventSource<AudioSourceEvent>;

    private privFile: File;

    public constructor(file: File, audioSourceId?: string) {
        this.privId = audioSourceId ? audioSourceId : createNoDashGuid();
        this.privEvents = new EventSource<AudioSourceEvent>();
        this.privFile = file;
    }

    public get format(): AudioStreamFormat {
        return FileAudioSource.FILEFORMAT;
    }

    public turnOn = (): Promise<boolean> => {
        if (typeof FileReader === "undefined") {
            const errorMsg = "Browser does not support FileReader.";
            this.onEvent(new AudioSourceErrorEvent(errorMsg, "")); // initialization error - no streamid at this point
            return PromiseHelper.fromError<boolean>(errorMsg);
        } else if (this.privFile.name.lastIndexOf(".wav") !== this.privFile.name.length - 4) {
            const errorMsg = this.privFile.name + " is not supported. Only WAVE files are allowed at the moment.";
            this.onEvent(new AudioSourceErrorEvent(errorMsg, ""));
            return PromiseHelper.fromError<boolean>(errorMsg);
        } else if (this.privFile.size > FileAudioSource.MAX_SIZE) {
            const errorMsg = this.privFile.name + " exceeds the maximum allowed file size (" + FileAudioSource.MAX_SIZE + ").";
            this.onEvent(new AudioSourceErrorEvent(errorMsg, ""));
            return PromiseHelper.fromError<boolean>(errorMsg);
        }

        this.onEvent(new AudioSourceInitializingEvent(this.privId)); // no stream id
        this.onEvent(new AudioSourceReadyEvent(this.privId));
        return PromiseHelper.fromResult(true);
    }

    public id = (): string => {
        return this.privId;
    }

    public attach = (audioNodeId: string): Promise<IAudioStreamNode> => {
        this.onEvent(new AudioStreamNodeAttachingEvent(this.privId, audioNodeId));

        return this.upload(audioNodeId).onSuccessContinueWith<IAudioStreamNode>(
            (streamReader: StreamReader<ArrayBuffer>) => {
                this.onEvent(new AudioStreamNodeAttachedEvent(this.privId, audioNodeId));
                return {
                    detach: () => {
                        streamReader.close();
                        delete this.privStreams[audioNodeId];
                        this.onEvent(new AudioStreamNodeDetachedEvent(this.privId, audioNodeId));
                        this.turnOff();
                    },
                    id: () => {
                        return audioNodeId;
                    },
                    read: () => {
                        return streamReader.read();
                    },
                };
            });
    }

    public detach = (audioNodeId: string): void => {
        if (audioNodeId && this.privStreams[audioNodeId]) {
            this.privStreams[audioNodeId].close();
            delete this.privStreams[audioNodeId];
            this.onEvent(new AudioStreamNodeDetachedEvent(this.privId, audioNodeId));
        }
    }

    public turnOff = (): Promise<boolean> => {
        for (const streamId in this.privStreams) {
            if (streamId) {
                const stream = this.privStreams[streamId];
                if (stream && !stream.isClosed) {
                    stream.close();
                }
            }
        }

        this.onEvent(new AudioSourceOffEvent(this.privId)); // no stream now
        return PromiseHelper.fromResult(true);
    }

    public get events(): EventSource<AudioSourceEvent> {
        return this.privEvents;
    }

    private upload = (audioNodeId: string): Promise<StreamReader<ArrayBuffer>> => {
        return this.turnOn()
            .onSuccessContinueWith<StreamReader<ArrayBuffer>>((_: boolean) => {
                const stream = new Stream<ArrayBuffer>(audioNodeId);

                this.privStreams[audioNodeId] = stream;

                const reader: FileReader = new FileReader();

                let startOffset = 0;
                let endOffset = FileAudioSource.CHUNK_SIZE;
                let lastWriteTimestamp = 0;

                const processNextChunk = (event: Event): void => {
                    if (stream.isClosed) {
                        return; // output stream was closed (somebody called TurnOff). We're done here.
                    }

                    if (lastWriteTimestamp !== 0) {
                        const delay = Date.now() - lastWriteTimestamp;
                        if (delay < FileAudioSource.UPLOAD_INTERVAL) {
                            // It's been less than the "upload interval" since we've uploaded the
                            // last chunk. Schedule the next upload to make sure that we're sending
                            // upstream roughly one chunk per upload interval.
                            new Timer(FileAudioSource.UPLOAD_INTERVAL - delay, processNextChunk).start();
                            return;
                        }
                    }

                    stream.write(reader.result as ArrayBuffer);
                    lastWriteTimestamp = Date.now();

                    if (endOffset < this.privFile.size) {
                        startOffset = endOffset;
                        endOffset = Math.min(endOffset + FileAudioSource.CHUNK_SIZE, this.privFile.size);
                        const chunk = this.privFile.slice(startOffset, endOffset);
                        reader.readAsArrayBuffer(chunk);
                    } else {
                        // we've written the entire file to the output stream, can close it now.
                        stream.close();
                    }
                };

                reader.onload = processNextChunk;

                reader.onerror = (event: ErrorEvent) => {
                    const errorMsg = `Error occurred while processing '${this.privFile.name}'. ${event.error}`;
                    this.onEvent(new AudioStreamNodeErrorEvent(this.privId, audioNodeId, event.error));
                    throw new Error(errorMsg);
                };

                const chunk = this.privFile.slice(startOffset, endOffset);
                reader.readAsArrayBuffer(chunk);

                return stream.getReader();
            });
    }

    private onEvent = (event: AudioSourceEvent): void => {
        this.privEvents.onEvent(event);
        Events.instance.onEvent(event);
    }
}
