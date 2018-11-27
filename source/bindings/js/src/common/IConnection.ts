// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ConnectionEvent } from "./ConnectionEvents";
import { ConnectionMessage } from "./ConnectionMessage";
import { ConnectionOpenResponse } from "./ConnectionOpenResponse";
import { EventSource } from "./EventSource";
import { IDisposable } from "./IDisposable";
import { Promise } from "./Promise";

export enum ConnectionState {
    None,
    Connected,
    Connecting,
    Disconnected,
}

export interface IConnection extends IDisposable {
    id: string;
    state(): ConnectionState;
    open(): Promise<ConnectionOpenResponse>;
    send(message: ConnectionMessage): Promise<boolean>;
    read(): Promise<ConnectionMessage>;
    events: EventSource<ConnectionEvent>;
}
