import { EventType, PlatformEvent } from "./PlatformEvent";

export class AudioSourceEvent extends PlatformEvent {
    private audioSourceId: string;

    constructor(eventName: string, audioSourceId: string, eventType: EventType = EventType.Info) {
        super(eventName, eventType);
        this.audioSourceId = audioSourceId;
    }

    public get AudioSourceId(): string {
        return this.audioSourceId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioSourceInitializingEvent extends AudioSourceEvent {
    constructor(audioSourceId: string) {
        super("AudioSourceInitializingEvent", audioSourceId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioSourceReadyEvent extends AudioSourceEvent {
    constructor(audioSourceId: string) {
        super("AudioSourceReadyEvent", audioSourceId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioSourceOffEvent extends AudioSourceEvent {
    constructor(audioSourceId: string) {
        super("AudioSourceOffEvent", audioSourceId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioSourceErrorEvent extends AudioSourceEvent {
    private error: string;
    constructor(audioSourceId: string, error: string) {
        super("AudioSourceErrorEvent", audioSourceId, EventType.Error);
        this.error = error;
    }

    public get Error(): string {
        return this.error;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamNodeEvent extends AudioSourceEvent {
    private audioNodeId: string;

    constructor(eventName: string, audioSourceId: string, audioNodeId: string) {
        super(eventName, audioSourceId);
        this.audioNodeId = audioNodeId;
    }

    public get AudioNodeId(): string {
        return this.audioNodeId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamNodeAttachingEvent extends AudioStreamNodeEvent {
    constructor(audioSourceId: string, audioNodeId: string) {
        super("AudioStreamNodeAttachingEvent", audioSourceId, audioNodeId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamNodeAttachedEvent extends AudioStreamNodeEvent {
    constructor(audioSourceId: string, audioNodeId: string) {
        super("AudioStreamNodeAttachedEvent", audioSourceId, audioNodeId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamNodeDetachedEvent extends AudioStreamNodeEvent {
    constructor(audioSourceId: string, audioNodeId: string) {
        super("AudioStreamNodeDetachedEvent", audioSourceId, audioNodeId);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamNodeErrorEvent extends AudioStreamNodeEvent {
    private error: string;

    constructor(audioSourceId: string, audioNodeId: string, error: string) {
        super("AudioStreamNodeErrorEvent", audioSourceId, audioNodeId);
        this.error = error;
    }

    public get Error(): string {
        return this.error;
    }
}
