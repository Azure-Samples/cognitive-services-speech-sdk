import {
    ArgumentNullError,
    ConnectionClosedEvent,
    ConnectionEstablishedEvent,
    ConnectionEstablishErrorEvent,
    ConnectionEvent,
    ConnectionMessage,
    ConnectionMessageReceivedEvent,
    ConnectionMessageSentEvent,
    ConnectionOpenResponse,
    ConnectionStartEvent,
    ConnectionState,
    Deferred,
    Events,
    EventSource,
    IWebsocketMessageFormatter,
    MessageType,
    PlatformEvent,
    Promise,
    PromiseHelper,
    Queue,
    RawWebsocketMessage,
} from "../common/Exports";

interface ISendItem {
    Message: ConnectionMessage;
    RawWebsocketMessage: RawWebsocketMessage;
    SendStatusDeferral: Deferred<boolean>;
}

export class WebsocketMessageAdapter {

    private connectionState: ConnectionState;
    private messageFormatter: IWebsocketMessageFormatter;
    private websocketClient: WebSocket;

    private sendMessageQueue: Queue<ISendItem>;
    private receivingMessageQueue: Queue<ConnectionMessage>;
    private connectionEstablishDeferral: Deferred<ConnectionOpenResponse>;
    private disconnectDeferral: Deferred<boolean>;
    private connectionEvents: EventSource<ConnectionEvent>;
    private connectionId: string;
    private uri: string;

    public constructor(
        uri: string,
        connectionId: string,
        messageFormatter: IWebsocketMessageFormatter) {

        if (!uri) {
            throw new ArgumentNullError("uri");
        }

        if (!messageFormatter) {
            throw new ArgumentNullError("messageFormatter");
        }

        this.connectionEvents = new EventSource<ConnectionEvent>();
        this.connectionId = connectionId;
        this.messageFormatter = messageFormatter;
        this.connectionState = ConnectionState.None;
        this.uri = uri;
    }

    public get State(): ConnectionState {
        return this.connectionState;
    }

    public Open = (): Promise<ConnectionOpenResponse> => {
        if (this.connectionState === ConnectionState.Disconnected) {
            return PromiseHelper.FromError<ConnectionOpenResponse>(`Cannot open a connection that is in ${this.connectionState} state`);
        }

        if (this.connectionEstablishDeferral) {
            return this.connectionEstablishDeferral.Promise();
        }

        this.connectionEstablishDeferral = new Deferred<ConnectionOpenResponse>();
        this.connectionState = ConnectionState.Connecting;

        this.websocketClient = new WebSocket(this.uri);
        this.websocketClient.binaryType = "arraybuffer";
        this.receivingMessageQueue = new Queue<ConnectionMessage>();
        this.disconnectDeferral = new Deferred<boolean>();
        this.sendMessageQueue = new Queue<ISendItem>();
        this.ProcessSendQueue();

        this.OnEvent(new ConnectionStartEvent(this.connectionId, this.uri));

        this.websocketClient.onopen = (e: Event) => {
            this.connectionState = ConnectionState.Connected;
            this.OnEvent(new ConnectionEstablishedEvent(this.connectionId));
            this.connectionEstablishDeferral.Resolve(new ConnectionOpenResponse(200, ""));
        };

        this.websocketClient.onerror = (e: Event) => {
            // TODO: Understand what this is error is. Will we still get onClose ?
            if (this.connectionState !== ConnectionState.Connecting) {
                // TODO: Is this required ?
                // this.OnEvent(new ConnectionErrorEvent(errorMsg, connectionId));
            }
        };

        this.websocketClient.onclose = (e: CloseEvent) => {
            if (this.connectionState === ConnectionState.Connecting) {
                this.connectionState = ConnectionState.Disconnected;
                this.OnEvent(new ConnectionEstablishErrorEvent(this.connectionId, e.code, e.reason));
                this.connectionEstablishDeferral.Resolve(new ConnectionOpenResponse(e.code, e.reason));
            } else {
                this.OnEvent(new ConnectionClosedEvent(this.connectionId, e.code, e.reason));
            }

            this.OnClose(e.code, e.reason);
        };

        this.websocketClient.onmessage = (e: MessageEvent) => {
            const networkReceivedTime = new Date().toISOString();
            if (this.connectionState === ConnectionState.Connected) {
                const deferred = new Deferred<ConnectionMessage>();
                // let id = ++this.idCounter;
                this.receivingMessageQueue.EnqueueFromPromise(deferred.Promise());
                if (e.data instanceof ArrayBuffer) {
                    const rawMessage = new RawWebsocketMessage(MessageType.Binary, e.data);
                    this.messageFormatter
                        .ToConnectionMessage(rawMessage)
                        .On((connectionMessage: ConnectionMessage) => {
                            this.OnEvent(new ConnectionMessageReceivedEvent(this.connectionId, networkReceivedTime, connectionMessage));
                            deferred.Resolve(connectionMessage);
                        }, (error: string) => {
                            // TODO: Events for these ?
                            deferred.Reject(`Invalid binary message format. Error: ${error}`);
                        });
                } else {
                    const rawMessage = new RawWebsocketMessage(MessageType.Text, e.data);
                    this.messageFormatter
                        .ToConnectionMessage(rawMessage)
                        .On((connectionMessage: ConnectionMessage) => {
                            this.OnEvent(new ConnectionMessageReceivedEvent(this.connectionId, networkReceivedTime, connectionMessage));
                            deferred.Resolve(connectionMessage);
                        }, (error: string) => {
                            // TODO: Events for these ?
                            deferred.Reject(`Invalid text message format. Error: ${error}`);
                        });
                }
            }
        };

        return this.connectionEstablishDeferral.Promise();
    }

    public Send = (message: ConnectionMessage): Promise<boolean> => {
        if (this.connectionState !== ConnectionState.Connected) {
            return PromiseHelper.FromError<boolean>(`Cannot send on connection that is in ${this.connectionState} state`);
        }

        const messageSendStatusDeferral = new Deferred<boolean>();
        const messageSendDeferral = new Deferred<ISendItem>();

        this.sendMessageQueue.EnqueueFromPromise(messageSendDeferral.Promise());

        this.messageFormatter
            .FromConnectionMessage(message)
            .On((rawMessage: RawWebsocketMessage) => {
                messageSendDeferral.Resolve({
                    Message: message,
                    RawWebsocketMessage: rawMessage,
                    SendStatusDeferral: messageSendStatusDeferral,
                });
            }, (error: string) => {
                messageSendDeferral.Reject(`Error formatting the message. ${error}`);
            });

        return messageSendStatusDeferral.Promise();
    }

    public Read = (): Promise<ConnectionMessage> => {
        if (this.connectionState !== ConnectionState.Connected) {
            return PromiseHelper.FromError<ConnectionMessage>(`Cannot read on connection that is in ${this.connectionState} state`);
        }

        return this.receivingMessageQueue.Dequeue();
    }

    public Close = (reason?: string): Promise<boolean> => {
        if (this.websocketClient) {
            if (this.connectionState !== ConnectionState.Connected) {
                this.websocketClient.close(1000, reason ? reason : "Normal closure by client");
            }
        } else {
            const deferral = new Deferred<boolean>();
            deferral.Resolve(true);
            return deferral.Promise();
        }

        return this.disconnectDeferral.Promise();
    }

    public get Events(): EventSource<ConnectionEvent> {
        return this.connectionEvents;
    }

    private SendRawMessage = (sendItem: ISendItem): Promise<boolean> => {
        try {
            this.OnEvent(new ConnectionMessageSentEvent(this.connectionId, new Date().toISOString(), sendItem.Message));
            this.websocketClient.send(sendItem.RawWebsocketMessage.Payload);
            return PromiseHelper.FromResult(true);
        } catch (e) {
            return PromiseHelper.FromError<boolean>(`websocket send error: ${e}`);
        }
    }

    private OnClose = (code: number, reason: string): void => {
        const closeReason = `Connection closed. ${code}: ${reason}`;
        this.connectionState = ConnectionState.Disconnected;
        this.disconnectDeferral.Resolve(true);
        this.receivingMessageQueue.Dispose(reason);
        this.receivingMessageQueue.DrainAndDispose((pendingReceiveItem: ConnectionMessage) => {
            // TODO: Events for these ?
            // Logger.Instance.OnEvent(new LoggingEvent(LogType.Warning, null, `Failed to process received message. Reason: ${closeReason}, Message: ${JSON.stringify(pendingReceiveItem)}`));
        }, closeReason);

        this.sendMessageQueue.DrainAndDispose((pendingSendItem: ISendItem) => {
            pendingSendItem.SendStatusDeferral.Reject(closeReason);
        }, closeReason);
    }

    private ProcessSendQueue = (): void => {
        this.sendMessageQueue
            .Dequeue()
            .On((sendItem: ISendItem) => {
                this.SendRawMessage(sendItem)
                    .On((result: boolean) => {
                        sendItem.SendStatusDeferral.Resolve(result);
                        this.ProcessSendQueue();
                    }, (sendError: string) => {
                        sendItem.SendStatusDeferral.Reject(sendError);
                        this.ProcessSendQueue();
                    });
            }, (error: string) => {
                // do nothing
            });
    }

    private OnEvent = (event: ConnectionEvent): void => {
        this.connectionEvents.OnEvent(event);
        Events.Instance.OnEvent(event);
    }
}
