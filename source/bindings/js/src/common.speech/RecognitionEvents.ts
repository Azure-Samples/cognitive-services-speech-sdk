// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { EventType, PlatformEvent } from "../common/Exports";

export class SpeechRecognitionEvent extends PlatformEvent {
    private privRequestId: string;
    private privSessionId: string;

    constructor(eventName: string, requestId: string, sessionId: string, eventType: EventType = EventType.Info) {
        super(eventName, eventType);

        this.privRequestId = requestId;
        this.privSessionId = sessionId;
    }

    public get requestId(): string {
        return this.privRequestId;
    }

    public get sessionId(): string {
        return this.privSessionId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionTriggeredEvent extends SpeechRecognitionEvent {
    private privAudioSourceId: string;
    private privAudioNodeId: string;

    constructor(requestId: string, sessionId: string, audioSourceId: string, audioNodeId: string) {
        super("RecognitionTriggeredEvent", requestId, sessionId);

        this.privAudioSourceId = audioSourceId;
        this.privAudioNodeId = audioNodeId;
    }

    public get audioSourceId(): string {
        return this.privAudioSourceId;
    }

    public get audioNodeId(): string {
        return this.privAudioNodeId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ListeningStartedEvent extends SpeechRecognitionEvent {
    private privAudioSourceId: string;
    private privAudioNodeId: string;

    constructor(requestId: string, sessionId: string, audioSourceId: string, audioNodeId: string) {
        super("ListeningStartedEvent", requestId, sessionId);
        this.privAudioSourceId = audioSourceId;
        this.privAudioNodeId = audioNodeId;
    }

    public get audioSourceId(): string {
        return this.privAudioSourceId;
    }

    public get audioNodeId(): string {
        return this.privAudioNodeId;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class ConnectingToServiceEvent extends SpeechRecognitionEvent {
    private privAuthFetchEventid: string;

    constructor(requestId: string, authFetchEventid: string, sessionId: string) {
        super("ConnectingToServiceEvent", requestId, sessionId);
        this.privAuthFetchEventid = authFetchEventid;
    }

    public get authFetchEventid(): string {
        return this.privAuthFetchEventid;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionStartedEvent extends SpeechRecognitionEvent {
    private privAudioSourceId: string;
    private privAudioNodeId: string;
    private privAuthFetchEventId: string;

    constructor(requestId: string, audioSourceId: string, audioNodeId: string, authFetchEventId: string, sessionId: string) {
        super("RecognitionStartedEvent", requestId, sessionId);

        this.privAudioSourceId = audioSourceId;
        this.privAudioNodeId = audioNodeId;
        this.privAuthFetchEventId = authFetchEventId;
    }

    public get audioSourceId(): string {
        return this.privAudioSourceId;
    }

    public get audioNodeId(): string {
        return this.privAudioNodeId;
    }

    public get authFetchEventId(): string {
        return this.privAuthFetchEventId;
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
    private privAudioSourceId: string;
    private privAudioNodeId: string;
    private privAuthFetchEventId: string;
    private privServiceTag: string;
    private privStatus: RecognitionCompletionStatus;
    private privError: string;

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

        this.privAudioSourceId = audioSourceId;
        this.privAudioNodeId = audioNodeId;
        this.privAuthFetchEventId = authFetchEventId;
        this.privStatus = status;
        this.privError = error;
        this.privServiceTag = serviceTag;
    }

    public get audioSourceId(): string {
        return this.privAudioSourceId;
    }

    public get audioNodeId(): string {
        return this.privAudioNodeId;
    }

    public get authFetchEventId(): string {
        return this.privAuthFetchEventId;
    }

    public get serviceTag(): string {
        return this.privServiceTag;
    }

    public get status(): RecognitionCompletionStatus {
        return this.privStatus;
    }

    public get error(): string {
        return this.privError;
    }
}
