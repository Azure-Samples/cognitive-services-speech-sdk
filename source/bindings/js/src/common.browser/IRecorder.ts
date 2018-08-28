import { Stream } from "../common/Exports";

export interface IRecorder {
    Record(context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void;
    ReleaseMediaResources(context: AudioContext): void;
}
