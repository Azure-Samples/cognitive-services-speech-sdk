//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { Stream } from "../common/Exports";

export interface IRecorder {
    Record(context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void;
    ReleaseMediaResources(context: AudioContext): void;
}
