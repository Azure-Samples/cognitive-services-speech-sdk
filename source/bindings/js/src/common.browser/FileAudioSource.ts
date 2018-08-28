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
    CreateNoDashGuid,
    Events,
    EventSource,
    IAudioSource,
    IAudioStreamNode,
    IStringDictionary,
    PlatformEvent,
    Promise,
    PromiseHelper,
    Stream,
    StreamReader,
} from "../common/Exports";

import { Timer } from "../common.browser/Exports";

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

    private streams: IStringDictionary<Stream<ArrayBuffer>> = {};

    private id: string;

    private events: EventSource<AudioSourceEvent>;

    private file: File;

    public constructor(file: File, audioSourceId?: string) {
        this.id = audioSourceId ? audioSourceId : CreateNoDashGuid();
        this.events = new EventSource<AudioSourceEvent>();
        this.file = file;
    }

    public TurnOn = (): Promise<boolean> => {
        if (typeof FileReader === "undefined") {
            const errorMsg = "Browser does not support FileReader.";
            this.OnEvent(new AudioSourceErrorEvent(errorMsg, "")); // initialization error - no streamid at this point
            return PromiseHelper.FromError<boolean>(errorMsg);
        } else if (this.file.name.lastIndexOf(".wav") !== this.file.name.length - 4) {
            const errorMsg = this.file.name + " is not supported. Only WAVE files are allowed at the moment.";
            this.OnEvent(new AudioSourceErrorEvent(errorMsg, ""));
            return PromiseHelper.FromError<boolean>(errorMsg);
        } else if (this.file.size > FileAudioSource.MAX_SIZE) {
            const errorMsg = this.file.name + " exceeds the maximum allowed file size (" + FileAudioSource.MAX_SIZE + ").";
            this.OnEvent(new AudioSourceErrorEvent(errorMsg, ""));
            return PromiseHelper.FromError<boolean>(errorMsg);
        }

        this.OnEvent(new AudioSourceInitializingEvent(this.id)); // no stream id
        this.OnEvent(new AudioSourceReadyEvent(this.id));
        return PromiseHelper.FromResult(true);
    }

    public Id = (): string => {
        return this.id;
    }

    public Attach = (audioNodeId: string): Promise<IAudioStreamNode> => {
        this.OnEvent(new AudioStreamNodeAttachingEvent(this.id, audioNodeId));

        return this.Upload(audioNodeId).OnSuccessContinueWith<IAudioStreamNode>(
            (streamReader: StreamReader<ArrayBuffer>) => {
                this.OnEvent(new AudioStreamNodeAttachedEvent(this.id, audioNodeId));
                return {
                    Detach: () => {
                        streamReader.Close();
                        delete this.streams[audioNodeId];
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

    public Detach = (audioNodeId: string): void => {
        if (audioNodeId && this.streams[audioNodeId]) {
            this.streams[audioNodeId].Close();
            delete this.streams[audioNodeId];
            this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
        }
    }

    public TurnOff = (): Promise<boolean> => {
        for (const streamId in this.streams) {
            if (streamId) {
                const stream = this.streams[streamId];
                if (stream && !stream.IsClosed) {
                    stream.Close();
                }
            }
        }

        this.OnEvent(new AudioSourceOffEvent(this.id)); // no stream now
        return PromiseHelper.FromResult(true);
    }

    public get Events(): EventSource<AudioSourceEvent> {
        return this.events;
    }

    private Upload = (audioNodeId: string): Promise<StreamReader<ArrayBuffer>> => {
        return this.TurnOn()
            .OnSuccessContinueWith<StreamReader<ArrayBuffer>>((_: boolean) => {
                const stream = new Stream<ArrayBuffer>(audioNodeId);

                this.streams[audioNodeId] = stream;

                const reader: FileReader = new FileReader();

                let startOffset = 0;
                let endOffset = FileAudioSource.CHUNK_SIZE;
                let lastWriteTimestamp = 0;

                const processNextChunk = (event: Event): void => {
                    if (stream.IsClosed) {
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

                    stream.Write(reader.result);
                    lastWriteTimestamp = Date.now();

                    if (endOffset < this.file.size) {
                        startOffset = endOffset;
                        endOffset = Math.min(endOffset + FileAudioSource.CHUNK_SIZE, this.file.size);
                        const chunk = this.file.slice(startOffset, endOffset);
                        reader.readAsArrayBuffer(chunk);
                    } else {
                        // we've written the entire file to the output stream, can close it now.
                        stream.Close();
                    }
                };

                reader.onload = processNextChunk;

                reader.onerror = (event: ErrorEvent) => {
                    const errorMsg = `Error occurred while processing '${this.file.name}'. ${event.error}`;
                    this.OnEvent(new AudioStreamNodeErrorEvent(this.id, audioNodeId, event.error));
                    throw new Error(errorMsg);
                };

                const chunk = this.file.slice(startOffset, endOffset);
                reader.readAsArrayBuffer(chunk);

                return stream.GetReader();
            });
    }

    private OnEvent = (event: AudioSourceEvent): void => {
        this.events.OnEvent(event);
        Events.Instance.OnEvent(event);
    }
}
