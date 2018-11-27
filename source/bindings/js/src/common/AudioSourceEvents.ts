// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { EventType, PlatformEvent } from "./PlatformEvent";

export class AudioSourceEvent extends PlatformEvent {
    private privAudioSourceId: string;

    constructor(eventName: string, audioSourceId: string, eventType: EventType = EventType.Info) {
        super(eventName, eventType);
        this.privAudioSourceId = audioSourceId;
    }

    public get audioSourceId(): string {
        return this.privAudioSourceId;
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
    private privError: string;

    constructor(audioSourceId: string, error: string) {
        super("AudioSourceErrorEvent", audioSourceId, EventType.Error);
        this.privError = error;
    }

    public get error(): string {
        return this.privError;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class AudioStreamNodeEvent extends AudioSourceEvent {
    private privAudioNodeId: string;

    constructor(eventName: string, audioSourceId: string, audioNodeId: string) {
        super(eventName, audioSourceId);
        this.privAudioNodeId = audioNodeId;
    }

    public get audioNodeId(): string {
        return this.privAudioNodeId;
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
    private privError: string;

    constructor(audioSourceId: string, audioNodeId: string, error: string) {
        super("AudioStreamNodeErrorEvent", audioSourceId, audioNodeId);
        this.privError = error;
    }

    public get error(): string {
        return this.privError;
    }
}
