// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError, IKeyValueStorage } from "../common/Exports";

export class SessionStorage implements IKeyValueStorage {

    public get = (key: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        return sessionStorage.getItem(key);
    }

    public getOrAdd = (key: string, valueToAdd: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        const value = sessionStorage.getItem(key);
        if (value === null || value === undefined) {
            sessionStorage.setItem(key, valueToAdd);
        }

        return sessionStorage.getItem(key);
    }

    public set = (key: string, value: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        sessionStorage.setItem(key, value);
    }

    public remove = (key: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        sessionStorage.removeItem(key);
    }
}
