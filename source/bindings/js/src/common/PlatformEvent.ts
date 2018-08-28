import { CreateNoDashGuid } from "./Guid";
import { IStringDictionary } from "./IDictionary";

export enum EventType {
    Debug,
    Info,
    Warning,
    Error,
}

export class PlatformEvent {
    private name: string;
    private eventId: string;
    private eventTime: string;
    private eventType: EventType;
    private metadata: IStringDictionary<string>;

    constructor(eventName: string, eventType: EventType) {
        this.name = eventName;
        this.eventId = CreateNoDashGuid();
        this.eventTime = new Date().toISOString();
        this.eventType = eventType;
        this.metadata = { };
    }

    public get Name(): string {
        return this.name;
    }

    public get EventId(): string {
        return this.eventId;
    }

    public get EventTime(): string {
        return this.eventTime;
    }

    public get EventType(): EventType {
        return this.eventType;
    }

    public get Metadata(): IStringDictionary<string> {
        return this.metadata;
    }
}
