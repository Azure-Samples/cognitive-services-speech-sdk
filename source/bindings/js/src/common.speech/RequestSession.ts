//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import {
    CreateNoDashGuid,
    Deferred,
    Events,
    IAudioStreamNode,
    IDetachable,
    IEventSource,
    PlatformEvent,
    Promise,
} from "../common/Exports";
import {
    ConnectingToServiceEvent,
    ListeningStartedEvent,
    RecognitionStartedEvent,
    RecognitionTriggeredEvent,
    SpeechRecognitionEvent,
} from "./RecognitionEvents";
import { ServiceTelemetryListener } from "./ServiceTelemetryListener.Internal";

export class RequestSession {
    private isDisposed: boolean = false;
    private serviceTelemetryListener: ServiceTelemetryListener;
    private detachables: IDetachable[] = new Array<IDetachable>();
    private requestId: string;
    private audioSourceId: string;
    private audioNodeId: string;
    private audioNode: IAudioStreamNode;
    private authFetchEventId: string;
    private serviceTag: string;
    private isAudioNodeDetached: boolean = false;
    private isCompleted: boolean = false;

    private requestCompletionDeferral: Deferred<boolean>;

    protected sessionId: string;

    constructor(audioSourceId: string) {
        this.audioSourceId = audioSourceId;
        this.requestId = CreateNoDashGuid();
        this.audioNodeId = CreateNoDashGuid();
        this.requestCompletionDeferral = new Deferred<boolean>();

        this.serviceTelemetryListener = new ServiceTelemetryListener(this.requestId, this.audioSourceId, this.audioNodeId);

        this.OnEvent(new RecognitionTriggeredEvent(this.RequestId, this.sessionId, this.audioSourceId, this.audioNodeId));
    }

    public get SessionId(): string {
        return this.sessionId;
    }

    public get RequestId(): string {
        return this.requestId;
    }

    public get AudioNodeId(): string {
        return this.audioNodeId;
    }

    public get CompletionPromise(): Promise<boolean> {
        return this.requestCompletionDeferral.Promise();
    }

    public get IsSpeechEnded(): boolean {
        return this.isAudioNodeDetached;
    }

    public get IsCompleted(): boolean {
        return this.isCompleted;
    }

    public ListenForServiceTelemetry(eventSource: IEventSource<PlatformEvent>): void {
        this.detachables.push(eventSource.AttachListener(this.serviceTelemetryListener));
    }

    public OnAudioSourceAttachCompleted = (audioNode: IAudioStreamNode, isError: boolean, error?: string): void => {
        this.audioNode = audioNode;
        if (isError) {
            this.OnComplete();
        } else {
            this.OnEvent(new ListeningStartedEvent(this.requestId, this.sessionId, this.audioSourceId, this.audioNodeId));
        }
    }

    public OnPreConnectionStart = (authFetchEventId: string, connectionId: string): void => {
        this.authFetchEventId = authFetchEventId;
        this.sessionId = connectionId;
        this.OnEvent(new ConnectingToServiceEvent(this.requestId, this.authFetchEventId, this.sessionId));
    }

    public OnAuthCompleted = (isError: boolean, error?: string): void => {
        if (isError) {
            this.OnComplete();
        }
    }

    public OnConnectionEstablishCompleted = (statusCode: number, reason?: string): void => {
        if (statusCode === 200) {
            this.OnEvent(new RecognitionStartedEvent(this.RequestId, this.audioSourceId, this.audioNodeId, this.authFetchEventId, this.sessionId));
            return;
        } else if (statusCode === 403) {
            this.OnComplete();
        } else {
            this.OnComplete();
        }
    }

    public OnServiceTurnEndResponse = (): void => {
        this.OnComplete();
    }

    public Dispose = (error?: string): void => {
        if (!this.isDisposed) {
            // we should have completed by now. If we did not its an unknown error.
            this.isDisposed = true;
            for (const detachable of this.detachables) {
                detachable.Detach();
            }

            this.serviceTelemetryListener.Dispose();
        }
    }

    public GetTelemetry = (): string => {
        return this.serviceTelemetryListener.GetTelemetry();
    }

    protected OnEvent = (event: SpeechRecognitionEvent): void => {
        this.serviceTelemetryListener.OnEvent(event);
        Events.Instance.OnEvent(event);
    }

    private OnComplete = (): void => {
        if (!this.isCompleted) {
            this.isCompleted = true;
            this.DetachAudioNode();
        }
    }

    private DetachAudioNode = (): void => {
        if (!this.isAudioNodeDetached) {
            this.isAudioNodeDetached = true;
            if (this.audioNode) {
                this.audioNode.Detach();
            }
        }
    }
}
