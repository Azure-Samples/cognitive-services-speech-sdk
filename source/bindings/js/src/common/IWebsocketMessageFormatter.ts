// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ConnectionMessage } from "./ConnectionMessage";
import { Promise } from "./Promise";
import { RawWebsocketMessage } from "./RawWebsocketMessage";

export interface IWebsocketMessageFormatter {
    toConnectionMessage(message: RawWebsocketMessage): Promise<ConnectionMessage>;
    fromConnectionMessage(message: ConnectionMessage): Promise<RawWebsocketMessage>;
}
