// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError } from "./Error";
import { IKeyValueStorage } from "./IKeyValueStorage";
import { InMemoryStorage } from "./InMemoryStorage";

export class Storage {
    private static privSessionStorage: IKeyValueStorage = new InMemoryStorage();
    private static privLocalStorage: IKeyValueStorage = new InMemoryStorage();

    public static setSessionStorage = (sessionStorage: IKeyValueStorage): void => {
        if (!sessionStorage) {
            throw new ArgumentNullError("sessionStorage");
        }

        Storage.privSessionStorage = sessionStorage;
    }

    public static setLocalStorage = (localStorage: IKeyValueStorage): void => {
        if (!localStorage) {
            throw new ArgumentNullError("localStorage");
        }

        Storage.privLocalStorage = localStorage;
    }

    public static get session(): IKeyValueStorage {
        return Storage.privSessionStorage;
    }

    public static get local(): IKeyValueStorage {
        return Storage.privLocalStorage;
    }
}
