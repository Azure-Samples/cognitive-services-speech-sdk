import { ConnectionMessage } from "./ConnectionMessage";
import { Promise } from "./Promise";
import { RawWebsocketMessage } from "./RawWebsocketMessage";

export interface IWebsocketMessageFormatter {
    ToConnectionMessage(message: RawWebsocketMessage): Promise<ConnectionMessage>;
    FromConnectionMessage(message: ConnectionMessage): Promise<RawWebsocketMessage>;
}
