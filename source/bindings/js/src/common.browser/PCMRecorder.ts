// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { RiffPcmEncoder, Stream } from "../common/Exports";
import { IRecorder } from "./IRecorder";

export class PcmRecorder implements IRecorder {
    private privMediaResources: IMediaResources;

    public record = (context: AudioContext, mediaStream: MediaStream, outputStream: Stream<ArrayBuffer>): void => {
        const desiredSampleRate = 16000;

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

            if (outputStream && !outputStream.isClosed) {
                const waveFrame = waveStreamEncoder.encode(needHeader, inputFrame);
                if (!!waveFrame) {
                    outputStream.write(waveFrame);
                    needHeader = false;
                }
            }
        };

        const micInput = context.createMediaStreamSource(mediaStream);

        this.privMediaResources = {
            scriptProcessorNode: scriptNode,
            source: micInput,
            stream: mediaStream,
        };

        micInput.connect(scriptNode);
        scriptNode.connect(context.destination);
    }

    public releaseMediaResources = (context: AudioContext): void => {
        if (this.privMediaResources) {
            if (this.privMediaResources.scriptProcessorNode) {
                this.privMediaResources.scriptProcessorNode.disconnect(context.destination);
                this.privMediaResources.scriptProcessorNode = null;
            }
            if (this.privMediaResources.source) {
                this.privMediaResources.source.disconnect();
                this.privMediaResources.stream.getTracks().forEach((track: any) => track.stop());
                this.privMediaResources.source = null;
            }
        }
    }
}

interface IMediaResources {
    source: MediaStreamAudioSourceNode;
    scriptProcessorNode: ScriptProcessorNode;
    stream: MediaStream;
}
