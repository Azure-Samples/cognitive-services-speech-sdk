// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { IConnection } from "../common/Exports";
import { AuthInfo } from "./IAuthentication";
import { RecognizerConfig } from "./RecognizerConfig";

export interface IConnectionFactory {
    create(
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection;
}
