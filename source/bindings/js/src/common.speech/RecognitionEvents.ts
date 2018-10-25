//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import {
    EventType,
    PlatformEvent,
} from "../common/Exports";

export class SpeechRecognitionEvent extends PlatformEvent {
    private requestId: string;
    private sessionId: string;

    constructor(eventName: string, requestId: string, sessionId: string, eventType: EventType = EventType.Info) {
        super(eventName, eventType);

        this.requestId = requestId;
        this.sessionId = sessionId;
    }

    public get RequestId(): string {
        return this.requestId;
    }

    public get SessionId(): string {
        return this.sessionId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionTriggeredEvent extends SpeechRecognitionEvent {
    private audioSourceId: string;
    private audioNodeId: string;

    constructor(requestId: string, sessionId: string, audioSourceId: string, audioNodeId: string) {
        super("RecognitionTriggeredEvent", requestId, sessionId);

        this.audioSourceId = audioSourceId;
        this.audioNodeId = audioNodeId;
    }

    public get AudioSourceId(): string {
        return this.audioSourceId;
    }

    public get AudioNodeId(): string {
        return this.audioNodeId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ListeningStartedEvent extends SpeechRecognitionEvent {
    private audioSourceId: string;
    private audioNodeId: string;

    constructor(requestId: string, sessionId: string, audioSourceId: string, audioNodeId: string) {
        super("ListeningStartedEvent", requestId, sessionId);
        this.audioSourceId = audioSourceId;
        this.audioNodeId = audioNodeId;
    }

    public get AudioSourceId(): string {
        return this.audioSourceId;
    }

    public get AudioNodeId(): string {
        return this.audioNodeId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectingToServiceEvent extends SpeechRecognitionEvent {
    private authFetchEventid: string;

    constructor(requestId: string, authFetchEventid: string, sessionId: string) {
        super("ConnectingToServiceEvent", requestId, sessionId);
        this.authFetchEventid = authFetchEventid;
    }

    public get AuthFetchEventid(): string {
        return this.authFetchEventid;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionStartedEvent extends SpeechRecognitionEvent {
    private audioSourceId: string;
    private audioNodeId: string;
    private authFetchEventId: string;

    constructor(requestId: string, audioSourceId: string, audioNodeId: string, authFetchEventId: string, sessionId: string) {
        super("RecognitionStartedEvent", requestId, sessionId);

        this.audioSourceId = audioSourceId;
        this.audioNodeId = audioNodeId;
        this.authFetchEventId = authFetchEventId;
    }

    public get AudioSourceId(): string {
        return this.audioSourceId;
    }

    public get AudioNodeId(): string {
        return this.audioNodeId;
    }

    public get AuthFetchEventId(): string {
        return this.authFetchEventId;
    }
}

export enum RecognitionCompletionStatus {
    Success,
    AudioSourceError,
    AudioSourceTimeout,
    AuthTokenFetchError,
    AuthTokenFetchTimeout,
    UnAuthorized,
    ConnectTimeout,
    ConnectError,
    ClientRecognitionActivityTimeout,
    UnknownError,
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionEndedEvent extends SpeechRecognitionEvent {
    private audioSourceId: string;
    private audioNodeId: string;
    private authFetchEventId: string;
    private serviceTag: string;
    private status: RecognitionCompletionStatus;
    private error: string;

    constructor(
        requestId: string,
        audioSourceId: string,
        audioNodeId: string,
        authFetchEventId: string,
        sessionId: string,
        serviceTag: string,
        status: RecognitionCompletionStatus,
        error: string) {

        super("RecognitionEndedEvent", requestId, sessionId, status === RecognitionCompletionStatus.Success ? EventType.Info : EventType.Error);

        this.audioSourceId = audioSourceId;
        this.audioNodeId = audioNodeId;
        this.authFetchEventId = authFetchEventId;
        this.status = status;
        this.error = error;
        this.serviceTag = serviceTag;
    }

    public get AudioSourceId(): string {
        return this.audioSourceId;
    }

    public get AudioNodeId(): string {
        return this.audioNodeId;
    }

    public get AuthFetchEventId(): string {
        return this.authFetchEventId;
    }

    public get ServiceTag(): string {
        return this.serviceTag;
    }

    public get Status(): RecognitionCompletionStatus {
        return this.status;
    }

    public get Error(): string {
        return this.error;
    }
}
