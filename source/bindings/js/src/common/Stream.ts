// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { InvalidOperationError } from "./Error";
import { createNoDashGuid } from "./Guid";
import { IStringDictionary } from "./IDictionary";
import { Promise } from "./Promise";
import { Queue } from "./Queue";
import { IStreamChunk } from "./Stream";

export interface IStreamChunk<TBuffer> {
    isEnd: boolean;
    buffer: TBuffer;
}

export class Stream<TBuffer> {
    private privId: string;
    private privReaderIdCounter: number = 1;
    private privStreambuffer: Array<IStreamChunk<TBuffer>>;
    private privIsEnded: boolean = false;
    private privReaderQueues: IStringDictionary<Queue<IStreamChunk<TBuffer>>>;

    public constructor(streamId?: string) {
        this.privId = streamId ? streamId : createNoDashGuid();
        this.privStreambuffer = [];
        this.privReaderQueues = {};
    }

    public get isClosed(): boolean {
        return this.privIsEnded;
    }

    public get id(): string {
        return this.privId;
    }

    public write = (buffer2: TBuffer): void => {
        this.throwIfClosed();
        this.writeStreamChunk({
            buffer: buffer2,
            isEnd: false,
        });
    }

    public getReader = (): StreamReader<TBuffer> => {
        const readerId = this.privReaderIdCounter;
        this.privReaderIdCounter++;
        const readerQueue = new Queue<IStreamChunk<TBuffer>>();
        const currentLength = this.privStreambuffer.length;
        this.privReaderQueues[readerId] = readerQueue;
        for (let i = 0; i < currentLength; i++) {
            readerQueue.enqueue(this.privStreambuffer[i]);
        }
        return new StreamReader(
            this.privId,
            readerQueue,
            () => {
                delete this.privReaderQueues[readerId];
            });
    }

    public close = (): void => {
        if (!this.privIsEnded) {
            this.writeStreamChunk({
                buffer: null,
                isEnd: true,
            });
            this.privIsEnded = true;
        }
    }

    private writeStreamChunk = (streamChunk: IStreamChunk<TBuffer>): void => {
        this.throwIfClosed();
        this.privStreambuffer.push(streamChunk);
        for (const readerId in this.privReaderQueues) {
            if (!this.privReaderQueues[readerId].isDisposed()) {
                try {
                    this.privReaderQueues[readerId].enqueue(streamChunk);
                } catch (e) {
                    // Do nothing
                }
            }
        }
    }

    private throwIfClosed = (): void => {
        if (this.privIsEnded) {
            throw new InvalidOperationError("Stream closed");
        }
    }
}

// tslint:disable-next-line:max-classes-per-file
export class StreamReader<TBuffer> {
    private privReaderQueue: Queue<IStreamChunk<TBuffer>>;
    private privOnClose: () => void;
    private privIsClosed: boolean = false;
    private privStreamId: string;

    public constructor(streamId: string, readerQueue: Queue<IStreamChunk<TBuffer>>, onClose: () => void) {
        this.privReaderQueue = readerQueue;
        this.privOnClose = onClose;
        this.privStreamId = streamId;
    }

    public get isClosed(): boolean {
        return this.privIsClosed;
    }

    public get streamId(): string {
        return this.privStreamId;
    }

    public read = (): Promise<IStreamChunk<TBuffer>> => {
        if (this.isClosed) {
            throw new InvalidOperationError("StreamReader closed");
        }

        return this.privReaderQueue
            .dequeue()
            .onSuccessContinueWith((streamChunk: IStreamChunk<TBuffer>) => {
                if (streamChunk.isEnd) {
                    this.privReaderQueue.dispose("End of stream reached");
                }

                return streamChunk;
            });
    }

    public close = (): void => {
        if (!this.privIsClosed) {
            this.privIsClosed = true;
            this.privReaderQueue.dispose("StreamReader closed");
            this.privOnClose();
        }
    }
}
