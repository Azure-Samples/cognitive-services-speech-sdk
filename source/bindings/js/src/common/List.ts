// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ObjectDisposedError } from "./Error";
import { IDetachable } from "./IDetachable";
import { IStringDictionary } from "./IDictionary";
import { IDisposable } from "./IDisposable";

export interface IList<TItem> extends IDisposable {
    get(itemIndex: number): TItem;
    first(): TItem;
    last(): TItem;

    add(item: TItem): void;
    insertAt(index: number, item: TItem): void;

    removeFirst(): TItem;
    removeLast(): TItem;
    removeAt(index: number): TItem;
    remove(index: number, count: number): TItem[];
    clear(): void;

    length(): number;

    onAdded(addedCallback: () => void): IDetachable;
    onRemoved(removedCallback: () => void): IDetachable;
    onDisposed(disposedCallback: () => void): IDetachable;

    join(seperator?: string): string;

    toArray(): TItem[];

    any(callback?: (item: TItem, index: number) => boolean): boolean;
    all(callback: (item: TItem) => boolean): boolean;
    forEach(callback: (item: TItem, index: number) => void): void;
    select<T2>(callback: (item: TItem, index: number) => T2): List<T2>;
    where(callback: (item: TItem, index: number) => boolean): List<TItem>;
    orderBy(compareFn: (a: TItem, b: TItem) => number): List<TItem>;
    orderByDesc(compareFn: (a: TItem, b: TItem) => number): List<TItem>;
    clone(): List<TItem>;
    concat(list: List<TItem>): List<TItem>;
    concatArray(array: TItem[]): List<TItem>;
}

export class List<TItem> implements IList<TItem>  {
    private privList: TItem[];
    private privSubscriptionIdCounter: number = 0;
    private privAddSubscriptions: IStringDictionary<() => void> = {};
    private privRemoveSubscriptions: IStringDictionary<() => void> = {};
    private privDisposedSubscriptions: IStringDictionary<() => void> = {};
    private privDisposeReason: string = null;

    public constructor(list?: TItem[]) {
        this.privList = [];
        // copy the list rather than taking as is.
        if (list) {
            for (const item of list) {
                this.privList.push(item);
            }
        }
    }

    public get = (itemIndex: number): TItem => {
        this.throwIfDisposed();
        return this.privList[itemIndex];
    }

    public first = (): TItem => {
        return this.get(0);
    }

    public last = (): TItem => {
        return this.get(this.length() - 1);
    }

    public add = (item: TItem): void => {
        this.throwIfDisposed();
        this.insertAt(this.privList.length, item);
    }

    public insertAt = (index: number, item: TItem): void => {
        this.throwIfDisposed();
        if (index === 0) {
            this.privList.unshift(item);
        } else if (index === this.privList.length) {
            this.privList.push(item);
        } else {
            this.privList.splice(index, 0, item);
        }
        this.triggerSubscriptions(this.privAddSubscriptions);
    }

    public removeFirst = (): TItem => {
        this.throwIfDisposed();
        return this.removeAt(0);
    }

    public removeLast = (): TItem => {
        this.throwIfDisposed();
        return this.removeAt(this.length() - 1);
    }

    public removeAt = (index: number): TItem => {
        this.throwIfDisposed();
        return this.remove(index, 1)[0];
    }

    public remove = (index: number, count: number): TItem[] => {
        this.throwIfDisposed();
        const removedElements = this.privList.splice(index, count);
        this.triggerSubscriptions(this.privRemoveSubscriptions);
        return removedElements;
    }

    public clear = (): void => {
        this.throwIfDisposed();
        this.remove(0, this.length());
    }

    public length = (): number => {
        this.throwIfDisposed();
        return this.privList.length;
    }

    public onAdded = (addedCallback: () => void): IDetachable => {
        this.throwIfDisposed();
        const subscriptionId = this.privSubscriptionIdCounter++;

        this.privAddSubscriptions[subscriptionId] = addedCallback;

        return {
            detach: () => {
                delete this.privAddSubscriptions[subscriptionId];
            },
        };
    }

    public onRemoved = (removedCallback: () => void): IDetachable => {
        this.throwIfDisposed();
        const subscriptionId = this.privSubscriptionIdCounter++;

        this.privRemoveSubscriptions[subscriptionId] = removedCallback;

        return {
            detach: () => {
                delete this.privRemoveSubscriptions[subscriptionId];
            },
        };
    }

    public onDisposed = (disposedCallback: () => void): IDetachable => {
        this.throwIfDisposed();
        const subscriptionId = this.privSubscriptionIdCounter++;

        this.privDisposedSubscriptions[subscriptionId] = disposedCallback;

        return {
            detach: () => {
                delete this.privDisposedSubscriptions[subscriptionId];
            },
        };
    }

    public join = (seperator?: string): string => {
        this.throwIfDisposed();
        return this.privList.join(seperator);
    }

    public toArray = (): TItem[] => {
        const cloneCopy = Array<TItem>();
        this.privList.forEach((val: TItem) => {
            cloneCopy.push(val);
        });
        return cloneCopy;
    }

    public any = (callback?: (item: TItem, index: number) => boolean): boolean => {
        this.throwIfDisposed();
        if (callback) {
            return this.where(callback).length() > 0;
        } else {
            return this.length() > 0;
        }
    }

    public all = (callback: (item: TItem) => boolean): boolean => {
        this.throwIfDisposed();
        return this.where(callback).length() === this.length();
    }

    public forEach = (callback: (item: TItem, index: number) => void): void => {
        this.throwIfDisposed();
        for (let i = 0; i < this.length(); i++) {
            callback(this.privList[i], i);
        }
    }

    public select = <T2>(callback: (item: TItem, index: number) => T2): List<T2> => {
        this.throwIfDisposed();
        const selectList: T2[] = [];
        for (let i = 0; i < this.privList.length; i++) {
            selectList.push(callback(this.privList[i], i));
        }

        return new List<T2>(selectList);
    }

    public where = (callback: (item: TItem, index: number) => boolean): List<TItem> => {
        this.throwIfDisposed();
        const filteredList = new List<TItem>();
        for (let i = 0; i < this.privList.length; i++) {
            if (callback(this.privList[i], i)) {
                filteredList.add(this.privList[i]);
            }
        }
        return filteredList;
    }

    public orderBy = (compareFn: (a: TItem, b: TItem) => number): List<TItem> => {
        this.throwIfDisposed();
        const clonedArray = this.toArray();
        const orderedArray = clonedArray.sort(compareFn);
        return new List(orderedArray);
    }

    public orderByDesc = (compareFn: (a: TItem, b: TItem) => number): List<TItem> => {
        this.throwIfDisposed();
        return this.orderBy((a: TItem, b: TItem) => compareFn(b, a));
    }

    public clone = (): List<TItem> => {
        this.throwIfDisposed();
        return new List<TItem>(this.toArray());
    }

    public concat = (list: List<TItem>): List<TItem> => {
        this.throwIfDisposed();
        return new List<TItem>(this.privList.concat(list.toArray()));
    }

    public concatArray = (array: TItem[]): List<TItem> => {
        this.throwIfDisposed();
        return new List<TItem>(this.privList.concat(array));
    }

    public isDisposed = (): boolean => {
        return this.privList == null;
    }

    public dispose = (reason?: string): void => {
        if (!this.isDisposed()) {
            this.privDisposeReason = reason;
            this.privList = null;
            this.privAddSubscriptions = null;
            this.privRemoveSubscriptions = null;
            this.triggerSubscriptions(this.privDisposedSubscriptions);
        }
    }

    private throwIfDisposed = (): void => {
        if (this.isDisposed()) {
            throw new ObjectDisposedError("List", this.privDisposeReason);
        }
    }

    private triggerSubscriptions = (subscriptions: IStringDictionary<() => void>): void => {
        if (subscriptions) {
            for (const subscriptionId in subscriptions) {
                if (subscriptionId) {
                    subscriptions[subscriptionId]();
                }
            }
        }
    }
}
