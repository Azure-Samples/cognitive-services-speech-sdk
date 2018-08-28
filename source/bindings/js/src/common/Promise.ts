import { ArgumentNullError  } from "./Error";

export enum PromiseState {
    None,
    Resolved,
    Rejected,
}

export interface IPromise<T> {
    Result(): PromiseResult<T>;

    ContinueWith<TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => TContinuationResult): IPromise<TContinuationResult>;

    ContinueWithPromise<TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => IPromise<TContinuationResult>): IPromise<TContinuationResult>;

    OnSuccessContinueWith<TContinuationResult>(
        continuationCallback: (result: T) => TContinuationResult): IPromise<TContinuationResult>;

    OnSuccessContinueWithPromise<TContinuationResult>(
        continuationCallback: (result: T) => IPromise<TContinuationResult>): IPromise<TContinuationResult>;

    On(successCallback: (result: T) => void, errorCallback: (error: string) => void): IPromise<T>;

    Finally(callback: () => void): IPromise<T>;
}

export interface IDeferred<T> {
    State(): PromiseState;

    Promise(): IPromise<T>;

    Resolve(result: T): IDeferred<T>;

    Reject(error: string): IDeferred<T>;
}

export class PromiseResult<T> {
    protected isCompleted: boolean;
    protected isError: boolean;
    protected error: string;
    protected result: T;

    public constructor(promiseResultEventSource: PromiseResultEventSource<T>) {
        promiseResultEventSource.On((result: T) => {
            if (!this.isCompleted) {
                this.isCompleted = true;
                this.isError = false;
                this.result = result;
            }
        }, (error: string) => {
            if (!this.isCompleted) {
                this.isCompleted = true;
                this.isError = true;
                this.error = error;
            }
        });
    }

    public get IsCompleted(): boolean {
        return this.isCompleted;
    }

    public get IsError(): boolean {
        return this.isError;
    }

    public get Error(): string {
        return this.error;
    }

    public get Result(): T {
        return this.result;
    }

    public ThrowIfError = (): void => {
        if (this.IsError) {
            throw this.Error;
        }
    }
}

// tslint:disable-next-line:max-classes-per-file
export class PromiseResultEventSource<T>  {

    private onSetResult: (result: T) => void;
    private onSetError: (error: string) => void;

    public SetResult = (result: T): void => {
        this.onSetResult(result);
    }

    public SetError = (error: string): void => {
        this.onSetError(error);
    }

    public On = (onSetResult: (result: T) => void, onSetError: (error: string) => void): void => {
        this.onSetResult = onSetResult;
        this.onSetError = onSetError;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class PromiseHelper {
    public static WhenAll = (promises: Array<Promise<any>>): Promise<boolean> => {
        if (!promises || promises.length === 0) {
            throw new ArgumentNullError("promises");
        }

        const deferred = new Deferred<boolean>();
        const errors: string[] = [];
        let completedPromises: number = 0;

        const checkForCompletion = () => {
            completedPromises++;
            if (completedPromises === promises.length) {
                if (errors.length === 0) {
                    deferred.Resolve(true);
                } else {
                    deferred.Reject(errors.join(", "));
                }
            }
        };

        for (const promise of promises) {
            promise.On((r: any) => {
                checkForCompletion();
            }, (e: string) => {
                errors.push(e);
                checkForCompletion();
            });
        }

        return deferred.Promise();
    }

    public static FromResult = <TResult>(result: TResult): Promise<TResult> => {
        const deferred = new Deferred<TResult>();
        deferred.Resolve(result);
        return deferred.Promise();
    }

    public static FromError = <TResult>(error: string): Promise<TResult> => {
        const deferred = new Deferred<TResult>();
        deferred.Reject(error);
        return deferred.Promise();
    }
}

// TODO: replace with ES6 promises
// tslint:disable-next-line:max-classes-per-file
export class Promise<T> implements IPromise<T> {

    private sink: Sink<T>;

    public constructor(sink: Sink<T>) {
        this.sink = sink;
    }

    public Result = (): PromiseResult<T> => {
        return this.sink.Result;
    }

    public ContinueWith = <TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => TContinuationResult): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.sink.on(
            (r: T) => {
                try {
                    const continuationResult: TContinuationResult = continuationCallback(this.sink.Result);
                    continuationDeferral.Resolve(continuationResult);
                } catch (e) {
                    continuationDeferral.Reject(`'Unhandled callback error: ${e}'`);
                }
            },
            (error: string) => {
                try {
                    const continuationResult: TContinuationResult = continuationCallback(this.sink.Result);
                    continuationDeferral.Resolve(continuationResult);
                } catch (e) {
                    continuationDeferral.Reject(`'Unhandled callback error: ${e}. InnerError: ${error}'`);
                }
            },
        );

        return continuationDeferral.Promise();
    }

    public OnSuccessContinueWith = <TContinuationResult>(
        continuationCallback: (result: T) => TContinuationResult): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.sink.on(
            (r: T) => {
                try {
                    const continuationResult: TContinuationResult = continuationCallback(r);
                    continuationDeferral.Resolve(continuationResult);
                } catch (e) {
                    continuationDeferral.Reject(`'Unhandled callback error: ${e}'`);
                }
            },
            (error: string) => {
                continuationDeferral.Reject(`'Unhandled callback error: ${error}'`);
            },
        );

        return continuationDeferral.Promise();
    }

    public ContinueWithPromise = <TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => Promise<TContinuationResult>): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.sink.on(
            (r: T) => {
                try {
                    const continuationPromise: Promise<TContinuationResult> = continuationCallback(this.sink.Result);
                    if (!continuationPromise) {
                        throw new Error("'Continuation callback did not return promise'");
                    }
                    continuationPromise.On((continuationResult: TContinuationResult) => {
                        continuationDeferral.Resolve(continuationResult);
                    }, (e: string) => {
                        continuationDeferral.Reject(e);
                    });
                } catch (e) {
                    continuationDeferral.Reject(`'Unhandled callback error: ${e}'`);
                }
            },
            (error: string) => {
                try {
                    const continuationPromise: Promise<TContinuationResult> = continuationCallback(this.sink.Result);
                    if (!continuationPromise) {
                        throw new Error("Continuation callback did not return promise");
                    }
                    continuationPromise.On((continuationResult: TContinuationResult) => {
                        continuationDeferral.Resolve(continuationResult);
                    }, (e: string) => {
                        continuationDeferral.Reject(e);
                    });
                } catch (e) {
                    continuationDeferral.Reject(`'Unhandled callback error: ${e}. InnerError: ${error}'`);
                }
            },
        );

        return continuationDeferral.Promise();
    }

    public OnSuccessContinueWithPromise = <TContinuationResult>(
        continuationCallback: (result: T) => Promise<TContinuationResult>): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.sink.on(
            (r: T) => {
                try {
                    const continuationPromise: Promise<TContinuationResult> = continuationCallback(r);
                    if (!continuationPromise) {
                        throw new Error("Continuation callback did not return promise");
                    }
                    continuationPromise.On((continuationResult: TContinuationResult) => {
                        continuationDeferral.Resolve(continuationResult);
                    }, (e: string) => {
                        continuationDeferral.Reject(e);
                    });
                } catch (e) {
                    continuationDeferral.Reject(`'Unhandled callback error: ${e}'`);
                }
            },
            (error: string) => {
                continuationDeferral.Reject(`'Unhandled callback error: ${error}.'`);
            },
        );

        return continuationDeferral.Promise();
    }

    public On = (
        successCallback: (result: T) => void,
        errorCallback: (error: string) => void): Promise<T> => {
        if (!successCallback) {
            throw new ArgumentNullError("successCallback");
        }

        if (!errorCallback) {
            throw new ArgumentNullError("errorCallback");
        }

        this.sink.on(successCallback, errorCallback);
        return this;
    }

    public Finally = (callback: () => void): Promise<T> => {
        if (!callback) {
            throw new ArgumentNullError("callback");
        }

        const callbackWrapper = (_: any) => {
            callback();
        };

        return this.On(callbackWrapper, callbackWrapper);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class Deferred<T> implements IDeferred<T> {

    private promise: Promise<T>;
    private sink: Sink<T>;

    public constructor() {
        this.sink = new Sink<T>();
        this.promise = new Promise<T>(this.sink);
    }

    public State = (): PromiseState => {
        return this.sink.State;
    }

    public Promise = (): Promise<T> => {
        return this.promise;
    }

    public Resolve = (result: T): Deferred<T> => {
        this.sink.Resolve(result);
        return this;
    }

    public Reject = (error: string): Deferred<T> => {
        this.sink.Reject(error);
        return this;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class Sink<T> {

    private state: PromiseState = PromiseState.None;
    private promiseResult: PromiseResult<T> = null;
    private promiseResultEvents: PromiseResultEventSource<T> = null;

    private successHandlers: Array<((result: T) => void)> = [];
    private errorHandlers: Array<(e: string) => void> = [];

    public constructor() {
        this.promiseResultEvents = new PromiseResultEventSource();
        this.promiseResult = new PromiseResult(this.promiseResultEvents);
    }

    public get State(): PromiseState {
        return this.state;
    }

    public get Result(): PromiseResult<T> {
        return this.promiseResult;
    }

    public Resolve = (result: T): void => {
        if (this.state !== PromiseState.None) {
            throw new Error("'Cannot resolve a completed promise'");
        }

        this.state = PromiseState.Resolved;
        this.promiseResultEvents.SetResult(result);

        for (let i = 0; i < this.successHandlers.length; i++) {
            this.ExecuteSuccessCallback(result, this.successHandlers[i], this.errorHandlers[i]);
        }

        this.DetachHandlers();
    }

    public Reject = (error: string): void => {
        if (this.state !== PromiseState.None) {
            throw new Error("'Cannot reject a completed promise'");
        }

        this.state = PromiseState.Rejected;
        this.promiseResultEvents.SetError(error);

        for (const errorHandler of this.errorHandlers) {
            this.ExecuteErrorCallback(error, errorHandler);
        }

        this.DetachHandlers();
    }

    public on = (
        successCallback: (result: T) => void,
        errorCallback: (error: string) => void): void => {

        if (successCallback == null) {
            successCallback = (r: T) => { return; };
        }

        if (this.state === PromiseState.None) {
            this.successHandlers.push(successCallback);
            this.errorHandlers.push(errorCallback);
        } else {
            if (this.state === PromiseState.Resolved) {
                this.ExecuteSuccessCallback(this.promiseResult.Result, successCallback, errorCallback);
            } else if (this.state === PromiseState.Rejected) {
                this.ExecuteErrorCallback(this.promiseResult.Error, errorCallback);
            }

            this.DetachHandlers();
        }
    }

    private ExecuteSuccessCallback = (result: T, successCallback: (result: T) => void, errorCallback: (error: string) => void): void => {
        try {
            successCallback(result);
        } catch (e) {
            this.ExecuteErrorCallback(`'Unhandled callback error: ${e}'`, errorCallback);
        }
    }

    private ExecuteErrorCallback = (error: string, errorCallback: (error: string) => void): void => {
        if (errorCallback) {
            try {
                errorCallback(error);
            } catch (e) {
                throw new Error(`'Unhandled callback error: ${e}. InnerError: ${error}'`);
            }
        } else {
            throw new Error(`'Unhandled error: ${error}'`);
        }
    }

    private DetachHandlers = (): void => {
        this.errorHandlers = [];
        this.successHandlers = [];
    }
}
