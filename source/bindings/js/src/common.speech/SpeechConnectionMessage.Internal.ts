// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError, ConnectionMessage, IStringDictionary, MessageType } from "../common/Exports";

const PathHeaderName: string = "path";
const ContentTypeHeaderName: string = "content-type";
const RequestIdHeaderName: string = "x-requestid";
const RequestTimestampHeaderName: string = "x-timestamp";

export class SpeechConnectionMessage extends ConnectionMessage {

    private privPath: string;
    private privRequestId: string;
    private privContentType: string;
    private privAdditionalHeaders: IStringDictionary<string>;

    public constructor(
        messageType: MessageType,
        path: string,
        requestId: string,
        contentType: string,
        body: any,
        additionalHeaders?: IStringDictionary<string>,
        id?: string) {

        if (!path) {
            throw new ArgumentNullError("path");
        }

        if (!requestId) {
            throw new ArgumentNullError("requestId");
        }

        const headers: IStringDictionary<string> = {};
        headers[PathHeaderName] = path;
        headers[RequestIdHeaderName] = requestId;
        headers[RequestTimestampHeaderName] = new Date().toISOString();
        if (contentType) {
            headers[ContentTypeHeaderName] = contentType;
        }

        if (additionalHeaders) {
            for (const headerName in additionalHeaders) {
                if (headerName) {
                    headers[headerName] = additionalHeaders[headerName];
                }

            }
        }

        if (id) {
            super(messageType, body, headers, id);
        } else {
            super(messageType, body, headers);
        }

        this.privPath = path;
        this.privRequestId = requestId;
        this.privContentType = contentType;
        this.privAdditionalHeaders = additionalHeaders;
    }

    public get path(): string {
        return this.privPath;
    }

    public get requestId(): string {
        return this.privRequestId;
    }

    public get contentType(): string {
        return this.privContentType;
    }

    public get additionalHeaders(): IStringDictionary<string> {
        return this.privAdditionalHeaders;
    }

    public static fromConnectionMessage = (message: ConnectionMessage): SpeechConnectionMessage => {
        let path = null;
        let requestId = null;
        let contentType = null;
        let requestTimestamp = null;
        const additionalHeaders: IStringDictionary<string> = {};

        if (message.headers) {
            for (const headerName in message.headers) {
                if (headerName) {
                    if (headerName.toLowerCase() === PathHeaderName.toLowerCase()) {
                        path = message.headers[headerName];
                    } else if (headerName.toLowerCase() === RequestIdHeaderName.toLowerCase()) {
                        requestId = message.headers[headerName];
                    } else if (headerName.toLowerCase() === RequestTimestampHeaderName.toLowerCase()) {
                        requestTimestamp = message.headers[headerName];
                    } else if (headerName.toLowerCase() === ContentTypeHeaderName.toLowerCase()) {
                        contentType = message.headers[headerName];
                    } else {
                        additionalHeaders[headerName] = message.headers[headerName];
                    }
                }
            }
        }

        return new SpeechConnectionMessage(
            message.messageType,
            path,
            requestId,
            contentType,
            message.body,
            additionalHeaders,
            message.id);
    }
}
