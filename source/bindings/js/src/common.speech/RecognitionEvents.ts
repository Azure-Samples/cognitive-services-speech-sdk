//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import {
    EventType,
    PlatformEvent,
} from "../common/Exports";
import {
    IDetailedSpeechPhrase,
    IIntentResponse,
    ISimpleSpeechPhrase,
    ISpeechEndDetectedResult,
    ISpeechHypothesis,
    ISpeechStartDetected,
    ITranslationHypothesis,
    ITranslationPhrase,
    ITranslationSynthesisEnd,
    } from "./Exports";

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
export class SpeechRecognitionResultEvent<TResult> extends SpeechRecognitionEvent {
    private result: TResult;

    constructor(eventName: string, requestId: string, sessionId: string, result: TResult) {
        super(eventName, requestId, sessionId);
        this.result = result;
    }

    public get Result(): TResult {
        return this.result;
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

// tslint:disable-next-line:max-classes-per-file
export class SpeechStartDetectedEvent extends SpeechRecognitionResultEvent<ISpeechStartDetected> {
    constructor(requestId: string, sessionId: string, result: ISpeechStartDetected) {
        super("SpeechStartDetectedEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechHypothesisEvent extends SpeechRecognitionResultEvent<ISpeechHypothesis> {
    constructor(requestId: string, sessionId: string, result: ISpeechHypothesis) {
        super("SpeechHypothesisEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechFragmentEvent extends SpeechRecognitionResultEvent<ISpeechHypothesis> {
    constructor(requestId: string, sessionId: string, result: ISpeechHypothesis) {
        super("SpeechFragmentEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechEndDetectedEvent extends SpeechRecognitionResultEvent<ISpeechEndDetectedResult> {
    constructor(requestId: string, sessionId: string, result: ISpeechEndDetectedResult) {
        super("SpeechEndDetectedEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechSimplePhraseEvent extends SpeechRecognitionResultEvent<ISimpleSpeechPhrase> {
    constructor(requestId: string, sessionId: string, result: ISimpleSpeechPhrase) {
        super("SpeechSimplePhraseEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechDetailedPhraseEvent extends SpeechRecognitionResultEvent<IDetailedSpeechPhrase> {
    constructor(requestId: string, sessionId: string, result: IDetailedSpeechPhrase) {
        super("SpeechDetailedPhraseEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationHypothesisEvent extends SpeechRecognitionResultEvent<ITranslationHypothesis> {
    constructor(requestId: string, sessionId: string, result: ITranslationHypothesis) {
        super("TranslationHypothesisEvent", requestId, sessionId, result);
    }
}
// tslint:disable-next-line:max-classes-per-file
export class TranslationPhraseEvent extends SpeechRecognitionResultEvent<ITranslationPhrase> {
    constructor(requestId: string, sessionId: string, result: ITranslationPhrase) {
        super("TranslationPhraseEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationFailedEvent extends SpeechRecognitionResultEvent<ITranslationPhrase> {
    constructor(requestId: string, sessionId: string, result: ITranslationPhrase) {
        super("TranslationFailedEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionFailedEvent extends SpeechRecognitionResultEvent<ISimpleSpeechPhrase> {
    constructor(requestId: string, sessionId: string, result: ISimpleSpeechPhrase) {
        super("RecognitionFailedEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationSynthesisEvent extends SpeechRecognitionResultEvent<ArrayBuffer> {
    constructor(requestId: string, sessionId: string, result: ArrayBuffer) {
        super("TranslationSynthesisEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationSynthesisErrorEvent extends SpeechRecognitionResultEvent<ITranslationSynthesisEnd> {
    constructor(requestId: string, sessionId: string, result: ITranslationSynthesisEnd) {
        super("TranslationSynthesisErrorEvent", requestId, sessionId, result);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class IntentResponseEvent extends SpeechRecognitionResultEvent<IIntentResponse> {
    constructor(requestId: string, sessionId: string, result: IIntentResponse) {
        super("IntentResponseEvent", requestId, sessionId, result);
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
