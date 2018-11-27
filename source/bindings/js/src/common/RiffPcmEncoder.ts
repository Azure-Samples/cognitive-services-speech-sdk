// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

export class RiffPcmEncoder {

    private privActualSampleRate: number;
    private privDesiredSampleRate: number;
    private privChannelCount: number = 1;

    public constructor(actualSampleRate: number, desiredSampleRate: number) {
        this.privActualSampleRate = actualSampleRate;
        this.privDesiredSampleRate = desiredSampleRate;
    }

    public encode = (
        needHeader: boolean,
        actualAudioFrame: Float32Array): ArrayBuffer => {

        const audioFrame = this.downSampleAudioFrame(actualAudioFrame, this.privActualSampleRate, this.privDesiredSampleRate);

        if (!audioFrame) {
            return null;
        }

        const audioLength = audioFrame.length * 2;

        if (!needHeader) {
            const buffer = new ArrayBuffer(audioLength);
            const view = new DataView(buffer);
            this.floatTo16BitPCM(view, 0, audioFrame);

            return buffer;
        }

        const buffer = new ArrayBuffer(44 + audioLength);

        const bitsPerSample = 16;
        const bytesPerSample = bitsPerSample / 8;
        // We dont know ahead of time about the length of audio to stream. So set to 0.
        const fileLength = 0;

        // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView
        const view = new DataView(buffer);

        /* RIFF identifier */
        this.setString(view, 0, "RIFF");
        /* file length */
        view.setUint32(4, fileLength, true);
        /* RIFF type & Format */
        this.setString(view, 8, "WAVEfmt ");
        /* format chunk length */
        view.setUint32(16, 16, true);
        /* sample format (raw) */
        view.setUint16(20, 1, true);
        /* channel count */
        view.setUint16(22, this.privChannelCount, true);
        /* sample rate */
        view.setUint32(24, this.privDesiredSampleRate, true);
        /* byte rate (sample rate * block align) */
        view.setUint32(28, this.privDesiredSampleRate * this.privChannelCount * bytesPerSample, true);
        /* block align (channel count * bytes per sample) */
        view.setUint16(32, this.privChannelCount * bytesPerSample, true);
        /* bits per sample */
        view.setUint16(34, bitsPerSample, true);
        /* data chunk identifier */
        this.setString(view, 36, "data");
        /* data chunk length */
        view.setUint32(40, fileLength, true);

        this.floatTo16BitPCM(view, 44, audioFrame);

        return buffer;
    }

    private setString = (view: DataView, offset: number, str: string): void => {
        for (let i = 0; i < str.length; i++) {
            view.setUint8(offset + i, str.charCodeAt(i));
        }
    }

    private floatTo16BitPCM = (view: DataView, offset: number, input: Float32Array): void => {
        for (let i = 0; i < input.length; i++ , offset += 2) {
            const s = Math.max(-1, Math.min(1, input[i]));
            view.setInt16(offset, s < 0 ? s * 0x8000 : s * 0x7FFF, true);
        }
    }

    private downSampleAudioFrame = (
        srcFrame: Float32Array,
        srcRate: number,
        dstRate: number): Float32Array => {

        if (dstRate === srcRate || dstRate > srcRate) {
            return srcFrame;
        }

        const ratio = srcRate / dstRate;
        const dstLength = Math.round(srcFrame.length / ratio);
        const dstFrame = new Float32Array(dstLength);
        let srcOffset = 0;
        let dstOffset = 0;
        while (dstOffset < dstLength) {
            const nextSrcOffset = Math.round((dstOffset + 1) * ratio);
            let accum = 0;
            let count = 0;
            while (srcOffset < nextSrcOffset && srcOffset < srcFrame.length) {
                accum += srcFrame[srcOffset++];
                count++;
            }
            dstFrame[dstOffset++] = accum / count;
        }

        return dstFrame;
    }
}
