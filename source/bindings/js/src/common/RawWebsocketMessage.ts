import { MessageType } from "./ConnectionMessage";
import { ArgumentNullError, InvalidOperationError } from "./Error";
import { CreateNoDashGuid } from "./Guid";

export class RawWebsocketMessage {

    private messageType: MessageType;
    private payload: any = null;
    private id: string;

    public constructor(messageType: MessageType, payload: any, id?: string) {
        if (!payload) {
            throw new ArgumentNullError("payload");
        }

        if (messageType === MessageType.Binary && !(payload instanceof ArrayBuffer)) {
            throw new InvalidOperationError("Payload must be ArrayBuffer");
        }

        if (messageType === MessageType.Text && !(typeof (payload) === "string")) {
            throw new InvalidOperationError("Payload must be a string");
        }

        this.messageType = messageType;
        this.payload = payload;
        this.id = id ? id : CreateNoDashGuid();
    }

    public get MessageType(): MessageType {
        return this.messageType;
    }

    public get Payload(): any {
        return this.payload;
    }

    public get TextContent(): string {
        if (this.messageType === MessageType.Binary) {
            throw new InvalidOperationError("Not supported for binary message");
        }

        return this.payload as string;
    }

    public get BinaryContent(): ArrayBuffer {
        if (this.messageType === MessageType.Text) {
            throw new InvalidOperationError("Not supported for text message");
        }

        return this.payload;
    }

    public get Id(): string {
        return this.id;
    }
}
