// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    AudioSourceErrorEvent,
    AudioStreamNodeAttachedEvent,
    AudioStreamNodeAttachingEvent,
    AudioStreamNodeDetachedEvent,
    AudioStreamNodeErrorEvent,
    ConnectionEstablishedEvent,
    ConnectionEstablishErrorEvent,
    ConnectionMessageReceivedEvent,
    ConnectionStartEvent,
    IEventListener,
    IStringDictionary,
    PlatformEvent,
} from "../common/Exports";
import { ConnectingToServiceEvent, RecognitionTriggeredEvent } from "./RecognitionEvents";

interface ITelemetry {
    Metrics: IMetric[];
    ReceivedMessages: IStringDictionary<string[]>;
}

// tslint:disable-next-line:max-classes-per-file
interface IMetric {
    End: string;
    Error?: string;
    Id?: string;
    Name: string;
    Start: string;
}

// tslint:disable-next-line:max-classes-per-file
export class ServiceTelemetryListener implements IEventListener<PlatformEvent> {
    private privIsDisposed: boolean = false;

    private privRequestId: string;
    private privAudioSourceId: string;
    private privAudioNodeId: string;

    private privListeningTriggerMetric: IMetric = null;
    private privMicMetric: IMetric = null;
    private privConnectionEstablishMetric: IMetric = null;

    private privMicStartTime: string;

    private privConnectionId: string;
    private privConnectionStartTime: string;

    private privReceivedMessages: IStringDictionary<string[]>;

    constructor(requestId: string, audioSourceId: string, audioNodeId: string) {
        this.privRequestId = requestId;
        this.privAudioSourceId = audioSourceId;
        this.privAudioNodeId = audioNodeId;

        this.privReceivedMessages = {};
    }

    public onEvent = (e: PlatformEvent): void => {
        if (this.privIsDisposed) {
            return;
        }

        if (e instanceof RecognitionTriggeredEvent && e.requestId === this.privRequestId) {
            this.privListeningTriggerMetric = {
                End: e.eventTime,
                Name: "ListeningTrigger",
                Start: e.eventTime,
            };
        }

        if (e instanceof AudioStreamNodeAttachingEvent && e.audioSourceId === this.privAudioSourceId && e.audioNodeId === this.privAudioNodeId) {
            this.privMicStartTime = e.eventTime;
        }

        if (e instanceof AudioStreamNodeAttachedEvent && e.audioSourceId === this.privAudioSourceId && e.audioNodeId === this.privAudioNodeId) {
            this.privMicStartTime = e.eventTime;
        }

        if (e instanceof AudioSourceErrorEvent && e.audioSourceId === this.privAudioSourceId) {
            if (!this.privMicMetric) {
                this.privMicMetric = {
                    End: e.eventTime,
                    Error: e.error,
                    Name: "Microphone",
                    Start: this.privMicStartTime,
                };
            }
        }

        if (e instanceof AudioStreamNodeErrorEvent && e.audioSourceId === this.privAudioSourceId && e.audioNodeId === this.privAudioNodeId) {
            if (!this.privMicMetric) {
                this.privMicMetric = {
                    End: e.eventTime,
                    Error: e.error,
                    Name: "Microphone",
                    Start: this.privMicStartTime,
                };
            }
        }

        if (e instanceof AudioStreamNodeDetachedEvent && e.audioSourceId === this.privAudioSourceId && e.audioNodeId === this.privAudioNodeId) {
            if (!this.privMicMetric) {
                this.privMicMetric = {
                    End: e.eventTime,
                    Name: "Microphone",
                    Start: this.privMicStartTime,
                };
            }
        }

        if (e instanceof ConnectingToServiceEvent && e.requestId === this.privRequestId) {
            this.privConnectionId = e.sessionId;
        }

        if (e instanceof ConnectionStartEvent && e.connectionId === this.privConnectionId) {
            this.privConnectionStartTime = e.eventTime;
        }

        if (e instanceof ConnectionEstablishedEvent && e.connectionId === this.privConnectionId) {
            if (!this.privConnectionEstablishMetric) {
                this.privConnectionEstablishMetric = {
                    End: e.eventTime,
                    Id: this.privConnectionId,
                    Name: "Connection",
                    Start: this.privConnectionStartTime,
                };
            }
        }

        if (e instanceof ConnectionEstablishErrorEvent && e.connectionId === this.privConnectionId) {
            if (!this.privConnectionEstablishMetric) {
                this.privConnectionEstablishMetric = {
                    End: e.eventTime,
                    Error: this.getConnectionError(e.statusCode),
                    Id: this.privConnectionId,
                    Name: "Connection",
                    Start: this.privConnectionStartTime,
                };
            }
        }

        if (e instanceof ConnectionMessageReceivedEvent && e.connectionId === this.privConnectionId) {
            if (e.message && e.message.headers && e.message.headers.path) {
                if (!this.privReceivedMessages[e.message.headers.path]) {
                    this.privReceivedMessages[e.message.headers.path] = new Array<string>();
                }

                this.privReceivedMessages[e.message.headers.path].push(e.networkReceivedTime);
            }
        }
    }

    public getTelemetry = (): string => {
        const metrics = new Array<IMetric>();

        if (this.privListeningTriggerMetric) {
            metrics.push(this.privListeningTriggerMetric);
        }

        if (this.privMicMetric) {
            metrics.push(this.privMicMetric);
        }

        if (this.privConnectionEstablishMetric) {
            metrics.push(this.privConnectionEstablishMetric);
        }

        const telemetry: ITelemetry = {
            Metrics: metrics,
            ReceivedMessages: this.privReceivedMessages,
        };

        const json = JSON.stringify(telemetry);

        // We dont want to send the same telemetry again. So clean those out.
        this.privReceivedMessages = {};
        this.privListeningTriggerMetric = null;
        this.privMicMetric = null;
        this.privConnectionEstablishMetric = null;

        return json;
    }

    public dispose = (): void => {
        this.privIsDisposed = true;
    }

    private getConnectionError = (statusCode: number): string => {
        /*
        -- Websocket status codes --
        NormalClosure = 1000,
        EndpointUnavailable = 1001,
        ProtocolError = 1002,
        InvalidMessageType = 1003,
        Empty = 1005,
        InvalidPayloadData = 1007,
        PolicyViolation = 1008,
        MessageTooBig = 1009,
        MandatoryExtension = 1010,
        InternalServerError = 1011
        */

        switch (statusCode) {
            case 400:
            case 1002:
            case 1003:
            case 1005:
            case 1007:
            case 1008:
            case 1009: return "BadRequest";
            case 401: return "Unauthorized";
            case 403: return "Forbidden";
            case 503:
            case 1001: return "ServerUnavailable";
            case 500:
            case 1011: return "ServerError";
            case 408:
            case 504: return "Timeout";
            default: return "statuscode:" + statusCode.toString();
        }
    }
}
