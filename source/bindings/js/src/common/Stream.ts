import { InvalidOperationError } from "./Error";
import { CreateNoDashGuid } from "./Guid";
import { IStringDictionary } from "./IDictionary";
import { Promise } from "./Promise";
import { Queue } from "./Queue";
import { IStreamChunk } from "./Stream";

export interface IStreamChunk<TBuffer> {
    IsEnd: boolean;
    Buffer: TBuffer;
}

export class Stream<TBuffer> {
    private id: string;
    private readerIdCounter: number = 1;
    private streambuffer: Array<IStreamChunk<TBuffer>>;
    private isEnded: boolean = false;
    private readerQueues: IStringDictionary<Queue<IStreamChunk<TBuffer>>>;

    public constructor(streamId?: string) {
        this.id = streamId ? streamId : CreateNoDashGuid();
        this.streambuffer = [];
        this.readerQueues = {};
    }

    public get IsClosed(): boolean {
        return this.isEnded;
    }

    public get Id(): string {
        return this.id;
    }

    public Write = (buffer: TBuffer): void => {
        this.ThrowIfClosed();
        this.WriteStreamChunk({
            Buffer: buffer,
            IsEnd: false,
        });
    }

    public GetReader = (): StreamReader<TBuffer> => {
        const readerId = this.readerIdCounter;
        this.readerIdCounter++;
        const readerQueue = new Queue<IStreamChunk<TBuffer>>();
        const currentLength = this.streambuffer.length;
        this.readerQueues[readerId] = readerQueue;
        for (let i = 0; i < currentLength; i++) {
            readerQueue.Enqueue(this.streambuffer[i]);
        }
        return new StreamReader(
            this.id,
            readerQueue,
            () => {
                delete this.readerQueues[readerId];
            });
    }

    public Close = (): void => {
        if (!this.isEnded) {
            this.WriteStreamChunk({
                Buffer: null,
                IsEnd: true,
            });
            this.isEnded = true;
        }
    }

    private WriteStreamChunk = (streamChunk: IStreamChunk<TBuffer>): void => {
        this.ThrowIfClosed();
        this.streambuffer.push(streamChunk);
        for (const readerId in this.readerQueues) {
            if (!this.readerQueues[readerId].IsDisposed()) {
                try {
                    this.readerQueues[readerId].Enqueue(streamChunk);
                } catch (e) {
                    // Do nothing
                }
            }
        }
    }

    private ThrowIfClosed = (): void => {
        if (this.isEnded) {
            throw new InvalidOperationError("Stream closed");
        }
    }
}

// tslint:disable-next-line:max-classes-per-file
export class StreamReader<TBuffer> {
    private readerQueue: Queue<IStreamChunk<TBuffer>>;
    private onClose: () => void;
    private isClosed: boolean = false;
    private streamId: string;

    public constructor(streamId: string, readerQueue: Queue<IStreamChunk<TBuffer>>, onClose: () => void) {
        this.readerQueue = readerQueue;
        this.onClose = onClose;
        this.streamId = streamId;
    }

    public get IsClosed(): boolean {
        return this.isClosed;
    }

    public get StreamId(): string {
        return this.streamId;
    }

    public Read = (): Promise<IStreamChunk<TBuffer>> => {
        if (this.IsClosed) {
            throw new InvalidOperationError("StreamReader closed");
        }

        return this.readerQueue
            .Dequeue()
            .OnSuccessContinueWith((streamChunk: IStreamChunk<TBuffer>) => {
                if (streamChunk.IsEnd) {
                    this.readerQueue.Dispose("End of stream reached");
                }

                return streamChunk;
            });
    }

    public Close = (): void => {
        if (!this.isClosed) {
            this.isClosed = true;
            this.readerQueue.Dispose("StreamReader closed");
            this.onClose();
        }
    }
}
