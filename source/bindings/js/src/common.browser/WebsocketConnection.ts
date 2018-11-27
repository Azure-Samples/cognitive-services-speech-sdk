// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    ArgumentNullError,
    ConnectionEvent,
    ConnectionMessage,
    ConnectionOpenResponse,
    ConnectionState,
    createNoDashGuid,
    EventSource,
    IConnection,
    IStringDictionary,
    IWebsocketMessageFormatter,
    Promise,
} from "../common/Exports";
import { WebsocketMessageAdapter } from "./WebsocketMessageAdapter";

export class WebsocketConnection implements IConnection {

    private privUri: string;
    private privMessageFormatter: IWebsocketMessageFormatter;
    private privConnectionMessageAdapter: WebsocketMessageAdapter;
    private privId: string;
    private privIsDisposed: boolean = false;

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

        this.privMessageFormatter = messageFormatter;

        let queryParams = "";
        let i = 0;

        if (queryParameters) {
            for (const paramName in queryParameters) {
                if (paramName) {
                    queryParams += ((i === 0) && (uri.indexOf("?") === -1)) ? "?" : "&";
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

        this.privUri = uri + queryParams;
        this.privId = connectionId ? connectionId : createNoDashGuid();

        this.privConnectionMessageAdapter = new WebsocketMessageAdapter(
            this.privUri,
            this.id,
            this.privMessageFormatter);
    }

    public dispose = (): void => {
        this.privIsDisposed = true;

        if (this.privConnectionMessageAdapter) {
            this.privConnectionMessageAdapter.close();
        }
    }

    public isDisposed = (): boolean => {
        return this.privIsDisposed;
    }

    public get id(): string {
        return this.privId;
    }

    public state = (): ConnectionState => {
        return this.privConnectionMessageAdapter.state;
    }

    public open = (): Promise<ConnectionOpenResponse> => {
        return this.privConnectionMessageAdapter.open();
    }

    public send = (message: ConnectionMessage): Promise<boolean> => {
        return this.privConnectionMessageAdapter.send(message);
    }

    public read = (): Promise<ConnectionMessage> => {
        return this.privConnectionMessageAdapter.read();
    }

    public get events(): EventSource<ConnectionEvent> {
        return this.privConnectionMessageAdapter.events;
    }
}
