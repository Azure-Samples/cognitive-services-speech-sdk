// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { InvalidOperationError, ObjectDisposedError } from "./Error";
import { IDetachable } from "./IDetachable";
import { IDisposable } from "./IDisposable";
import { List } from "./List";
import { Deferred, Promise, PromiseHelper } from "./Promise";

export interface IQueue<TItem> extends IDisposable {
    enqueue(item: TItem): void;
    enqueueFromPromise(promise: Promise<TItem>): void;
    dequeue(): Promise<TItem>;
    peek(): Promise<TItem>;
    length(): number;
}

enum SubscriberType {
    Dequeue,
    Peek,
}

export class Queue<TItem> implements IQueue<TItem> {
    private privPromiseStore: List<Promise<TItem>> = new List<Promise<TItem>>();
    private privList: List<TItem>;
    private privDetachables: IDetachable[];
    private privSubscribers: List<{ type: SubscriberType, deferral: Deferred<TItem> }>;
    private privIsDrainInProgress: boolean = false;
    private privIsDisposing: boolean = false;
    private privDisposeReason: string = null;

    public constructor(list?: List<TItem>) {
        this.privList = list ? list : new List<TItem>();
        this.privDetachables = [];
        this.privSubscribers = new List<{ type: SubscriberType, deferral: Deferred<TItem> }>();
        this.privDetachables.push(this.privList.onAdded(this.drain));
    }

    public enqueue = (item: TItem): void => {
        this.throwIfDispose();
        this.enqueueFromPromise(PromiseHelper.fromResult(item));
    }

    public enqueueFromPromise = (promise: Promise<TItem>): void => {
        this.throwIfDispose();
        this.privPromiseStore.add(promise);
        promise.finally(() => {
            while (this.privPromiseStore.length() > 0) {
                if (!this.privPromiseStore.first().result().isCompleted) {
                    break;
                } else {
                    const p = this.privPromiseStore.removeFirst();
                    if (!p.result().isError) {
                        this.privList.add(p.result().result);
                    } else {
                        // TODO: Log as warning.
                    }
                }
            }
        });
    }

    public dequeue = (): Promise<TItem> => {
        this.throwIfDispose();
        const deferredSubscriber = new Deferred<TItem>();

        if (this.privSubscribers) {
            this.privSubscribers.add({ deferral: deferredSubscriber, type: SubscriberType.Dequeue });
            this.drain();
        }

        return deferredSubscriber.promise();
    }

    public peek = (): Promise<TItem> => {
        this.throwIfDispose();
        const deferredSubscriber = new Deferred<TItem>();

        const subs = this.privSubscribers;
        if (subs) {
            this.privSubscribers.add({ deferral: deferredSubscriber, type: SubscriberType.Peek });
            this.drain();
        }

        return deferredSubscriber.promise();
    }

    public length = (): number => {
        this.throwIfDispose();
        return this.privList.length();
    }

    public isDisposed = (): boolean => {
        return this.privSubscribers == null;
    }

    public drainAndDispose = (pendingItemProcessor: (pendingItemInQueue: TItem) => void, reason?: string): Promise<boolean> => {
        if (!this.isDisposed() && !this.privIsDisposing) {
            this.privDisposeReason = reason;
            this.privIsDisposing = true;

            const subs = this.privSubscribers;
            if (subs) {
                while (subs.length() > 0) {
                    const subscriber = subs.removeFirst();
                    // TODO: this needs work (Resolve(null) instead?).
                    subscriber.deferral.resolve(undefined);
                    // subscriber.deferral.reject("Disposed");
                }

                // note: this block assumes cooperative multitasking, i.e.,
                // between the if-statement and the assignment there are no
                // thread switches.
                // Reason is that between the initial const = this.; and this
                // point there is the derral.resolve() operation that might have
                // caused recursive calls to the Queue, especially, calling
                // Dispose() on the queue alredy (which would reset the var
                // here to null!).
                // That should generally hold true for javascript...
                if (this.privSubscribers === subs) {
                    this.privSubscribers = subs;
                }
            }

            for (const detachable of this.privDetachables) {
                detachable.detach();
            }

            if (this.privPromiseStore.length() > 0 && pendingItemProcessor) {
                return PromiseHelper
                    .whenAll(this.privPromiseStore.toArray())
                    .continueWith(() => {
                        this.privSubscribers = null;
                        this.privList.forEach((item: TItem, index: number): void => {
                            pendingItemProcessor(item);
                        });
                        this.privList = null;
                        return true;
                    });
            } else {
                this.privSubscribers = null;
                this.privList = null;
            }
        }

        return PromiseHelper.fromResult(true);
    }

    public dispose = (reason?: string): void => {
        this.drainAndDispose(null, reason);
    }

    private drain = (): void => {
        if (!this.privIsDrainInProgress && !this.privIsDisposing) {
            this.privIsDrainInProgress = true;

            const subs = this.privSubscribers;
            const lists = this.privList;
            if (subs && lists) {
                while (lists.length() > 0 && subs.length() > 0 && !this.privIsDisposing) {
                    const subscriber = subs.removeFirst();
                    if (subscriber.type === SubscriberType.Peek) {
                        subscriber.deferral.resolve(lists.first());
                    } else {
                        const dequeuedItem = lists.removeFirst();
                        subscriber.deferral.resolve(dequeuedItem);
                    }
                }

                // note: this block assumes cooperative multitasking, i.e.,
                // between the if-statement and the assignment there are no
                // thread switches.
                // Reason is that between the initial const = this.; and this
                // point there is the derral.resolve() operation that might have
                // caused recursive calls to the Queue, especially, calling
                // Dispose() on the queue alredy (which would reset the var
                // here to null!).
                // That should generally hold true for javascript...
                if (this.privSubscribers === subs) {
                    this.privSubscribers = subs;
                }

                // note: this block assumes cooperative multitasking, i.e.,
                // between the if-statement and the assignment there are no
                // thread switches.
                // Reason is that between the initial const = this.; and this
                // point there is the derral.resolve() operation that might have
                // caused recursive calls to the Queue, especially, calling
                // Dispose() on the queue alredy (which would reset the var
                // here to null!).
                // That should generally hold true for javascript...
                if (this.privList === lists) {
                    this.privList = lists;
                }
            }

            this.privIsDrainInProgress = false;
        }
    }

    private throwIfDispose = (): void => {
        if (this.isDisposed()) {
            if (this.privDisposeReason) {
                throw new InvalidOperationError(this.privDisposeReason);
            }

            throw new ObjectDisposedError("Queue");
        } else if (this.privIsDisposing) {
            throw new InvalidOperationError("Queue disposing");
        }
    }
}
