// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import {
    AudioSourceErrorEvent,
    AudioStreamNodeAttachedEvent,
    AudioStreamNodeAttachingEvent,
    AudioStreamNodeDetachedEvent,
    AudioStreamNodeErrorEvent,
    ConnectionClosedEvent,
    ConnectionEstablishedEvent,
    ConnectionEstablishErrorEvent,
    ConnectionMessageReceivedEvent,
    ConnectionStartEvent,
    IEventListener,
    IStringDictionary,
    PlatformEvent,
} from "../common/Exports";
import {
    ConnectingToServiceEvent,
    RecognitionTriggeredEvent,
} from "./RecognitionEvents";

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
    private isDisposed: boolean = false;

    private requestId: string;
    private audioSourceId: string;
    private audioNodeId: string;

    private listeningTriggerMetric: IMetric = null;
    private micMetric: IMetric = null;
    private connectionEstablishMetric: IMetric = null;

    private micStartTime: string;

    private connectionId: string;
    private connectionStartTime: string;

    private receivedMessages: IStringDictionary<string[]>;

    constructor(requestId: string, audioSourceId: string, audioNodeId: string) {
        this.requestId = requestId;
        this.audioSourceId = audioSourceId;
        this.audioNodeId = audioNodeId;

        this.receivedMessages = {};
    }

    public OnEvent = (e: PlatformEvent): void => {
        if (this.isDisposed) {
            return;
        }

        if (e instanceof RecognitionTriggeredEvent && e.RequestId === this.requestId) {
            this.listeningTriggerMetric = {
                End: e.EventTime,
                Name: "ListeningTrigger",
                Start: e.EventTime,
            };
        }

        if (e instanceof AudioStreamNodeAttachingEvent && e.AudioSourceId === this.audioSourceId && e.AudioNodeId === this.audioNodeId) {
            this.micStartTime = e.EventTime;
        }

        if (e instanceof AudioStreamNodeAttachedEvent && e.AudioSourceId === this.audioSourceId && e.AudioNodeId === this.audioNodeId) {
            this.micStartTime = e.EventTime;
        }

        if (e instanceof AudioSourceErrorEvent && e.AudioSourceId === this.audioSourceId) {
            if (!this.micMetric) {
                this.micMetric = {
                    End: e.EventTime,
                    Error: e.Error,
                    Name: "Microphone",
                    Start: this.micStartTime,
                };
            }
        }

        if (e instanceof AudioStreamNodeErrorEvent && e.AudioSourceId === this.audioSourceId && e.AudioNodeId === this.audioNodeId) {
            if (!this.micMetric) {
                this.micMetric = {
                    End: e.EventTime,
                    Error: e.Error,
                    Name: "Microphone",
                    Start: this.micStartTime,
                };
            }
        }

        if (e instanceof AudioStreamNodeDetachedEvent && e.AudioSourceId === this.audioSourceId && e.AudioNodeId === this.audioNodeId) {
            if (!this.micMetric) {
                this.micMetric = {
                    End: e.EventTime,
                    Name: "Microphone",
                    Start: this.micStartTime,
                };
            }
        }

        if (e instanceof ConnectingToServiceEvent && e.RequestId === this.requestId) {
            this.connectionId = e.SessionId;
        }

        if (e instanceof ConnectionStartEvent && e.ConnectionId === this.connectionId) {
            this.connectionStartTime = e.EventTime;
        }

        if (e instanceof ConnectionEstablishedEvent && e.ConnectionId === this.connectionId) {
            if (!this.connectionEstablishMetric) {
                this.connectionEstablishMetric = {
                    End: e.EventTime,
                    Id: this.connectionId,
                    Name: "Connection",
                    Start: this.connectionStartTime,
                };
            }
        }

        if (e instanceof ConnectionEstablishErrorEvent && e.ConnectionId === this.connectionId) {
            if (!this.connectionEstablishMetric) {
                this.connectionEstablishMetric = {
                    End: e.EventTime,
                    Error: this.GetConnectionError(e.StatusCode),
                    Id: this.connectionId,
                    Name: "Connection",
                    Start: this.connectionStartTime,
                };
            }
        }

        if (e instanceof ConnectionMessageReceivedEvent && e.ConnectionId === this.connectionId) {
            if (e.Message && e.Message.Headers && e.Message.Headers.path) {
                if (!this.receivedMessages[e.Message.Headers.path]) {
                    this.receivedMessages[e.Message.Headers.path] = new Array<string>();
                }

                this.receivedMessages[e.Message.Headers.path].push(e.NetworkReceivedTime);
            }
        }
    }

    public GetTelemetry = (): string => {
        const metrics = new Array<IMetric>();

        if (this.listeningTriggerMetric) {
            metrics.push(this.listeningTriggerMetric);
        }

        if (this.micMetric) {
            metrics.push(this.micMetric);
        }

        if (this.connectionEstablishMetric) {
            metrics.push(this.connectionEstablishMetric);
        }

        const telemetry: ITelemetry = {
            Metrics: metrics,
            ReceivedMessages: this.receivedMessages,
        };

        const json = JSON.stringify(telemetry);

        // We dont want to send the same telemetry again. So clean those out.
        this.receivedMessages = {};
        this.listeningTriggerMetric = null;
        this.micMetric = null;
        this.connectionEstablishMetric = null;

        return json;
    }

    public Dispose = (): void => {
        this.isDisposed = true;
    }

    private GetConnectionError = (statusCode: number): string => {
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
