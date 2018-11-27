// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

export class ConnectionOpenResponse {
    private privStatusCode: number;
    private privReason: string;

    constructor(statusCode: number, reason: string) {
        this.privStatusCode = statusCode;
        this.privReason = reason;
    }

    public get statusCode(): number {
        return this.privStatusCode;
    }

    public get reason(): string {
        return this.privReason;
    }
}
