import { Stream } from "../common/Exports";
import { IRecorder } from "./IRecorder";

// getting around the build error for MediaRecorder as Typescript does not have a definition for this one.
declare var MediaRecorder: any;

export class OpusRecorder implements IRecorder {
    private mediaResources: IMediaResources;
    private mediaRecorderOptions: any;

    constructor(options?: { mimeType: string, bitsPerSecond: number }) {
        this.mediaRecorderOptions = options;
    }

    public Record = (context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void => {
        const mediaRecorder: any = new MediaRecorder(mediaStream, this.mediaRecorderOptions);
        const timeslice = 100; // this is in ms - 100 ensures that the chunk doesn't exceed the max size of chunk allowed in WS connection
        mediaRecorder.ondataavailable = (dataAvailableEvent: any) => {
            if (outputStream) {
                const reader = new FileReader();
                reader.readAsArrayBuffer(dataAvailableEvent.data);
                reader.onloadend = (event: ProgressEvent) => {
                    outputStream.Write(reader.result);
                };
            }
        };

        this.mediaResources = {
            recorder: mediaRecorder,
            stream: mediaStream,
        };
        mediaRecorder.start(timeslice);
    }

    public ReleaseMediaResources = (context: AudioContext): void => {
        if (this.mediaResources.recorder.state !== "inactive") {
            this.mediaResources.recorder.stop();
        }
        this.mediaResources.stream.getTracks().forEach((track: any) => track.stop());
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
