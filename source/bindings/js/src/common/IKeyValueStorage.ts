// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

export interface IKeyValueStorage {
    get(key: string): string;
    getOrAdd(key: string, valueToAdd: string): string;
    set(key: string, value: string): void;
    remove(key: string): void;
}
