import { RiffPcmEncoder, Stream } from "../common/Exports";
import { IRecorder } from "./IRecorder";

export class PcmRecorder implements IRecorder {
    private mediaResources: IMediaResources;

    public Record = (context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void => {
        const desiredSampleRate = 16000;

        // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createScriptProcessor
        const scriptNode = (() => {
            let bufferSize = 0;
            try {
                return context.createScriptProcessor(bufferSize, 1, 1);
            } catch (error) {
                // Webkit (<= version 31) requires a valid bufferSize.
                bufferSize = 2048;
                let audioSampleRate = context.sampleRate;
                while (bufferSize < 16384 && audioSampleRate >= (2 * desiredSampleRate)) {
                    bufferSize <<= 1 ;
                    audioSampleRate >>= 1;
                }
                return context.createScriptProcessor(bufferSize, 1, 1);
            }
        })();

        const waveStreamEncoder = new RiffPcmEncoder(context.sampleRate, desiredSampleRate);
        let needHeader: boolean = true;
        const that = this;
        scriptNode.onaudioprocess = (event: AudioProcessingEvent) => {
            const inputFrame = event.inputBuffer.getChannelData(0);

            if (outputStream && !outputStream.IsClosed) {
                const waveFrame = waveStreamEncoder.Encode(needHeader, inputFrame);
                if (!!waveFrame) {
                    outputStream.Write(waveFrame);
                    needHeader = false;
                }
            }
        };

        // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createMediaStreamSource
        const micInput = context.createMediaStreamSource(mediaStream);

        this.mediaResources = {
            scriptProcessorNode: scriptNode,
            source: micInput,
            stream: mediaStream,
        };

        micInput.connect(scriptNode);
        scriptNode.connect(context.destination);
    }

    public ReleaseMediaResources = (context: AudioContext): void => {
        if (this.mediaResources) {
            if (this.mediaResources.scriptProcessorNode) {
                this.mediaResources.scriptProcessorNode.disconnect(context.destination);
                this.mediaResources.scriptProcessorNode = null;
            }
            if (this.mediaResources.source) {
                this.mediaResources.source.disconnect();
                this.mediaResources.stream.getTracks().forEach((track: any) => track.stop());
                this.mediaResources.source = null;
            }
        }
    }
}

interface IMediaResources {
    source: MediaStreamAudioSourceNode;
    scriptProcessorNode: ScriptProcessorNode;
    stream: MediaStream;
}
