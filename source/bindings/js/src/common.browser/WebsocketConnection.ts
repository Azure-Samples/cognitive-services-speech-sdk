import {
    ArgumentNullError,
    ConnectionEvent,
    ConnectionMessage,
    ConnectionOpenResponse,
    ConnectionState,
    CreateNoDashGuid,
    EventSource,
    IConnection,
    IStringDictionary,
    IWebsocketMessageFormatter,
    PlatformEvent,
    Promise,
} from "../common/Exports";
import { WebsocketMessageAdapter } from "./WebsocketMessageAdapter";

export class WebsocketConnection implements IConnection {

    private uri: string;
    private messageFormatter: IWebsocketMessageFormatter;
    private connectionMessageAdapter: WebsocketMessageAdapter;
    private id: string;
    private isDisposed: boolean = false;

    public constructor(
        uri: string,
        queryParameters: IStringDictionary<string>,
        headers: IStringDictionary<string>,
        messageFormatter: IWebsocketMessageFormatter,
        connectionId?: string) {

        if (!uri) {
            throw new ArgumentNullError("uri");
        }

        if (!messageFormatter) {
            throw new ArgumentNullError("messageFormatter");
        }

        this.messageFormatter = messageFormatter;

        let queryParams = "";
        let i = 0;

        if (queryParameters) {
            for (const paramName in queryParameters) {
                if (paramName) {
                    queryParams += i === 0 ? "?" : "&";
                    const val = encodeURIComponent(queryParameters[paramName]);
                    queryParams += `${paramName}=${val}`;
                    i++;
                }
            }
        }

        if (headers) {
            for (const headerName in headers) {
                if (headerName) {
                    queryParams += i === 0 ? "?" : "&";
                    const val = encodeURIComponent(headers[headerName]);
                    queryParams += `${headerName}=${val}`;
                    i++;
                }
            }
        }

        this.uri = uri + queryParams;
        this.id = connectionId ? connectionId : CreateNoDashGuid();

        this.connectionMessageAdapter = new WebsocketMessageAdapter(
            this.uri,
            this.Id,
            this.messageFormatter);
    }

    public Dispose = (): void => {
        this.isDisposed = true;

        if (this.connectionMessageAdapter) {
            this.connectionMessageAdapter.Close();
        }
    }

    public IsDisposed = (): boolean => {
        return this.isDisposed;
    }

    public get Id(): string {
        return this.id;
    }

    public State = (): ConnectionState => {
        return this.connectionMessageAdapter.State;
    }

    public Open = (): Promise<ConnectionOpenResponse> => {
        return this.connectionMessageAdapter.Open();
    }

    public Send = (message: ConnectionMessage): Promise<boolean> => {
        return this.connectionMessageAdapter.Send(message);
    }

    public Read = (): Promise<ConnectionMessage> => {
        return this.connectionMessageAdapter.Read();
    }

    public get Events(): EventSource<ConnectionEvent> {
        return this.connectionMessageAdapter.Events;
    }
}
