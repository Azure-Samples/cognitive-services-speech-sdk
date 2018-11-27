// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

export interface IStringDictionary<TValue> {
    [propName: string]: TValue;
}

export interface INumberDictionary<TValue> extends Object {
    [propName: number]: TValue;
}
