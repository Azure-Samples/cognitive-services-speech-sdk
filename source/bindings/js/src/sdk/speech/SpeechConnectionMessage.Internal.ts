import {
    ArgumentNullError,
    ConnectionMessage,
    IStringDictionary,
    MessageType,
} from "../../common/Exports";

const PathHeaderName: string = "path";
const ContentTypeHeaderName: string = "content-type";
const RequestIdHeaderName: string = "x-requestid";
const RequestTimestampHeaderName: string = "x-timestamp";

export class SpeechConnectionMessage extends ConnectionMessage {

    private path: string;
    private requestId: string;
    private contentType: string;
    private additionalHeaders: IStringDictionary<string>;

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

        this.path = path;
        this.requestId = requestId;
        this.contentType = contentType;
        this.additionalHeaders = additionalHeaders;
    }

    public get Path(): string {
        return this.path;
    }

    public get RequestId(): string {
        return this.requestId;
    }

    public get ContentType(): string {
        return this.contentType;
    }

    public get AdditionalHeaders(): IStringDictionary<string> {
        return this.additionalHeaders;
    }

    public static FromConnectionMessage = (message: ConnectionMessage): SpeechConnectionMessage => {
        let path = null;
        let requestId = null;
        let contentType = null;
        let requestTimestamp = null;
        const additionalHeaders: IStringDictionary<string> = {};

        if (message.Headers) {
            for (const headerName in message.Headers) {
                if (headerName) {
                    if (headerName.toLowerCase() === PathHeaderName.toLowerCase()) {
                        path = message.Headers[headerName];
                    } else if (headerName.toLowerCase() === RequestIdHeaderName.toLowerCase()) {
                        requestId = message.Headers[headerName];
                    } else if (headerName.toLowerCase() === RequestTimestampHeaderName.toLowerCase()) {
                        requestTimestamp = message.Headers[headerName];
                    } else if (headerName.toLowerCase() === ContentTypeHeaderName.toLowerCase()) {
                        contentType = message.Headers[headerName];
                    } else {
                        additionalHeaders[headerName] = message.Headers[headerName];
                    }
                }
            }
        }

        return new SpeechConnectionMessage(
            message.MessageType,
            path,
            requestId,
            contentType,
            message.Body,
            additionalHeaders,
            message.Id);
    }
}
