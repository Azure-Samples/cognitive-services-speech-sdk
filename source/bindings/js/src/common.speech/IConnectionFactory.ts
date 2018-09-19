//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { IConnection } from "../common/Exports";
import { AuthInfo } from "./IAuthentication";
import { RecognizerConfig } from "./RecognizerConfig";

export interface IConnectionFactory {
    Create(
        config: RecognizerConfig,
        authInfo: AuthInfo,
        connectionId?: string): IConnection;
}
