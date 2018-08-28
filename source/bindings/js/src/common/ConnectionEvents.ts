import { ConnectionMessage } from "./ConnectionMessage";
import { IStringDictionary } from "./IDictionary";
import { EventType, PlatformEvent } from "./PlatformEvent";

export class ConnectionEvent extends PlatformEvent {
    private connectionId: string;

    constructor(eventName: string, connectionId: string, eventType: EventType = EventType.Info) {
        super(eventName, eventType);
        this.connectionId = connectionId;
    }

    public get ConnectionId(): string {
        return this.connectionId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionStartEvent extends ConnectionEvent {
    private uri: string;
    private headers: IStringDictionary<string>;

    constructor(connectionId: string, uri: string, headers?: IStringDictionary<string>) {
        super("ConnectionStartEvent", connectionId);
        this.uri = uri;
        this.headers = headers;
    }

    public get Uri(): string {
        return this.uri;
    }

    public get Headers(): IStringDictionary<string> {
        return this.headers;
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
    private reason: string;
    private statusCode: number;

    constructor(connectionId: string, statusCode: number, reason: string) {
        super("ConnectionClosedEvent", connectionId, EventType.Warning);
        this.reason = reason;
        this.statusCode = statusCode;
    }

    public get Reason(): string {
        return this.reason;
    }

    public get StatusCode(): number {
        return this.statusCode;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionEstablishErrorEvent extends ConnectionEvent {
    private statusCode: number;
    private reason: string;

    constructor(connectionId: string, statuscode: number, reason: string) {
        super("ConnectionEstablishErrorEvent", connectionId, EventType.Error);
        this.statusCode = statuscode;
        this.reason = reason;
    }

    public get Reason(): string {
        return this.reason;
    }

    public get StatusCode(): number {
        return this.statusCode;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionMessageReceivedEvent extends ConnectionEvent {
    private networkReceivedTime: string;
    private message: ConnectionMessage;

    constructor(connectionId: string, networkReceivedTimeISO: string, message: ConnectionMessage) {
        super("ConnectionMessageReceivedEvent", connectionId);
        this.networkReceivedTime = networkReceivedTimeISO;
        this.message = message;
    }

    public get NetworkReceivedTime(): string {
        return this.networkReceivedTime;
    }

    public get Message(): ConnectionMessage {
        return this.message;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectionMessageSentEvent extends ConnectionEvent {
    private networkSentTime: string;
    private message: ConnectionMessage;

    constructor(connectionId: string, networkSentTimeISO: string, message: ConnectionMessage) {
        super("ConnectionMessageSentEvent", connectionId);
        this.networkSentTime = networkSentTimeISO;
        this.message = message;
    }

    public get NetworkSentTime(): string {
        return this.networkSentTime;
    }

    public get Message(): ConnectionMessage {
        return this.message;
    }
}
