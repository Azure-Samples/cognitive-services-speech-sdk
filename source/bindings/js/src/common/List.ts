import { ObjectDisposedError } from "./Error";
import { IDetachable } from "./IDetachable";
import { IStringDictionary } from "./IDictionary";
import { IDisposable } from "./IDisposable";

export interface IList<TItem> extends IDisposable {
    Get(itemIndex: number): TItem;
    First(): TItem;
    Last(): TItem;

    Add(item: TItem): void;
    InsertAt(index: number, item: TItem): void;

    RemoveFirst(): TItem;
    RemoveLast(): TItem;
    RemoveAt(index: number): TItem;
    Remove(index: number, count: number): TItem[];
    Clear(): void;

    Length(): number;

    OnAdded(addedCallback: () => void): IDetachable;
    OnRemoved(removedCallback: () => void): IDetachable;
    OnDisposed(disposedCallback: () => void): IDetachable;

    Join(seperator?: string): string;

    ToArray(): TItem[];

    Any(callback?: (item: TItem, index: number) => boolean): boolean;
    All(callback: (item: TItem) => boolean): boolean;
    ForEach(callback: (item: TItem, index: number) => void): void;
    Select<T2>(callback: (item: TItem, index: number) => T2): List<T2>;
    Where(callback: (item: TItem, index: number) => boolean): List<TItem>;
    OrderBy(compareFn: (a: TItem, b: TItem) => number): List<TItem>;
    OrderByDesc(compareFn: (a: TItem, b: TItem) => number): List<TItem>;
    Clone(): List<TItem>;
    Concat(list: List<TItem>): List<TItem>;
    ConcatArray(array: TItem[]): List<TItem>;
}

export class List<TItem> implements IList<TItem>  {
    private list: TItem[];
    private subscriptionIdCounter: number = 0;
    private addSubscriptions: IStringDictionary<() => void> = {};
    private removeSubscriptions: IStringDictionary<() => void> = {};
    private disposedSubscriptions: IStringDictionary<() => void> = {};
    private disposeReason: string = null;

    public constructor(list?: TItem[]) {
        this.list = [];
        // copy the list rather than taking as is.
        if (list) {
            for (const item of list) {
                this.list.push(item);
            }
        }
    }

    public Get = (itemIndex: number): TItem => {
        this.ThrowIfDisposed();
        return this.list[itemIndex];
    }

    public First = (): TItem => {
        return this.Get(0);
    }

    public Last = (): TItem => {
        return this.Get(this.Length() - 1);
    }

    public Add = (item: TItem): void => {
        this.ThrowIfDisposed();
        this.InsertAt(this.list.length, item);
    }

    public InsertAt = (index: number, item: TItem): void => {
        this.ThrowIfDisposed();
        if (index === 0) {
            this.list.unshift(item);
        } else if (index === this.list.length) {
            this.list.push(item);
        } else {
            this.list.splice(index, 0, item);
        }
        this.TriggerSubscriptions(this.addSubscriptions);
    }

    public RemoveFirst = (): TItem => {
        this.ThrowIfDisposed();
        return this.RemoveAt(0);
    }

    public RemoveLast = (): TItem => {
        this.ThrowIfDisposed();
        return this.RemoveAt(this.Length() - 1);
    }

    public RemoveAt = (index: number): TItem => {
        this.ThrowIfDisposed();
        return this.Remove(index, 1)[0];
    }

    public Remove = (index: number, count: number): TItem[] => {
        this.ThrowIfDisposed();
        const removedElements = this.list.splice(index, count);
        this.TriggerSubscriptions(this.removeSubscriptions);
        return removedElements;
    }

    public Clear = (): void => {
        this.ThrowIfDisposed();
        this.Remove(0, this.Length());
    }

    public Length = (): number => {
        this.ThrowIfDisposed();
        return this.list.length;
    }

    public OnAdded = (addedCallback: () => void): IDetachable => {
        this.ThrowIfDisposed();
        const subscriptionId = this.subscriptionIdCounter++;

        this.addSubscriptions[subscriptionId] = addedCallback;

        return {
            Detach: () => {
                delete this.addSubscriptions[subscriptionId];
            },
        };
    }

    public OnRemoved = (removedCallback: () => void): IDetachable => {
        this.ThrowIfDisposed();
        const subscriptionId = this.subscriptionIdCounter++;

        this.removeSubscriptions[subscriptionId] = removedCallback;

        return {
            Detach: () => {
                delete this.removeSubscriptions[subscriptionId];
            },
        };
    }

    public OnDisposed = (disposedCallback: () => void): IDetachable => {
        this.ThrowIfDisposed();
        const subscriptionId = this.subscriptionIdCounter++;

        this.disposedSubscriptions[subscriptionId] = disposedCallback;

        return {
            Detach: () => {
                delete this.disposedSubscriptions[subscriptionId];
            },
        };
    }

    public Join = (seperator?: string): string => {
        this.ThrowIfDisposed();
        return this.list.join(seperator);
    }

    public ToArray = (): TItem[] => {
        const cloneCopy = Array<TItem>();
        this.list.forEach((val: TItem) => {
            cloneCopy.push(val);
        });
        return cloneCopy;
    }

    public Any = (callback?: (item: TItem, index: number) => boolean): boolean => {
        this.ThrowIfDisposed();
        if (callback) {
            return this.Where(callback).Length() > 0;
        } else {
            return this.Length() > 0;
        }
    }

    public All = (callback: (item: TItem) => boolean): boolean => {
        this.ThrowIfDisposed();
        return this.Where(callback).Length() === this.Length();
    }

    public ForEach = (callback: (item: TItem, index: number) => void): void => {
        this.ThrowIfDisposed();
        for (let i = 0; i < this.Length(); i++) {
            callback(this.list[i], i);
        }
    }

    public Select = <T2>(callback: (item: TItem, index: number) => T2): List<T2> => {
        this.ThrowIfDisposed();
        const selectList: T2[] = [];
        for (let i = 0; i < this.list.length; i++) {
            selectList.push(callback(this.list[i], i));
        }

        return new List<T2>(selectList);
    }

    public Where = (callback: (item: TItem, index: number) => boolean): List<TItem> => {
        this.ThrowIfDisposed();
        const filteredList = new List<TItem>();
        for (let i = 0; i < this.list.length; i++) {
            if (callback(this.list[i], i)) {
                filteredList.Add(this.list[i]);
            }
        }
        return filteredList;
    }

    public OrderBy = (compareFn: (a: TItem, b: TItem) => number): List<TItem> => {
        this.ThrowIfDisposed();
        const clonedArray = this.ToArray();
        const orderedArray = clonedArray.sort(compareFn);
        return new List(orderedArray);
    }

    public OrderByDesc = (compareFn: (a: TItem, b: TItem) => number): List<TItem> => {
        this.ThrowIfDisposed();
        return this.OrderBy((a: TItem, b: TItem) => compareFn(b, a));
    }

    public Clone = (): List<TItem> => {
        this.ThrowIfDisposed();
        return new List<TItem>(this.ToArray());
    }

    public Concat = (list: List<TItem>): List<TItem> => {
        this.ThrowIfDisposed();
        return new List<TItem>(this.list.concat(list.ToArray()));
    }

    public ConcatArray = (array: TItem[]): List<TItem> => {
        this.ThrowIfDisposed();
        return new List<TItem>(this.list.concat(array));
    }

    public IsDisposed = (): boolean => {
        return this.list == null;
    }

    public Dispose = (reason?: string): void => {
        if (!this.IsDisposed()) {
            this.disposeReason = reason;
            this.list = null;
            this.addSubscriptions = null;
            this.removeSubscriptions = null;
            this.TriggerSubscriptions(this.disposedSubscriptions);
        }
    }

    private ThrowIfDisposed = (): void => {
        if (this.IsDisposed()) {
            throw new ObjectDisposedError("List", this.disposeReason);
        }
    }

    private TriggerSubscriptions = (subscriptions: IStringDictionary<() => void>): void => {
        if (subscriptions) {
            for (const subscriptionId in subscriptions) {
                if (subscriptionId) {
                    subscriptions[subscriptionId]();
                }
            }
        }
    }
}
