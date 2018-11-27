// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError } from "./Error";
import { IStringDictionary } from "./IDictionary";
import { IKeyValueStorage } from "./IKeyValueStorage";

export class InMemoryStorage implements IKeyValueStorage {

    private privStore: IStringDictionary<string> = {};

    public get = (key: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        return this.privStore[key];
    }

    public getOrAdd = (key: string, valueToAdd: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        if (this.privStore[key] === undefined) {
            this.privStore[key] = valueToAdd;
        }

        return this.privStore[key];
    }

    public set = (key: string, value: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        this.privStore[key] = value;
    }

    public remove = (key: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        if (this.privStore[key] !== undefined) {
            delete this.privStore[key];
        }
    }
}
