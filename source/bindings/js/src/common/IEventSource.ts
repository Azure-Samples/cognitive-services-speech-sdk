// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { IDetachable } from "./IDetachable";
import { IStringDictionary } from "./IDictionary";
import { IDisposable } from "./IDisposable";
import { PlatformEvent } from "./PlatformEvent";

export interface IEventListener<TEvent extends PlatformEvent> {
    onEvent(e: TEvent): void;
}

export interface IEventSource<TEvent extends PlatformEvent> extends IDisposable {
    metadata: IStringDictionary<string>;

    onEvent(e: TEvent): void;

    attach(onEventCallback: (event: TEvent) => void): IDetachable;

    attachListener(listener: IEventListener<TEvent>): IDetachable;
}
