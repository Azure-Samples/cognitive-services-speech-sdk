// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { InvalidOperationError } from "./Error";
import { createNoDashGuid } from "./Guid";
import { IStringDictionary } from "./IDictionary";

export enum MessageType {
    Text,
    Binary,
}

export class ConnectionMessage {

    private privMessageType: MessageType;
    private privHeaders: IStringDictionary<string>;
    private privBody: any = null;

    private privId: string;

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

        this.privMessageType = messageType;
        this.privBody = body;
        this.privHeaders = headers ? headers : {};
        this.privId = id ? id : createNoDashGuid();
    }

    public get messageType(): MessageType {
        return this.privMessageType;
    }

    public get headers(): any {
        return this.privHeaders;
    }

    public get body(): any {
        return this.privBody;
    }

    public get textBody(): string {
        if (this.privMessageType === MessageType.Binary) {
            throw new InvalidOperationError("Not supported for binary message");
        }

        return this.privBody as string;
    }

    public get binaryBody(): ArrayBuffer {
        if (this.privMessageType === MessageType.Text) {
            throw new InvalidOperationError("Not supported for text message");
        }

        return this.privBody;
    }

    public get id(): string {
        return this.privId;
    }
}
