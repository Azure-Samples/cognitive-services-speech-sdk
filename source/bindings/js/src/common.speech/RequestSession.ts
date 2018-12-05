// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ReplayableAudioNode } from "../common.browser/Exports";
import {
    createNoDashGuid,
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
    private privIsDisposed: boolean = false;
    private privServiceTelemetryListener: ServiceTelemetryListener;
    private privDetachables: IDetachable[] = new Array<IDetachable>();
    private privRequestId: string;
    private privAudioSourceId: string;
    private privAudioNodeId: string;
    private privAudioNode: ReplayableAudioNode;
    private privAuthFetchEventId: string;
    private privIsAudioNodeDetached: boolean = false;
    private privIsCompleted: boolean = false;
    private privRequestCompletionDeferral: Deferred<boolean>;
    private privIsSpeechEnded: boolean = false;
    private privIsCanceled: boolean = false;
    private privContextJson: string;
    private privTurnStartAudioOffset: number = 0;
    private privLastRecoOffset: number = 0;

    protected privSessionId: string;

    constructor(audioSourceId: string, contextJson: string) {
        this.privAudioSourceId = audioSourceId;
        this.privRequestId = createNoDashGuid();
        this.privAudioNodeId = createNoDashGuid();
        this.privRequestCompletionDeferral = new Deferred<boolean>();
        this.privContextJson = contextJson;
        this.privServiceTelemetryListener = new ServiceTelemetryListener(this.privRequestId, this.privAudioSourceId, this.privAudioNodeId);

        this.onEvent(new RecognitionTriggeredEvent(this.requestId, this.privSessionId, this.privAudioSourceId, this.privAudioNodeId));
    }

    public get contextJson(): string {
        return this.privContextJson;
    }

    public get sessionId(): string {
        return this.privSessionId;
    }

    public get requestId(): string {
        return this.privRequestId;
    }

    public get audioNodeId(): string {
        return this.privAudioNodeId;
    }

    public get completionPromise(): Promise<boolean> {
        return this.privRequestCompletionDeferral.promise();
    }

    public get isSpeechEnded(): boolean {
        return this.privIsSpeechEnded;
    }

    public get isCompleted(): boolean {
        return this.privIsCompleted;
    }

    public get isCanceled(): boolean {
        return this.privIsCanceled;
    }

    public get currentTurnAudioOffset(): number {
        return this.privTurnStartAudioOffset;
    }

    public listenForServiceTelemetry(eventSource: IEventSource<PlatformEvent>): void {
        this.privDetachables.push(eventSource.attachListener(this.privServiceTelemetryListener));
    }

    public onAudioSourceAttachCompleted = (audioNode: ReplayableAudioNode, isError: boolean, error?: string): void => {
        this.privAudioNode = audioNode;

        if (isError) {
            this.onComplete();
        } else {
            this.onEvent(new ListeningStartedEvent(this.privRequestId, this.privSessionId, this.privAudioSourceId, this.privAudioNodeId));
        }
    }

    public onPreConnectionStart = (authFetchEventId: string, connectionId: string): void => {
        this.privAuthFetchEventId = authFetchEventId;
        this.privSessionId = connectionId;
        this.onEvent(new ConnectingToServiceEvent(this.privRequestId, this.privAuthFetchEventId, this.privSessionId));
    }

    public onAuthCompleted = (isError: boolean, error?: string): void => {
        if (isError) {
            this.onComplete();
        }
    }

    public onConnectionEstablishCompleted = (statusCode: number, reason?: string): void => {
        if (statusCode === 200) {
            this.onEvent(new RecognitionStartedEvent(this.requestId, this.privAudioSourceId, this.privAudioNodeId, this.privAuthFetchEventId, this.privSessionId));
            this.privAudioNode.replay();
            this.privTurnStartAudioOffset = this.privLastRecoOffset;
            return;
        } else if (statusCode === 403) {
            this.onComplete();
        } else {
            this.onComplete();
        }
    }

    public onServiceTurnEndResponse = (continuousRecognition: boolean): void => {
        if (!continuousRecognition || this.isSpeechEnded) {
            this.onComplete();
        } else {
            // Start a new request set.
            this.privTurnStartAudioOffset = this.privLastRecoOffset;
            this.privRequestId = createNoDashGuid();
            this.privAudioNode.replay();
        }
    }

    public onServiceRecognized(offset: number): void {
        this.privLastRecoOffset = offset;
        this.privAudioNode.shrinkBuffers(offset);
    }

    public dispose = (error?: string): void => {
        if (!this.privIsDisposed) {
            // we should have completed by now. If we did not its an unknown error.
            this.privIsDisposed = true;
            for (const detachable of this.privDetachables) {
                detachable.detach();
            }

            this.privServiceTelemetryListener.dispose();
        }
    }

    public getTelemetry = (): string => {
        return this.privServiceTelemetryListener.getTelemetry();
    }

    public onCancelled(): void {
        this.privIsCanceled = true;
    }

    // Should be called with the audioNode for this session has indicated that it is out of speech.
    public onSpeechEnded(): void {
        this.privIsSpeechEnded = true;
    }

    protected onEvent = (event: SpeechRecognitionEvent): void => {
        this.privServiceTelemetryListener.onEvent(event);
        Events.instance.onEvent(event);
    }

    private onComplete = (): void => {
        if (!this.privIsCompleted) {
            this.privIsCompleted = true;
            this.detachAudioNode();
        }
    }

    private detachAudioNode = (): void => {
        if (!this.privIsAudioNodeDetached) {
            this.privIsAudioNodeDetached = true;
            if (this.privAudioNode) {
                this.privAudioNode.detach();
            }
        }
    }
}
