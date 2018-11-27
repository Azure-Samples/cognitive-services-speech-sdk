// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { Promise } from "../common/Exports";

export interface IAuthentication {
    fetch(authFetchEventId: string): Promise<AuthInfo>;
    fetchOnExpiry(authFetchEventId: string): Promise<AuthInfo>;
}

export class AuthInfo {
    private privHeaderName: string;
    private privToken: string;

    public constructor(headerName: string, token: string) {
        this.privHeaderName = headerName;
        this.privToken = token;
    }

    public get headerName(): string {
        return this.privHeaderName;
    }

    public get token(): string {
        return this.privToken;
    }
}
