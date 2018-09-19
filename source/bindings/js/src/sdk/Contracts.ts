//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

export class Contracts {
    public static throwIfNullOrUndefined(param: any, name: string): void {
        if (param === undefined || param === null) {
            throw new Error("throwIfNullOrUndefined:" + name);
        }
    }

    public static throwIfNull(param: any, name: string): void {
        if (param === null) {
            throw new Error("throwIfNull:" + name);
        }
    }

    public static throwIfNullOrWhitespace(param: string, name: string): void {
        Contracts.throwIfNullOrUndefined(param, name);

        if (("" + param).trim().length < 1) {
            throw new Error("throwIfNullOrWhitespace:" + name);
        }
    }

    public static throwIfDisposed(isDisposed: boolean): void {
        if (isDisposed) {
            throw new Error("the object is already disposed");
        }
    }

    public static throwIfArrayEmptyOrWhitespace(array: string[], name: string): void {
        Contracts.throwIfNullOrUndefined(array, name);

        if (array.length === 0) {
            throw new Error("throwIfArrayEmptyOrWhitespace:" + name);
        }

        for (const item of array) {
            Contracts.throwIfNullOrWhitespace(item, name);
        }
    }

    public static throwIfFileDoesNotExist(param: any, name: string): void {
        Contracts.throwIfNullOrWhitespace(param, name);

        // TODO check for file existence.
    }
}
