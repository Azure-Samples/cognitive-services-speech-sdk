import { InvalidOperationError } from "./Error";
import { CreateNoDashGuid } from "./Guid";
import { IStringDictionary } from "./IDictionary";

export enum MessageType {
    Text,
    Binary,
}

export class ConnectionMessage {

    private messageType: MessageType;
    private headers: IStringDictionary<string>;
    private body: any = null;

    private id: string;

    public constructor(
        messageType: MessageType,
        body: any,
        headers?: IStringDictionary<string>,
        id?: string) {

        if (messageType === MessageType.Text && body && !(typeof (body) === "string")) {
            throw new InvalidOperationError("Payload must be a string");
        }

        if (messageType === MessageType.Binary && body && !(body instanceof ArrayBuffer)) {
            throw new InvalidOperationError("Payload must be ArrayBuffer");
        }

        this.messageType = messageType;
        this.body = body;
        this.headers = headers ? headers : {};
        this.id = id ? id : CreateNoDashGuid();
    }

    public get MessageType(): MessageType {
        return this.messageType;
    }

    public get Headers(): any {
        return this.headers;
    }

    public get Body(): any {
        return this.body;
    }

    public get TextBody(): string {
        if (this.messageType === MessageType.Binary) {
            throw new InvalidOperationError("Not supported for binary message");
        }

        return this.body as string;
    }

    public get BinaryBody(): ArrayBuffer {
        if (this.messageType === MessageType.Text) {
            throw new InvalidOperationError("Not supported for text message");
        }

        return this.body;
    }

    public get Id(): string {
        return this.id;
    }
}
