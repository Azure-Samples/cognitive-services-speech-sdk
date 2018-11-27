// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { Stream } from "../common/Exports";
import { IRecorder } from "./IRecorder";

// getting around the build error for MediaRecorder as Typescript does not have a definition for this one.
declare var MediaRecorder: any;

export class OpusRecorder implements IRecorder {
    private privMediaResources: IMediaResources;
    private privMediaRecorderOptions: any;

    constructor(options?: { mimeType: string, bitsPerSecond: number }) {
        this.privMediaRecorderOptions = options;
    }

    public record = (context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void => {
        const mediaRecorder: any = new MediaRecorder(mediaStream, this.privMediaRecorderOptions);
        const timeslice = 100; // this is in ms - 100 ensures that the chunk doesn't exceed the max size of chunk allowed in WS connection
        mediaRecorder.ondataavailable = (dataAvailableEvent: any) => {
            if (outputStream) {
                const reader = new FileReader();
                reader.readAsArrayBuffer(dataAvailableEvent.data);
                reader.onloadend = (event: ProgressEvent) => {
                    outputStream.write(reader.result as ArrayBuffer);
                };
            }
        };

        this.privMediaResources = {
            recorder: mediaRecorder,
            stream: mediaStream,
        };
        mediaRecorder.start(timeslice);
    }

    public releaseMediaResources = (context: AudioContext): void => {
        if (this.privMediaResources.recorder.state !== "inactive") {
            this.privMediaResources.recorder.stop();
        }
        this.privMediaResources.stream.getTracks().forEach((track: any) => track.stop());
    }
}

interface IMediaResources {
    stream: MediaStream;
    recorder: any;
}

/* Declaring this inline to avoid compiler warnings
declare class MediaRecorder {
    constructor(mediaStream: MediaStream, options: any);

    public state: string;

    public ondataavailable(dataAvailableEvent: any): void;
    public stop(): void;
}*/
