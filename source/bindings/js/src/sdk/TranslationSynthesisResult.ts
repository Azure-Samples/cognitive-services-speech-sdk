//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { SynthesisStatus } from "./Exports";

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 * @class TranslationSynthesisResult
 */
export class TranslationSynthesisResult {
    private privReason: SynthesisStatus;
    private privAudio: ArrayBuffer;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {SynthesisStatus} reason - The synthesis reason.
     * @param {ArrayBuffer} audio - The audio data.
     */
    constructor(reason: SynthesisStatus, audio: ArrayBuffer) {
        this.privReason = reason;
        this.privAudio = audio;
    }

    /**
     * Translated text in the target language.
     * @member TranslationSynthesisResult.prototype.audio
     * @function
     * @public
     * @returns Translated audio in the target language.
     */
    public get audio(): ArrayBuffer {
        return this.privAudio;
    }

    /**
     * The synthesis status.
     * @member TranslationSynthesisResult.prototype.reason
     * @function
     * @public
     * @returns The synthesis status.
     */
    public get reason(): SynthesisStatus {
        return this.privReason;
    }
}
