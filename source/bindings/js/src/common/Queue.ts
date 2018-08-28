import { InvalidOperationError, ObjectDisposedError } from "./Error";
import { IDetachable } from "./IDetachable";
import { IDisposable } from "./IDisposable";
import { List } from "./List";
import { Deferred, Promise, PromiseHelper } from "./Promise";

export interface IQueue<TItem> extends IDisposable {
    Enqueue(item: TItem): void;
    EnqueueFromPromise(promise: Promise<TItem>): void;
    Dequeue(): Promise<TItem>;
    Peek(): Promise<TItem>;
    Length(): number;
}

enum SubscriberType {
    Dequeue,
    Peek,
}

export class Queue<TItem> implements IQueue<TItem> {
    private promiseStore: List<Promise<TItem>> = new List<Promise<TItem>>();
    private list: List<TItem>;
    private detachables: IDetachable[];
    private subscribers: List<{ type: SubscriberType, deferral: Deferred<TItem> }>;
    private isDrainInProgress: boolean = false;
    private isDisposing: boolean = false;
    private disposeReason: string = null;

    public constructor(list?: List<TItem>) {
        this.list = list ? list : new List<TItem>();
        this.detachables = [];
        this.subscribers = new List<{ type: SubscriberType, deferral: Deferred<TItem> }>();
        this.detachables.push(this.list.OnAdded(this.Drain));
    }

    public Enqueue = (item: TItem): void => {
        this.ThrowIfDispose();
        this.EnqueueFromPromise(PromiseHelper.FromResult(item));
    }

    public EnqueueFromPromise = (promise: Promise<TItem>): void => {
        this.ThrowIfDispose();
        this.promiseStore.Add(promise);
        promise.Finally(() => {
            while (this.promiseStore.Length() > 0) {
                if (!this.promiseStore.First().Result().IsCompleted) {
                    break;
                } else {
                    const p = this.promiseStore.RemoveFirst();
                    if (!p.Result().IsError) {
                        this.list.Add(p.Result().Result);
                    } else {
                        // TODO: Log as warning.
                    }
                }
            }
        });
    }

    public Dequeue = (): Promise<TItem> => {
        this.ThrowIfDispose();
        const deferredSubscriber = new Deferred<TItem>();
        this.subscribers.Add({ deferral: deferredSubscriber, type: SubscriberType.Dequeue });
        this.Drain();
        return deferredSubscriber.Promise();
    }

    public Peek = (): Promise<TItem> => {
        this.ThrowIfDispose();
        const deferredSubscriber = new Deferred<TItem>();
        this.subscribers.Add({ deferral: deferredSubscriber, type: SubscriberType.Peek });
        this.Drain();
        return deferredSubscriber.Promise();
    }

    public Length = (): number => {
        this.ThrowIfDispose();
        return this.list.Length();
    }

    public IsDisposed = (): boolean => {
        return this.subscribers == null;
    }

    public DrainAndDispose = (pendingItemProcessor: (pendingItemInQueue: TItem) => void, reason?: string): Promise<boolean> => {
        if (!this.IsDisposed() && !this.isDisposing) {
            this.disposeReason = reason;
            this.isDisposing = true;
            while (this.subscribers.Length() > 0) {
                const subscriber = this.subscribers.RemoveFirst();
                // TODO: this needs work (Resolve(null) instead?).
                subscriber.deferral.Reject("Disposed");
            }

            for (const detachable of this.detachables) {
                detachable.Detach();
            }

            if (this.promiseStore.Length() > 0 && pendingItemProcessor) {
                return PromiseHelper
                    .WhenAll(this.promiseStore.ToArray())
                    .ContinueWith(() => {
                        this.subscribers = null;
                        this.list.ForEach((item: TItem, index: number): void => {
                            pendingItemProcessor(item);
                        });
                        this.list = null;
                        return true;
                    });
            } else {
                this.subscribers = null;
                this.list = null;
            }
        }

        return PromiseHelper.FromResult(true);
    }

    public Dispose = (reason?: string): void => {
        this.DrainAndDispose(null, reason);
    }

    private Drain = (): void => {
        if (!this.isDrainInProgress && !this.isDisposing) {
            this.isDrainInProgress = true;

            while (this.list.Length() > 0 && this.subscribers.Length() > 0 && !this.isDisposing) {
                const subscriber = this.subscribers.RemoveFirst();
                if (subscriber.type === SubscriberType.Peek) {
                    subscriber.deferral.Resolve(this.list.First());
                } else {
                    const dequeuedItem = this.list.RemoveFirst();
                    subscriber.deferral.Resolve(dequeuedItem);
                }
            }

            this.isDrainInProgress = false;
        }
    }

    private ThrowIfDispose = (): void => {
        if (this.IsDisposed()) {
            if (this.disposeReason) {
                throw new InvalidOperationError(this.disposeReason);
            }

            throw new ObjectDisposedError("Queue");
        } else if (this.isDisposing) {
            throw new InvalidOperationError("Queue disposing");
        }
    }
}
