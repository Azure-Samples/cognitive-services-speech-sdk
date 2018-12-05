// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { AudioStreamFormatImpl } from "../../src/sdk/Audio/AudioStreamFormat";
import {
    IAudioStreamNode,
    IStreamChunk,
    Promise,
    PromiseHelper,
} from "../common/Exports";

export class ReplayableAudioNode implements IAudioStreamNode {
    private privAudioNode: IAudioStreamNode;
    private privFormat: AudioStreamFormatImpl;
    private privBuffers: BufferEntry[] = [];
    private privReplayOffset: number = 0;
    private privLastShrinkOffset: number = 0;
    private privBufferStartOffset: number = 0;
    private privBufferSerial: number = 0;
    private privBufferedBytes: number = 0;
    private privReplay: boolean = false;

    public constructor(audioSource: IAudioStreamNode, format: AudioStreamFormatImpl) {
        this.privAudioNode = audioSource;
        this.privFormat = format;
    }

    public id = (): string => {
        return this.privAudioNode.id();
    }

    // Reads and returns the next chunk of audio buffer.
    // If replay of existing buffers are needed, read() will first seek and replay
    // existing content, and upoin completion it will read new content from the underlying
    // audio node, saving that content into the replayable buffers.
    public read(): Promise<IStreamChunk<ArrayBuffer>> {
        // if there is a replay request to honor.
        if (!!this.privReplay && this.privBuffers.length !== 0) {
            // Find the start point in the buffers.
            // Offsets are in 100ns increments.
            // So how many bytes do we need to seek to get the right offset?
            const offsetToSeek: number = this.privReplayOffset - this.privBufferStartOffset;

            let bytesToSeek: number = Math.round(offsetToSeek * this.privFormat.avgBytesPerSec * 1e-7);
            if (0 !== (bytesToSeek % 2)) {
                bytesToSeek++;
            }

            let i: number = 0;

            while (i < this.privBuffers.length && bytesToSeek >= this.privBuffers[i].buffer.byteLength) {
                bytesToSeek -= this.privBuffers[i++].buffer.byteLength;
            }

            const retVal: ArrayBuffer = this.privBuffers[i].buffer.slice(bytesToSeek);

            this.privReplayOffset += (retVal.byteLength / this.privFormat.avgBytesPerSec) * 1e+7;

            // If we've reached the end of the buffers, stop replaying.
            if (i === this.privBuffers.length - 1) {
                this.privReplay = false;
            }

            return PromiseHelper.fromResult<IStreamChunk<ArrayBuffer>>({
                buffer: retVal,
                isEnd: false,
            });
        }

        return this.privAudioNode.read()
            .onSuccessContinueWith((result: IStreamChunk<ArrayBuffer>) => {
                if (result.buffer) {

                    this.privBuffers.push(new BufferEntry(result.buffer, this.privBufferSerial++, this.privBufferedBytes));
                    this.privBufferedBytes += result.buffer.byteLength;
                }
                return result;
            });
    }

    public detach(): void {
        this.privAudioNode.detach();
        this.privBuffers = undefined;
    }

    public replay(): void {
        if (0 !== this.privBuffers.length) {
            this.privReplay = true;
            this.privReplayOffset = this.privLastShrinkOffset;
        }
    }

    // Shrinks the existing audio buffers to start at the new offset, or at the
    // beginnign of the buffer closest to the requested offset.
    // A replay request will start from the last shrink point.
    public shrinkBuffers(offset: number): void {
        this.privLastShrinkOffset = offset;

        // Find the start point in the buffers.
        // Offsets are in 100ns increments.
        // So how many bytes do we need to seek to get the right offset?
        const offsetToSeek: number = offset - this.privBufferStartOffset;

        let bytesToSeek: number = Math.round(offsetToSeek * this.privFormat.avgBytesPerSec * 1e-7);

        let i: number = 0;

        while (i < this.privBuffers.length && bytesToSeek >= this.privBuffers[i].buffer.byteLength) {
            bytesToSeek -= this.privBuffers[i++].buffer.byteLength;
        }
        this.privBufferStartOffset = Math.round(offset - ((bytesToSeek / this.privFormat.avgBytesPerSec) * 1e+7));

        this.privBuffers = this.privBuffers.slice(i);
    }
}

// Primary use of this class is to help debugging problems with the replay
// code. If the memory cost of alloc / dealloc gets too much, drop it and just use
// the ArrayBuffer directly.
// tslint:disable-next-line:max-classes-per-file
class BufferEntry {
    public buffer: ArrayBuffer;
    public serial: number;
    public byteOffset: number;

    public constructor(buffer: ArrayBuffer, serial: number, byteOffset: number) {
        this.buffer = buffer;
        this.serial = serial;
        this.byteOffset = byteOffset;
    }
}
