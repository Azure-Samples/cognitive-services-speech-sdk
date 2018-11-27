// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ConnectionMessage } from "./ConnectionMessage";
import { IStringDictionary } from "./IDictionary";
import { EventType, PlatformEvent } from "./PlatformEvent";

export class ConnectionEvent extends PlatformEvent {
    private privConnectionId: string;

    constructor(eventName: string, connectionId: string, eventType: EventType = EventType.Info) {
        super(eventName, eventType);
        this.privConnectionId = connectionId;
    }

    public get connectionId(): string {
        return this.privConnectionId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionStartEvent extends ConnectionEvent {
    private privUri: string;
    private privHeaders: IStringDictionary<string>;

    constructor(connectionId: string, uri: string, headers?: IStringDictionary<string>) {
        super("ConnectionStartEvent", connectionId);
        this.privUri = uri;
        this.privHeaders = headers;
    }

    public get uri(): string {
        return this.privUri;
    }

    public get headers(): IStringDictionary<string> {
        return this.privHeaders;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionEstablishedEvent extends ConnectionEvent {
    constructor(connectionId: string, metadata?: IStringDictionary<string>) {
        super("ConnectionEstablishedEvent", connectionId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionClosedEvent extends ConnectionEvent {
    private privRreason: string;
    private privStatusCode: number;

    constructor(connectionId: string, statusCode: number, reason: string) {
        super("ConnectionClosedEvent", connectionId, EventType.Debug);
        this.privRreason = reason;
        this.privStatusCode = statusCode;
    }

    public get reason(): string {
        return this.privRreason;
    }

    public get statusCode(): number {
        return this.privStatusCode;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionEstablishErrorEvent extends ConnectionEvent {
    private privStatusCode: number;
    private privReason: string;

    constructor(connectionId: string, statuscode: number, reason: string) {
        super("ConnectionEstablishErrorEvent", connectionId, EventType.Error);
        this.privStatusCode = statuscode;
        this.privReason = reason;
    }

    public get reason(): string {
        return this.privReason;
    }

    public get statusCode(): number {
        return this.privStatusCode;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionMessageReceivedEvent extends ConnectionEvent {
    private privNetworkReceivedTime: string;
    private privMessage: ConnectionMessage;

    constructor(connectionId: string, networkReceivedTimeISO: string, message: ConnectionMessage) {
        super("ConnectionMessageReceivedEvent", connectionId);
        this.privNetworkReceivedTime = networkReceivedTimeISO;
        this.privMessage = message;
    }

    public get networkReceivedTime(): string {
        return this.privNetworkReceivedTime;
    }

    public get message(): ConnectionMessage {
        return this.privMessage;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionMessageSentEvent extends ConnectionEvent {
    private privNetworkSentTime: string;
    private privMessage: ConnectionMessage;

    constructor(connectionId: string, networkSentTimeISO: string, message: ConnectionMessage) {
        super("ConnectionMessageSentEvent", connectionId);
        this.privNetworkSentTime = networkSentTimeISO;
        this.privMessage = message;
    }

    public get networkSentTime(): string {
        return this.privNetworkSentTime;
    }

    public get message(): ConnectionMessage {
        return this.privMessage;
    }
}
