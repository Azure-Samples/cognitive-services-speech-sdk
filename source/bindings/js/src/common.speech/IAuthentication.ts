// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import { Promise } from "../common/Exports";

export interface IAuthentication {
    Fetch(authFetchEventId: string): Promise<AuthInfo>;
    FetchOnExpiry(authFetchEventId: string): Promise<AuthInfo>;
}

export class AuthInfo {
    private headerName: string;
    private token: string;

    public constructor(headerName: string, token: string) {
        this.headerName = headerName;
        this.token = token;
    }

    public get HeaderName(): string {
        return this.headerName;
    }

    public get Token(): string {
        return this.token;
    }
}
