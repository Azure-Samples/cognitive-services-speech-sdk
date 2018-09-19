//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { SynthesisStatus } from "./Exports";

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 * @class
 */
export class TranslationSynthesisResult {
    private privReason: SynthesisStatus;
    private privAudio: ArrayBuffer;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param reason The synthesis reason.
     * @param audio The audio data.
     */
    constructor(reason: SynthesisStatus, audio: ArrayBuffer) {
        this.privReason = reason;
        this.privAudio = audio;
    }

    /**
     * Translated text in the target language.
     * @property
     * @returns Translated audio in the target language.
     */
    public get audio(): ArrayBuffer {
        return this.privAudio;
    }

    /**
     * The synthesis status.
     * @property
     * @returns The synthesis status.
     */
    public get reason(): SynthesisStatus {
        return this.privReason;
    }
}
