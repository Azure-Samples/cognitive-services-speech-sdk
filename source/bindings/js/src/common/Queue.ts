// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
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

        if (this.subscribers) {
            this.subscribers.Add({ deferral: deferredSubscriber, type: SubscriberType.Dequeue });
            this.Drain();
        }

        return deferredSubscriber.Promise();
    }

    public Peek = (): Promise<TItem> => {
        this.ThrowIfDispose();
        const deferredSubscriber = new Deferred<TItem>();

        const subs = this.subscribers;
        if (subs) {
            this.subscribers.Add({ deferral: deferredSubscriber, type: SubscriberType.Peek });
            this.Drain();
        }

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

            const subs = this.subscribers;
            if (subs) {
                while (subs.Length() > 0) {
                    const subscriber = subs.RemoveFirst();
                    // TODO: this needs work (Resolve(null) instead?).
                    subscriber.deferral.Resolve(undefined);
                    // subscriber.deferral.Reject("Disposed");
                }

                // note: this block assumes cooperative multitasking, i.e.,
                // between the if-statement and the assignment there are no
                // thread switches.
                // Reason is that between the initial const = this.; and this
                // point there is the derral.Resolve() operation that might have
                // caused recursive calls to the Queue, especially, calling
                // Dispose() on the queue alredy (which would reset the var
                // here to null!).
                // That should generally hold true for javascript...
                if (this.subscribers === subs) {
                    this.subscribers = subs;
                }
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

            const subs = this.subscribers;
            const lists = this.list;
            if (subs && lists) {
                while (lists.Length() > 0 && subs.Length() > 0 && !this.isDisposing) {
                    const subscriber = subs.RemoveFirst();
                    if (subscriber.type === SubscriberType.Peek) {
                        subscriber.deferral.Resolve(lists.First());
                    } else {
                        const dequeuedItem = lists.RemoveFirst();
                        subscriber.deferral.Resolve(dequeuedItem);
                    }
                }

                // note: this block assumes cooperative multitasking, i.e.,
                // between the if-statement and the assignment there are no
                // thread switches.
                // Reason is that between the initial const = this.; and this
                // point there is the derral.Resolve() operation that might have
                // caused recursive calls to the Queue, especially, calling
                // Dispose() on the queue alredy (which would reset the var
                // here to null!).
                // That should generally hold true for javascript...
                if (this.subscribers === subs) {
                    this.subscribers = subs;
                }

                // note: this block assumes cooperative multitasking, i.e.,
                // between the if-statement and the assignment there are no
                // thread switches.
                // Reason is that between the initial const = this.; and this
                // point there is the derral.Resolve() operation that might have
                // caused recursive calls to the Queue, especially, calling
                // Dispose() on the queue alredy (which would reset the var
                // here to null!).
                // That should generally hold true for javascript...
                if (this.list === lists) {
                    this.list = lists;
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
