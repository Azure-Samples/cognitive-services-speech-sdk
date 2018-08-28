import { AudioSourceEvent } from "./AudioSourceEvents";
import { EventSource } from "./EventSource";
import { IDetachable } from "./IDetachable";
import { Promise } from "./Promise";
import { IStreamChunk } from "./Stream";

export interface IAudioSource {
    Id(): string;
    TurnOn(): Promise<boolean>;
    Attach(audioNodeId: string): Promise<IAudioStreamNode>;
    Detach(audioNodeId: string): void;
    TurnOff(): Promise<boolean>;
    Events: EventSource<AudioSourceEvent>;
}

export interface IAudioStreamNode extends IDetachable {
    Id(): string;
    Read(): Promise<IStreamChunk<ArrayBuffer>>;
}
