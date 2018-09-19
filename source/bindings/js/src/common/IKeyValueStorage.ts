//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

export interface IKeyValueStorage {
    Get(key: string): string;
    GetOrAdd(key: string, valueToAdd: string): string;
    Set(key: string, value: string): void;
    Remove(key: string): void;
}
