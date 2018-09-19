//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { ConnectionMessage } from "./ConnectionMessage";
import { Promise } from "./Promise";
import { RawWebsocketMessage } from "./RawWebsocketMessage";

export interface IWebsocketMessageFormatter {
    ToConnectionMessage(message: RawWebsocketMessage): Promise<ConnectionMessage>;
    FromConnectionMessage(message: ConnectionMessage): Promise<RawWebsocketMessage>;
}
