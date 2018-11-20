// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import { Stream } from "../common/Exports";

export interface IRecorder {
    Record(context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void;
    ReleaseMediaResources(context: AudioContext): void;
}
