// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError, IKeyValueStorage } from "../common/Exports";

export class LocalStorage implements IKeyValueStorage {

    public get = (key: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        return localStorage.getItem(key);
    }

    public getOrAdd = (key: string, valueToAdd: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        const value = localStorage.getItem(key);
        if (value === null || value === undefined) {
            localStorage.setItem(key, valueToAdd);
        }

        return localStorage.getItem(key);
    }

    public set = (key: string, value: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        localStorage.setItem(key, value);
    }

    public remove = (key: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        localStorage.removeItem(key);
    }
}
