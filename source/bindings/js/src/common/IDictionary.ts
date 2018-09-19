//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

export interface IStringDictionary<TValue> {
    [propName: string]: TValue;
}

export interface INumberDictionary<TValue> extends Object {
    [propName: number]: TValue;
}
