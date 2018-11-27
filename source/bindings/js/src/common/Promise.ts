// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError  } from "./Error";

export enum PromiseState {
    None,
    Resolved,
    Rejected,
}

export interface IPromise<T> {
    result(): PromiseResult<T>;

    continueWith<TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => TContinuationResult): IPromise<TContinuationResult>;

    continueWithPromise<TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => IPromise<TContinuationResult>): IPromise<TContinuationResult>;

    onSuccessContinueWith<TContinuationResult>(
        continuationCallback: (result: T) => TContinuationResult): IPromise<TContinuationResult>;

    onSuccessContinueWithPromise<TContinuationResult>(
        continuationCallback: (result: T) => IPromise<TContinuationResult>): IPromise<TContinuationResult>;

    on(successCallback: (result: T) => void, errorCallback: (error: string) => void): IPromise<T>;

    finally(callback: () => void): IPromise<T>;
}

export interface IDeferred<T> {
    state(): PromiseState;

    promise(): IPromise<T>;

    resolve(result: T): IDeferred<T>;

    reject(error: string): IDeferred<T>;
}

export class PromiseResult<T> {
    protected privIsCompleted: boolean;
    protected privIsError: boolean;
    protected privError: string;
    protected privResult: T;

    public constructor(promiseResultEventSource: PromiseResultEventSource<T>) {
        promiseResultEventSource.on((result: T) => {
            if (!this.privIsCompleted) {
                this.privIsCompleted = true;
                this.privIsError = false;
                this.privResult = result;
            }
        }, (error: string) => {
            if (!this.privIsCompleted) {
                this.privIsCompleted = true;
                this.privIsError = true;
                this.privError = error;
            }
        });
    }

    public get isCompleted(): boolean {
        return this.privIsCompleted;
    }

    public get isError(): boolean {
        return this.privIsError;
    }

    public get error(): string {
        return this.privError;
    }

    public get result(): T {
        return this.privResult;
    }

    public throwIfError = (): void => {
        if (this.isError) {
            throw this.error;
        }
    }
}

// tslint:disable-next-line:max-classes-per-file
export class PromiseResultEventSource<T>  {

    private privOnSetResult: (result: T) => void;
    private privOnSetError: (error: string) => void;

    public setResult = (result: T): void => {
        this.privOnSetResult(result);
    }

    public setError = (error: string): void => {
        this.privOnSetError(error);
    }

    public on = (onSetResult: (result: T) => void, onSetError: (error: string) => void): void => {
        this.privOnSetResult = onSetResult;
        this.privOnSetError = onSetError;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class PromiseHelper {
    public static whenAll = (promises: Array<Promise<any>>): Promise<boolean> => {
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
                    deferred.resolve(true);
                } else {
                    deferred.reject(errors.join(", "));
                }
            }
        };

        for (const promise of promises) {
            promise.on((r: any) => {
                checkForCompletion();
            }, (e: string) => {
                errors.push(e);
                checkForCompletion();
            });
        }

        return deferred.promise();
    }

    public static fromResult = <TResult>(result: TResult): Promise<TResult> => {
        const deferred = new Deferred<TResult>();
        deferred.resolve(result);
        return deferred.promise();
    }

    public static fromError = <TResult>(error: string): Promise<TResult> => {
        const deferred = new Deferred<TResult>();
        deferred.reject(error);
        return deferred.promise();
    }
}

// TODO: replace with ES6 promises
// tslint:disable-next-line:max-classes-per-file
export class Promise<T> implements IPromise<T> {
    private privSink: Sink<T>;

    public constructor(sink: Sink<T>) {
        this.privSink = sink;
    }

    public result = (): PromiseResult<T> => {
        return this.privSink.result;
    }

    public continueWith = <TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => TContinuationResult): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.privSink.on(
            (r: T) => {
                try {
                    const continuationResult: TContinuationResult = continuationCallback(this.privSink.result);
                    continuationDeferral.resolve(continuationResult);
                } catch (e) {
                    continuationDeferral.reject(e);
                }
            },
            (error: string) => {
                try {
                    const continuationResult: TContinuationResult = continuationCallback(this.privSink.result);
                    continuationDeferral.resolve(continuationResult);
                } catch (e) {
                    continuationDeferral.reject(`'Error handler for error ${error} threw error ${e}'`);
                }
            },
        );

        return continuationDeferral.promise();
    }

    public onSuccessContinueWith = <TContinuationResult>(
        continuationCallback: (result: T) => TContinuationResult): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.privSink.on(
            (r: T) => {
                try {
                    const continuationResult: TContinuationResult = continuationCallback(r);
                    continuationDeferral.resolve(continuationResult);
                } catch (e) {
                    continuationDeferral.reject(e);
                }
            },
            (error: string) => {
                continuationDeferral.reject(error);
            },
        );

        return continuationDeferral.promise();
    }

    public continueWithPromise = <TContinuationResult>(
        continuationCallback: (promiseResult: PromiseResult<T>) => Promise<TContinuationResult>): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.privSink.on(
            (r: T) => {
                try {
                    const continuationPromise: Promise<TContinuationResult> = continuationCallback(this.privSink.result);
                    if (!continuationPromise) {
                        throw new Error("'Continuation callback did not return promise'");
                    }
                    continuationPromise.on((continuationResult: TContinuationResult) => {
                        continuationDeferral.resolve(continuationResult);
                    }, (e: string) => {
                        continuationDeferral.reject(e);
                    });
                } catch (e) {
                    continuationDeferral.reject(e);
                }
            },
            (error: string) => {
                try {
                    const continuationPromise: Promise<TContinuationResult> = continuationCallback(this.privSink.result);
                    if (!continuationPromise) {
                        throw new Error("Continuation callback did not return promise");
                    }
                    continuationPromise.on((continuationResult: TContinuationResult) => {
                        continuationDeferral.resolve(continuationResult);
                    }, (e: string) => {
                        continuationDeferral.reject(e);
                    });
                } catch (e) {
                    continuationDeferral.reject(`'Error handler for error ${error} threw error ${e}'`);
                }
            },
        );

        return continuationDeferral.promise();
    }

    public onSuccessContinueWithPromise = <TContinuationResult>(
        continuationCallback: (result: T) => Promise<TContinuationResult>): Promise<TContinuationResult> => {

        if (!continuationCallback) {
            throw new ArgumentNullError("continuationCallback");
        }

        const continuationDeferral = new Deferred<TContinuationResult>();

        this.privSink.on(
            (r: T) => {
                try {
                    const continuationPromise: Promise<TContinuationResult> = continuationCallback(r);
                    if (!continuationPromise) {
                        throw new Error("Continuation callback did not return promise");
                    }
                    continuationPromise.on((continuationResult: TContinuationResult) => {
                        continuationDeferral.resolve(continuationResult);
                    }, (e: string) => {
                        continuationDeferral.reject(e);
                    });
                } catch (e) {
                    continuationDeferral.reject(e);
                }
            },
            (error: string) => {
                continuationDeferral.reject(error);
            },
        );

        return continuationDeferral.promise();
    }

    public on = (
        successCallback: (result: T) => void,
        errorCallback: (error: string) => void): Promise<T> => {
        if (!successCallback) {
            throw new ArgumentNullError("successCallback");
        }

        if (!errorCallback) {
            throw new ArgumentNullError("errorCallback");
        }

        this.privSink.on(successCallback, errorCallback);
        return this;
    }

    public finally = (callback: () => void): Promise<T> => {
        if (!callback) {
            throw new ArgumentNullError("callback");
        }

        const callbackWrapper = (_: any) => {
            callback();
        };

        return this.on(callbackWrapper, callbackWrapper);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class Deferred<T> implements IDeferred<T> {
    private privPromise: Promise<T>;
    private privSink: Sink<T>;

    public constructor() {
        this.privSink = new Sink<T>();
        this.privPromise = new Promise<T>(this.privSink);
    }

    public state = (): PromiseState => {
        return this.privSink.state;
    }

    public promise = (): Promise<T> => {
        return this.privPromise;
    }

    public resolve = (result: T): Deferred<T> => {
        this.privSink.resolve(result);
        return this;
    }

    public reject = (error: string): Deferred<T> => {
        this.privSink.reject(error);
        return this;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class Sink<T> {
    private privState: PromiseState = PromiseState.None;
    private privPromiseResult: PromiseResult<T> = null;
    private privPromiseResultEvents: PromiseResultEventSource<T> = null;

    private privSuccessHandlers: Array<((result: T) => void)> = [];
    private privErrorHandlers: Array<(e: string) => void> = [];

    public constructor() {
        this.privPromiseResultEvents = new PromiseResultEventSource();
        this.privPromiseResult = new PromiseResult(this.privPromiseResultEvents);
    }

    public get state(): PromiseState {
        return this.privState;
    }

    public get result(): PromiseResult<T> {
        return this.privPromiseResult;
    }

    public resolve = (result: T): void => {
        if (this.privState !== PromiseState.None) {
            throw new Error("'Cannot resolve a completed promise'");
        }

        this.privState = PromiseState.Resolved;
        this.privPromiseResultEvents.setResult(result);

        for (let i = 0; i < this.privSuccessHandlers.length; i++) {
            this.executeSuccessCallback(result, this.privSuccessHandlers[i], this.privErrorHandlers[i]);
        }

        this.detachHandlers();
    }

    public reject = (error: string): void => {
        if (this.privState !== PromiseState.None) {
            throw new Error("'Cannot reject a completed promise'");
        }

        this.privState = PromiseState.Rejected;
        this.privPromiseResultEvents.setError(error);

        for (const errorHandler of this.privErrorHandlers) {
            this.executeErrorCallback(error, errorHandler);
        }

        this.detachHandlers();
    }

    public on = (
        successCallback: (result: T) => void,
        errorCallback: (error: string) => void): void => {

        if (successCallback == null) {
            successCallback = (r: T) => { return; };
        }

        if (this.privState === PromiseState.None) {
            this.privSuccessHandlers.push(successCallback);
            this.privErrorHandlers.push(errorCallback);
        } else {
            if (this.privState === PromiseState.Resolved) {
                this.executeSuccessCallback(this.privPromiseResult.result, successCallback, errorCallback);
            } else if (this.privState === PromiseState.Rejected) {
                this.executeErrorCallback(this.privPromiseResult.error, errorCallback);
            }

            this.detachHandlers();
        }
    }

    private executeSuccessCallback = (result: T, successCallback: (result: T) => void, errorCallback: (error: string) => void): void => {
        try {
            successCallback(result);
        } catch (e) {
            this.executeErrorCallback(`'Unhandled callback error: ${e}'`, errorCallback);
        }
    }

    private executeErrorCallback = (error: string, errorCallback: (error: string) => void): void => {
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

    private detachHandlers = (): void => {
        this.privErrorHandlers = [];
        this.privSuccessHandlers = [];
    }
}
