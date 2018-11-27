// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { MessageType } from "./ConnectionMessage";
import { ArgumentNullError, InvalidOperationError } from "./Error";
import { createNoDashGuid } from "./Guid";

export class RawWebsocketMessage {
    private privMessageType: MessageType;
    private privPayload: any = null;
    private privId: string;

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

        this.privMessageType = messageType;
        this.privPayload = payload;
        this.privId = id ? id : createNoDashGuid();
    }

    public get messageType(): MessageType {
        return this.privMessageType;
    }

    public get payload(): any {
        return this.privPayload;
    }

    public get textContent(): string {
        if (this.privMessageType === MessageType.Binary) {
            throw new InvalidOperationError("Not supported for binary message");
        }

        return this.privPayload as string;
    }

    public get binaryContent(): ArrayBuffer {
        if (this.privMessageType === MessageType.Text) {
            throw new InvalidOperationError("Not supported for text message");
        }

        return this.privPayload;
    }

    public get id(): string {
        return this.privId;
    }
}
