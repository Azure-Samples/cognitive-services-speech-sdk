// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import { AudioStreamFormat } from "../../src/sdk/Exports";
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
    Format: AudioStreamFormat;
}

export interface IAudioStreamNode extends IDetachable {
    Id(): string;
    Read(): Promise<IStreamChunk<ArrayBuffer>>;
}
