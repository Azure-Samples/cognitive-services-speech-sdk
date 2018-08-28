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
    Id: string;
    State(): ConnectionState;
    Open(): Promise<ConnectionOpenResponse>;
    Send(message: ConnectionMessage): Promise<boolean>;
    Read(): Promise<ConnectionMessage>;
    Events: EventSource<ConnectionEvent>;
}
